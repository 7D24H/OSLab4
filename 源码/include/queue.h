/*##############################################################

@author 	Jingqi Wu

@change		等待队列waitingQueue的结构定义，保存等待理发师理发的顾客编号

@intent		用于理发店顾客的等待队列

################################################################*/

struct waitingQueue {
	int maxLen;				/* 队列的最大长度取决于椅子的个数 */
	int actualLen;			/* 实际队列的长度，不超过最大长度 */
	int queueHead;			/* 现在队列的头*/
	int idQueue[5];			/* 用于存放等待用户id的数组*/
	char colorQueue[5];		/* 一个用于存放用户进程颜色的数组*/
};


PUBLIC void initQueue(struct waitingQueue * customerQueue, int maxQueueLen);
PUBLIC void enQueue(struct waitingQueue * customerQueue, int customerID, char procColor);
PUBLIC int deQueue(struct waitingQueue * customerQueue);
PUBLIC void printQueue(struct waitingQueue * customerQueue);


