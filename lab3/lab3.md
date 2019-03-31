# 实验三 实验报告

> 孙汉武	安全1601	16281047

[TOC]

## Task 1 

### 1.1 实验要求

$\qquad$通过fork的方式，产生4个进程P1,P2,P3,P4，每个进程打印输出自己的名字，例如P1输出“I am the process P1”。要求P1最先执行，P2、P3互斥执行，P4最后执行。通过多次测试验证实现是否正确。

### 1.2 实验过程

1. 实验源码

   `Task1.c`

   ```c++
   #include<stdio.h>
   #include<unistd.h>
   #include<stdlib.h>
   #include<pthread.h>
   #include<semaphore.h>
   #include<fcntl.h>
   int main()
   {
   	sem_t *P1_signal,*P2_signal,*P3_signal;
   	//主函数中的进程是P1
   	pid_t p2,p3,p4; 
   	P1_signal=sem_open("P1_signal",O_CREAT,0666,0);
   	P2_signal=sem_open("P2_signal",O_CREAT,0666,0);
   	P3_signal=sem_open("P3_signal",O_CREAT,0666,0);
   
   	p2=fork();//创建进程P2
   	if(p2<0)	
   	{
   		perror("创建进程p2出错！");
   	}
   	if(p2==0)
   	{
   		sem_wait(P1_signal);
   		printf("I am the process P2!\n");
   		sem_post(P1_signal);
   		sem_post(P2_signal);
   	}
   	if(p2>0)
   	{
   		p3=fork();
   		if(p3<0)	
   		{
   			perror("创建进程p出错！");
   		}
   		if(p3==0)
   		{
   			sem_wait(P1_signal);
   			printf("I am the process P3!\n");
   			sem_post(P1_signal);
   			sem_post(P3_signal);
   		}
   		if(p3>0)
   		{
   			printf("I am the process P1!\n");
   			sem_post(P1_signal);
   			p4=fork();
   			if(p4<0)	
   			{
   				perror("创建进程p4出错！");
   			}
   			if(p4==0)
   			{
   				sem_wait(P2_signal);
   				sem_wait(P3_signal);
   				printf("I am the process P4!\n");
   				sem_post(P2_signal);
   				sem_post(P3_signal);
   			}
   		}
   	}
   	sem_close(P1_signal);
   	sem_close(P3_signal);
   	sem_close(P2_signal);
   	sem_unlink("P1_signal");
   	sem_unlink("P2_signal");
   	sem_unlink("P3_signal");
   	return 0;
   }
   ```

2. 原理解释

   + 前趋图

     <img src="https://ws1.sinaimg.cn/large/006tKfTcly1g1jizdjii3j30xm0gcq3w.jpg" width="600">

     

     前驱关系：`P1-->P2`、`P1-->P3`、`P2-->P4`、`P3-->P4`

   + 前驱关系实现

     题目要求产生的四个进程必须是P1最先执行，P2、P3在P1执行完后互斥执行，P4最后执行。于是根据要求有了上面的前驱关系和前驱图。但是如何实现这种进程间的前驱关系呢？比较自然的想到了是用信号量机制。如上面的代码所示，定义了三个信号量，P1_signal、P2_signal和P3_signal，其初值均为0

     ```c
     P1_signal=sem_open("P1_signal",O_CREAT,0666,0);
     P2_signal=sem_open("P2_signal",O_CREAT,0666,0);
     P3_signal=sem_open("P3_signal",O_CREAT,0666,0);
     ```

     P1进程执行完打印任务之后对P1_signal信号量进行V操作，产生一个资源让等待P1_signal的进程P2和P3其中之一可以执行。由于P2和P3都是等待P1_signal信号量，但是P1进程只产生一个单位的信号，所以P2和P3的执行是互斥的，这样就满足了题目要求。最后在P2和P3执行完打印任务后对信号量P2_signal和P3_signal进行V操作从各产生一个单位的信号量，而进程P4会等待P2_signal和P3_signal，所以知道当P2和P3进程都完成才能进行P4进程。通过控制这三个信号量，这四个进程之间的前驱关系就满足了题目要求。

     <img src="https://ws1.sinaimg.cn/large/006tKfTcly1g1juc5uvifj30uk0ss778.jpg" width="400">

   + 进程产生实现

     根据题目要求，通过fork的方式产生四个进程。fork函数会从当前位置复制进程，并且在父进程中返回的pid为复制进程的真实pid，在子进程中返回的pid为0。了解这些知识之后可以得到如下的流程图：

     下面的流程图仅表示进程间的关系，前驱关系的实现请看上一小节.

     <img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jsiqayyyj30z60l0acn.jpg" width="600">

   

