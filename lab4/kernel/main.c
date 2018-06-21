/*##############################################################

@change		增加了进程新增变量的初始化
			修改了不同进程的颜色显示
			新增了顾客进程，理发师进程的实现
@intent		主程序

################################################################*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "queue.h"

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");
	
	struct task* p_task;	
	struct proc* p_proc= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
	u8    privilege;
    u8    rpl;
	int   eflags;
	int   i;
	int   prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
				p_task    = task_table + i;
				privilege = PRIVILEGE_TASK;
				rpl       = RPL_TASK;
				eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
				prio      = 15;
            }
			else {                  /* 用户进程 */
				p_task    = user_proc_table + (i - NR_TASKS);
				privilege = PRIVILEGE_USER;
				rpl       = RPL_USER;
				eflags    = 0x202; /* IF=1, bit 2 is always 1 */
				prio      = 5;
			}

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		p_proc->nr_tty		= 0;
		
		//新增变量的初始化
		p_proc->call_sleep_moment = 0;
		p_proc->sleep_ticks = 0;
		p_proc->expect_color = DEFAULT_CHAR_COLOR;
		
		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;
                
		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	
	k_reenter = 0;
	ticks = 0;
    
	p_proc_ready = proc_table;
	
	proc_table[NR_TASKS + 0].priority = 10;
	proc_table[NR_TASKS + 1].priority = 1;
	proc_table[NR_TASKS + 2].priority = 1;
	proc_table[NR_TASKS + 3].priority = 1;
	
	proc_table[NR_TASKS + 0].expect_color = PURPLE_BLUE;//理发师进程
	proc_table[NR_TASKS + 1].expect_color = BRIGHT_YELLOW;//顾客进程1
	proc_table[NR_TASKS + 2].expect_color = GRASS_GREEN;//顾客进程2
	proc_table[NR_TASKS + 3].expect_color = GREEN;//顾客进程3
	
	//控制变量、信号量、记录量的初始化
	waiting = 0;
	CHAIRS = 3;  //TODO here to change!
	customers.value = 0;
	barbers.value = 0;
	mutex.value = 1;
	customerID = 0;
	 
	//顾客等待队列的初始化
	initQueue(&waitingCustomersQueue,CHAIRS);
	
	init_clock();
    init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
/*PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}*/

//理发
PUBLIC void cutHair(){
	int currentCustomer = deQueue(&waitingCustomersQueue);
	int currentCustomerID = currentCustomer % 100;	 //当前顾客的编号
	int currentCustomerColor = currentCustomer / 100;//当前顾客所属进程的颜色
	
	proc_table[NR_TASKS + 0].expect_color = PURPLE_BLUE;	//下一句打印理发师行为，用绿色输出
	printf("Barber begins cutting #%d customer's hair. \n", currentCustomerID);

	process_sleep(20);									//睡眠
	proc_table[NR_TASKS + 0].expect_color = (char)currentCustomerColor;	//下一句打印顾客行为，用所属进程的颜色输出
	printf("#%d customer leaves with new hair style.^_^\n", currentCustomerID);
}

//理发师行为：参考书P143源码
PUBLIC void barberAction(){
	while(1){
		if(waiting==0)
			printf("No customers. Barber sleeps. zzz~\n");
		sem_p(&customers);								//申请一个顾客资源，如果现在没有顾客，则理发师等待
		sem_p(&mutex);									//直至有顾客可以接受服务
		waiting--;										//等候顾客数减1
		proc_table[NR_TASKS + 0].expect_color = PURPLE_BLUE;	//改回理发师进程的绿色
		
		printf("Barber calls next customer! Then there are %d customers waiting...\n", waiting);
		sem_v(&barbers);								//释放理发师资源（挂起的顾客进程可以得到理发服务）
		sem_v(&mutex);									//退出临界区
		cutHair();
	}
}
	
//顾客行为：参考书P143源码
PUBLIC void customerAction(int i){
	while(1){
		sem_p(&mutex);									//进入临界区
		customerID += 1;								//顾客编号加1
		printf("#%d customer comes. %d customers are waiting......", customerID,waiting);
		
		if(waiting < CHAIRS){						//判断：有空椅子
			waiting++;								//等候顾客数加1						
			printf("Sits down to wait.\n");
			char customerColor = proc_table[NR_TASKS + i].expect_color;//获取顾客进程对应的color属性的值
			enQueue(&waitingCustomersQueue,customerID, customerColor);//将进程的颜色值存入队列，方便在cutHair中获取并输出
			sem_v(&customers);							//释放顾客资源（挂起的理发师进程可以提供理发服务）
			sem_v(&mutex);								//退出临界区
			sem_p(&barbers);							//如果理发师忙，顾客则等待
		}
		else{
			printf("No more chairs, 886!\n");
			sem_v(&mutex);								//没有空椅子：顾客离开
		}
		
	}
}

/*======================================================================*
								普通进程
 *======================================================================*/
void TestA()
{
	while (1){
		//无限循环
		//milli_delay(50000);
		//clean_screen();
	}
}

/*======================================================================*
								理发师进程
 *======================================================================*/
void TestB(){
	barberAction();
}

/*======================================================================*
								顾客进程
 *======================================================================*/
 void TestC(){
	customerAction(1);
}

void TestD(){
    customerAction(2);
}

void TestE(){
    customerAction(3);
}


/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

