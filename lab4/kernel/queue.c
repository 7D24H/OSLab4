/*##############################################################

@change		实现了等待队列的初始化，进队列，出队列，打印队列

@intent		用于顾客队列

################################################################*/


#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"

#include "proc.h"
#include "global.h"
#include "proto.h"

#include "queue.h"


/*======================================================================*
							initQueue
*======================================================================*/
PUBLIC void initQueue(struct waitingQueue * customerQueue, int maxQueueLen){
	for(int i=0; i<maxQueueLen; i++){
		customerQueue->idQueue[i] = 0;
		customerQueue->colorQueue[i] = 0x00;
	}
	customerQueue->maxLen = maxQueueLen;
	customerQueue->actualLen = 0;
	customerQueue->queueHead = 0;
}

/*======================================================================*
							enQueue
*======================================================================*/
PUBLIC void enQueue(struct waitingQueue * customerQueue, int customerID, char procColor){
	if(customerQueue->actualLen < customerQueue->maxLen){
		int insertPosition = (customerQueue->queueHead + customerQueue->actualLen) % customerQueue->maxLen;
		customerQueue->idQueue[insertPosition] = customerID;
		customerQueue->colorQueue[insertPosition] = procColor;
		customerQueue->actualLen++;
	}
}

/*======================================================================*
							deQueue
*======================================================================*/
PUBLIC int deQueue(struct waitingQueue * customerQueue){
	int saveID = customerQueue->idQueue[customerQueue->queueHead];
	char saveColor = customerQueue->colorQueue[customerQueue->queueHead];
	customerQueue->queueHead = (customerQueue->queueHead + 1) % customerQueue->maxLen;
	customerQueue->actualLen--;
	return (saveColor * 100 + saveID);
}

/*======================================================================*
							printQueue
*======================================================================*/
PUBLIC void printQueue(struct waitingQueue * customerQueue){
	printf("Queue actualLen: %d.   Show the queue:",customerQueue->actualLen);
	if(customerQueue->actualLen == 0){
		printf("Empty Queue\n");
	}
	else{
		for(int i=0; i<customerQueue->actualLen; i++){
			printf("ID:%d, color:%c; ",customerQueue->idQueue[(customerQueue->queueHead+i)%customerQueue->maxLen],
									   customerQueue->colorQueue[(customerQueue->queueHead+i)%customerQueue->maxLen]);
		}
		printf("\n");
	}
}