3. 编译源码

   通过下面的命令编译源码，得到可执行程序

   ```shell
   gcc -g task1.c -o task1 -lpthrea
   ```

### 1.3 实验结果

$\qquad​$通过上面的实验已经得到满足实验要求的可执行程序task1,下面给出运行结果，经过多次测试，四个进程在屏幕上打印的顺序只有两种结果，分别如下：

1. 顺序1：`P1-->P2-->P3-->P4`

<img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jiscj3k3j30oc02jq40.jpg" width="600">

2. 顺序2：`P1-->P3--P2-->P4`

<img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jit8cce5j30lu02imy5.jpg" width="600">

### 1.4 现象解释

$\qquad$测试的实验结果中出现两种执行顺序，通过1.2 节中的分析不难解释这种现象，由于P1是P2和P3的前驱，所以P1一定会在P2和P3之前执行，但是P2和P3是互斥关系，这两个进程谁先获得P1产生的信号量谁就先执行另一个进程等待。最后等P2和P3都执行完了再执行P4，所以会出现上面的两种执行顺序。

## Task 2

### 2.1 实验要求

$\qquad$火车票余票数ticketCount 初始值为1000，有一个售票线程，一个退票线程，各循环执行多次。添加同步机制，使得结果始终正确。要求多次测试添加同步机制前后的实验效果。

### 2.2 实验过程

#### 2.2.1未添加同步机制

1. 实验源码

   `task2_1.c`:

   ```c
   #include<stdio.h>
   #include<stdlib.h>
   #include<pthread.h>
   #include<semaphore.h>
   #include<sys/stat.h>
   #include<fcntl.h>
   #include<string.h>
   int ticketCount=1000;
   void *SaleThread(void *arg)
   {
   	int num,temp;
   	num=atoi(arg);
   	for(int i=0;i<num;i++)
   	{
   		if(i % 10 ==0)
   			printf("卖%d张票,剩余%d张票\n",i,ticketCount);
   		temp=ticketCount;
   		//放弃CPU，强制切换到另外一个进程
   		pthread_yield();
   		temp=temp-1;
   		pthread_yield();
   		ticketCount=temp;
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
   		temp=ticketCount;
   		pthread_yield();
   		temp=temp+1;
   		pthread_yield();
   		ticketCount=temp;
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
   	printf("初始票数为：%d\n",ticketCount);
   	pthread_t p1,p2;
   	/* printf("%s %s",argv[1],argv[2]); */
   	pthread_create(&p1,NULL,SaleThread,argv[1]);
   	pthread_create(&p2,NULL,RefundThread,argv[2]);
   	pthread_join(p1,NULL);
   	pthread_join(p2,NULL);
   	printf("最终票数为：%d\n",ticketCount);
   	return 0;
   }
   ```

2. 程序解释

   + 在main函数中创建两个线程，分别是模拟售票的线程`SaleThread`和模拟退票的线程`RefundThread`，两个进程并发执行，不添加任何的同步机制。
   + 模拟票数的变量ticketCount是全局变量
   + 程序运行需要输入两个参数，第一个是售票数量，第二个数退票数量

3. 程序运行结果

   编译上述程序，得到可执行程序`task2_1`

   ```shell
   gcc -g  task2_1.c -o task2_1 -lpthread
   ```

   多次测试运行，运行结果可以分为两种两类，一种是售票数量比退票数量多，另一种是售票数量比退票数量少。两种情况的结果分别如下：

   + 售票数量比退票数量多：

     初始票数：1000  	售票：100 	退票：40

     <img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jzmct47hj30ze0h4n0i.jpg" width="600">

   + 售票数量比退票数量少：

     初始票数：1000	售票：50	退票：80

     <img src="https://ws2.sinaimg.cn/large/006tKfTcly1g1jznrmxgxj30z60g2whl.jpg" width="600">

     

4. 实验现象归纳

   通过一系列的测试，归纳出的实现现象如下：

   + 当售票数量大于退票数量的时候，最终票数等于总票数减去售票数
   + 当售票数量小于退票数量的时候，最终票数等于总票数加上退票数

#### 2.2.2 添加同步机制

1. 实验源码

   `task2_2.c`

   