/*##############################################################

@author 	Jingqi Wu

@change		进程结构体的修改，新增与睡眠相关的变量
			新增信号量的定义，详情参考书上P136
			修改任务数，进程数
			新增两个栈

@intent		进程的一些定义

################################################################*/

struct stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;			/* ┓						│			*/
	u32	fs;			/* ┃						│			*/
	u32	es;			/* ┃						│			*/
	u32	ds;			/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()		│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;			/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;			/*  ┛						┷High			*/
};


struct proc {
	struct stackframe regs;    /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	struct descriptor ldts[LDT_SIZE]; /* local descs for code and data */

        int ticks;                 /* remained ticks */
        int priority;

	u32 pid;                   /* process id passed in from MM */
	char name[16];		   /* name of the process */

	int  p_flags;              /**
				    * process flags.
				    * A proc is runnable iff p_flags==0
				    */

	MESSAGE * p_msg;
	int p_recvfrom;
	int p_sendto;

	int has_int_msg;  

	struct proc * q_sending;
	struct proc * next_sending;
	int nr_tty;
	
	int call_sleep_moment;	//调用process_sleep方法的时间
    int sleep_ticks;		//睡眠时间
	char expect_color;		//希望以什么颜色输出
};

struct task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
};

//@change 信号量的定义
struct semaphore {
  int value;                
  int len;
  struct proc * list[10];
};

#define proc2pid(x) (x - proc_table)

/* Number of tasks & procs */
#define NR_TASKS	3
#define NR_PROCS	4
#define FIRST_PROC	proc_table[0]
#define LAST_PROC	proc_table[NR_TASKS + NR_PROCS - 1]

/* stacks of tasks */
#define STACK_SIZE_TTY		0x8000
#define STACK_SIZE_SYS		0x8000
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
//新增进程D、E的栈大小也是0x8000
#define STACK_SIZE_TESTD	0x8000
#define STACK_SIZE_TESTE	0x8000

//栈的全部大小要加上新增的2个进程的栈大小
#define STACK_SIZE_TOTAL	(STACK_SIZE_TTY + \
							 STACK_SIZE_SYS + \
							 STACK_SIZE_TESTA + \
							 STACK_SIZE_TESTB + \
							 STACK_SIZE_TESTC + \
							 STACK_SIZE_TESTD + \
							 STACK_SIZE_TESTE\
                            )

