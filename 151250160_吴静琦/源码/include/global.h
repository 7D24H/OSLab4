/*##############################################################

@author 	Jingqi Wu

@change		新增了等待数，椅子数，顾客和理发师的信号量等

@intent		全局变量的定义

################################################################*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int	ticks;
EXTERN	int	disp_pos;

EXTERN	u8			gdt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	struct descriptor	gdt[GDT_SIZE];
EXTERN	u8			idt_ptr[6];	/* 0~15:Limit  16~47:Base */
EXTERN	struct gate		idt[IDT_SIZE];

EXTERN	u32	k_reenter;
EXTERN	int	nr_current_console;
EXTERN  int     current_proc;
EXTERN	struct tss	tss;
EXTERN	struct proc*	p_proc_ready;

EXTERN	char		task_stack[];
EXTERN	struct proc	proc_table[];
EXTERN  struct task	task_table[];
EXTERN  struct task	user_proc_table[];
EXTERN	irq_handler	irq_table[];
EXTERN	TTY		tty_table[];
EXTERN  CONSOLE		console_table[];

//global.c中新增变量的声明
EXTERN int waiting;
EXTERN int CHAIRS;
EXTERN struct semaphore customers, barbers, mutex;
EXTERN int customerID;
EXTERN struct waitingQueue waitingCustomersQueue;