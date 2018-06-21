/*##############################################################

@change		修改了输出的字符颜色
			只使用一个控制台

@intent		控制台

################################################################*/

/*
	回车键: 把光标移到第一列
	换行键: 把光标前进到下一行
*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);

/*======================================================================*
			   init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD) */

	int con_v_mem_size                   = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr      = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit        = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	//默认的第一个控制台
	if (nr_tty == 0) {
		//清屏
		disp_pos = 0;

		for(int i=0; i<80*23; i++){
			out_char(p_tty->p_console, ' ');
		}
		
		p_tty->p_console->cursor = 0;//不再沿用原来的位置，光标从屏幕左上角重新开始
		disp_pos = 0;
	}
	else {
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}

	set_cursor(p_tty->p_console->cursor);
}

/*======================================================================*
@change 增加的用于清屏的方法
*======================================================================*/
//PUBLIC void clean_screen()
//{
//	disable_int();
//
//	disp_pos=0;
//	int i=0;
//	for(;i<80*25;i++){
//		disp_str(" ");
//	}
//	disp_pos=0;
//
//	init_screen(tty_table);
//
//	enable_int();
//}


/*======================================================================*
			   is_current_console
*======================================================================*/
PUBLIC int is_current_console(CONSOLE* p_con)
{
	return (p_con == &console_table[nr_current_console]);
}

/*======================================================================*
			   out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE* p_con, char ch)
{
	//修改了颜色，从进程p中得到它对应的颜色属性
	char ch_color = p_proc_ready->expect_color;

	//V_MEM_BASE:base of color video memory
	u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);
	
	//改变了以下内容
	switch(ch) {
	case '\n':
		if (p_con->cursor < p_con->original_addr +
		    p_con->v_mem_limit - SCR_WIDTH) {	
			p_con->cursor = p_con->original_addr + SCR_WIDTH * 
				((p_con->cursor - p_con->original_addr) /
				 SCR_WIDTH + 1);
		}
		//当输出一行结束后，如果已经到了显示屏最后，等一段时间，输出End a screen
//		else {
//			int o = 0;
//			int l = 0;
//			int p = 0;
//			for(;o<200000;o++){
//				for(;l<10000000;l++){
//					for(;p<1000000;p++){}
//				}
//			}
//			printf("End a screen!\n");
//		}
		break;
	case '\b':
		if (p_con->cursor > p_con->original_addr) {
			p_con->cursor--;
			*(p_vmem-2) = ' ';
			*(p_vmem-1) = DEFAULT_CHAR_COLOR;
		}
		break;
	default:
		if (p_con->cursor <
		    p_con->original_addr + p_con->v_mem_limit - 1) {
			*p_vmem++ = ch; //QHD 當前這個光標位置填充字符 ++後置執行
			*p_vmem++ = ch_color; //設置字符的顏色
			p_con->cursor++;
		}
		break;
	}


//	while(p_con->cursor >= p_con->current_start_addr + SCR_SIZE) {
//		scroll_screen(p_con, SCR_DN);
//	}


	flush(p_con);
}

/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE* p_con)
{
	if (is_current_console(p_con)) {
		set_cursor(p_con->cursor);
		set_video_start_addr(p_con->current_start_addr);
	}
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}



/*======================================================================*
			   select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console)	/* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES)) {
		return;
	}

	nr_current_console = nr_console;

	flush(&console_table[nr_console]);
}

/*======================================================================*
			   scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCR_WIDTH;
		}
	}
	else if (direction == SCR_DN) {
		if (p_con->current_start_addr + SCR_SIZE <
		    p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCR_WIDTH;
		}
	}
	else{
	}

	flush(p_con);
}
