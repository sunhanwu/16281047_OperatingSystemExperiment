#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<semaphore.h>
volatile int counter=0;
int loops;
sem_t *mySem = NULL;
void *worker(void *arg)
{
	int i;
	for(i=0;i<loops;i++)
	{
		sem_wait(mySem);
		counter++;
		sem_post(mySem);
	}
	return NULL;
}

int main(int argc,char *argv[])
{
	
	if(argc!=2)
	{
		fprintf(stderr,"usage: thread <value>\n");
		exit(1);
	}
	loops=atoi(argv[1]);
	pthread_t p1,p2;
	printf("Initial value: %d\n",counter);
	mySem = sem_open("mySemName",O_CREAT,0666,1);
	pthread_create(&p1,NULL,worker,NULL);
	pthread_create(&p2,NULL,worker,NULL);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	printf("Final value: %d\n",counter);
	sem_close(mySem);
	sem_unlink("mySemName");
	return 0;

}
