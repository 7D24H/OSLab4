/*##############################################################

@author 	Jingqi Wu

@change		和global.h中相对应的增加了与理发师pv操作相关的一些变量

@intent		全局变量的声明

################################################################*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "queue.h"


PUBLIC	struct proc	proc_table[NR_TASKS + NR_PROCS];

//要求A进程不会调用sleep，所以将进程A变为系统进程
PUBLIC	struct task	task_table[NR_TASKS] = {
	{task_tty, STACK_SIZE_TTY, "TTY"},
	{task_sys, STACK_SIZE_SYS, "SYS"},
    {TestA, STACK_SIZE_TESTA, "PROCA"}
};

//B、C、D、E进程为用户进程
PUBLIC	struct task	user_proc_table[NR_PROCS] = {
	{TestB, STACK_SIZE_TESTB, "Barber"},
	{TestC, STACK_SIZE_TESTC, "Customer1"},
	{TestD, STACK_SIZE_TESTD, "Customer2"},
	{TestE, STACK_SIZE_TESTE, "Customer3"}
};

PUBLIC	char		task_stack[STACK_SIZE_TOTAL];

PUBLIC	TTY		tty_table[NR_CONSOLES];
PUBLIC	CONSOLE		console_table[NR_CONSOLES];

PUBLIC	irq_handler	irq_table[NR_IRQ];

//新增系统调用的声明
PUBLIC	system_call	sys_call_table[NR_SYS_CALL] = {sys_printx,sys_sendrec,
												sys_process_sleep, sys_new_disp_str, 
												sys_sem_p, sys_sem_v, 
												sys_get_ticks};
							   
//增加了以下变量
PUBLIC int waiting;		//等待理发的顾客人数，初值为0
PUBLIC int CHAIRS;		//为顾客准备的椅子数，初值为3
PUBLIC struct semaphore customers,	//记录等候理发的顾客数，并用于阻塞理发师进程（本程序中customers的值最大为CHAIRS的值）
						barbers,	//记录正在等候顾客的理发师数，并用于阻塞顾客进程（本程序中barbers的值最大为1，因为只有1个理发师进程）
						mutex;		//用于互斥
PUBLIC int customerID;	//顾客编号，初值为0
PUBLIC struct waitingQueue waitingCustomersQueue;//等待理发师服务的顾客队列