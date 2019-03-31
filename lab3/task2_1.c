#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
volatile int ticketCount=1000;
/* sem_t *flag=NULL; */
void *SaleThread(void *arg)
{
	int num,temp;
	num=atoi(arg);
	for(int i=0;i<num;i++)
	{
		if(i % 10 ==0)
			printf("卖%d张票,剩余%d张票\n",i,ticketCount);
		/* sem_wait(flag); */
		temp=ticketCount;
		//放弃CPU，强制切换到另外一个进程
		pthread_yield();
		temp=temp-1;
		ticketCount-=1;
		pthread_yield();
		ticketCount=temp;
		/* sem_post(flag); */
	}
	return NULL;
}

void *RefundThread(void *arg)
{
	int num,temp;
	num=atoi(arg);
	for(int i=0;i<num;i++)
	{
		if(i % 10 ==0)
			printf("退%d张票，剩余%d张票\n",i,ticketCount);
		/* sem_wait(flag); */
		temp=ticketCount;
		pthread_yield();
		temp=temp+1;
		ticketCount+=1;
		pthread_yield();
		ticketCount=temp;
		/* sem_post(flag); */
	}
	return NULL;
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		printf("请正确输入参数！\n");
		exit(0);
	}
	/* flag=sem_open("flag",O_CREAT,0666,1); */
	printf("初始票数为：%d\n",ticketCount);
	pthread_t p1,p2;
	/* printf("%s %s",argv[1],argv[2]); */
	pthread_create(&p1,NULL,SaleThread,argv[1]);
	pthread_create(&p2,NULL,RefundThread,argv[2]);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	printf("最终票数为：%d\n",ticketCount);
	/* sem_close(flag); */
	/* sem_unlink("flag"); */
	return 0;

}
