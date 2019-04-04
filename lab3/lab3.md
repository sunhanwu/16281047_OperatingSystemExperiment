



实验三 实验报告

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

   多次测试运行，运行结果可以分为两种类型，一种是售票数量比退票数量多，另一种是售票数量比退票数量少。两种情况的结果分别如下：

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

   ```c
   #include<stdio.h>
   #include<stdlib.h>
   #include<pthread.h>
   #include<semaphore.h>
   #include<sys/stat.h>
   #include<fcntl.h>
   #include<string.h>
   volatile int ticketCount=1000;
   sem_t *flag=NULL;
   void *SaleThread(void *arg)
   {
   	int num,temp;
   	num=atoi(arg);
   	for(int i=0;i<num;i++)
   	{
   		if(i % 10 ==0)
   			printf("卖%d张票,剩余%d张票\n",i,ticketCount);
   		sem_wait(flag);
   		temp=ticketCount;
   		//放弃CPU，强制切换到另外一个进程
   		pthread_yield();
   		temp=temp-1;
   		ticketCount-=1;
   		pthread_yield();
   		ticketCount=temp;
   		sem_post(flag);
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
   		sem_wait(flag);
   		temp=ticketCount;
   		pthread_yield();
   		temp=temp+1;
   		ticketCount+=1;
   		pthread_yield();
   		ticketCount=temp;
   		sem_post(flag);
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
   	flag=sem_open("flag",O_CREAT,0666,1);
   	printf("初始票数为：%d\n",ticketCount);
   	pthread_t p1,p2;
   	printf("%s %s",argv[1],argv[2]);
   	pthread_create(&p1,NULL,SaleThread,argv[1]);
   	pthread_create(&p2,NULL,RefundThread,argv[2]);
   	pthread_join(p1,NULL);
   	pthread_join(p2,NULL);
   	printf("最终票数为：%d\n",ticketCount);
   	sem_close(flag);
   	sem_unlink("flag");
   	return 0;
   
   }
   ```

2. 程序解释

   + task2_2.c在task2_1.c的基础上增加了同步机制，其他部分完全一致，通过信号量flag的控制，让售票线程和退票线程一次只能执行一个，在一个没有执行完成之前另一个不能进入执行，这样就保证了售票操作和退票操作的原子性，避免了脏数据的读取。

   + flag初始值为设置为1，表示每次只允许一个线程操作ticketCount这个数据

   + 售票线程和退票线程在进入操作之前都要sem_wait(flag)，等待信号量，在完成操作之后要sem_post(flag)，下图是售票线程中增加了信号量的操作：

     <img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1md41scu2j30qi06z3za.jpg" width="600">

3. 程序运行结果

   编译上述程序，得到可执行程序task2_2

   ```shell
   gcc task2_2.c -o task2_2 -lpthread
   ```

   多次测试运行，测试主要分为两种类型，一种是售票数量比退票数量多，另一种是售票数量比退票数量少。两种情况的结果分别如下：

   + 售票数量比退票数量多：

     初始票数：1000  	售票：100 	退票：40

     <img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1md9o8sw0j30q50dowh5.jpg" width="600">

   + 退票数量比售票数量多：

     初始票数：1000	售票：50	退票：80

     <img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mdaxj1kyj30q40cu40y.jpg" width="600">

4. 实验现象归纳

   - 在第一个测试样例中，初始票数为1000，售票100并且退票40，最终总票数为940，结果正确；

   - 在第二个测试样例中，初始票数为1000，售票50并且退票80，最终总票数为1030，结果正确。

   + 通过上面的测试结果可以看出，不论是售票数量比退票数量多还是少，都不会发生类似前面2.2.1的问题，最终的票数是期待得到的结果。

   + 上面的实验证实了增加了同步机制之后的多线程并发程序有效的解决了脏数据的读取问题

### 2.3 实验结果

 通过2.2节的对比实验可以看出，在执行多进程并发程序的时候，由于多进程的切换可能发生在某个进程的中间，会导致在一个进程处理的数据未写入ticketCount之前另外一个进程读取该数据，这样就导致了脏数据的读取，导致最终结果的不正确。

在2.2节的后半部分通过怎加同步机制，保证售票进程和退票进程的的原子性，就是指在某个进程操作的时候，在它完成操作之前另外一个进程无法操作共享变量ticketCount,这样就避免了脏数据的发生。

### 2.4 现象解释

#### 2.4.1 现象解释1

在2.2.1节的实验中，以售票线程为例（代码如下图所示），没有添加同步机制，并且在进行`temp=temp-1`和`temp=ticketCount`的后面均加上了pthread_yield，这个函数的作用是放弃对CPU的使用权，切换到其他进程中，本实验中就是切换到退票进程中。

这样就能解释为什么2.2.1节中的实验现象，在2.2.1节中，不论售票数多还是退票数多，最终结果都是总票数加上或减去值比较大的那个数。通过分析可以得出解释，售票进程和退票进程同时进行，初始票数均为1000，售票进程完成一次是票数为999，售票进程开始下一次售票，但是在运行`temp=ticketCount`之前，退票进程处理的数据还没有写入到内存中，导致售票进程读取的还是自己之前计算的ticketCount值，而不是全局的值。退票进程也是同理。

但是为什么刚好就是总票数加上或减去值比较大的那个呢？按照道理来说因该售票进程执行temp=ticketCount在退票进程写入ticketCount值之前发生是存在一定概率的，但是在目前为止的所有测试结果全部都是在写入之前读取ticketCount值，对此的解释是由于ticketCount=temp和temp=ticketCount之间没有加pthread_yield操作，而现代的处理器运算速度足够快，在退票进程放弃CPU控制权的那个时间片已经完成了这两步操作，所以相当于售票进程读取的ticketCount一直是自己本身的值，退票进程处理的数据对售票进程并没有影响。

<img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mg7l5m5jj30hu0ast9k.jpg" width="600">

为了验证上面的猜想，在=如下图所示代码，在ticketCount之后增加一行代码，pthread_yield，放弃当前进程对CPU的控制权，即售票进程放弃对CPU的控制权转而交给退票进程，这个时候退票进程处理的数据就能写入到内存中，而当售票进程再次处理temp=ticketCount的时候，读取的就是退票进程已经写入的数据。如果猜想正确的话，期待的最终票数因该还会发生错误，但并不是像第一中那种恰好等于总票数加减数值大的那个数。

<img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mg5o8rzxj30ht0am0tk.jpg" width="600">

得到的结果如下，发现最终的票数不在是1000+50=1050，而是分布在1000~1050之间的数值。猜想得到验证。

<img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mx3kkmlxj30hh05ndg8.jpg" width="600">

针对上面的猜想（CPU运算速度过快，导致ticketCount=temp和temp=ticketCount两步操作在一个进程的时间片内完成导致的数据错误），另外的一种验证方式是将初始票数和售票退票数设置的足够大，当数据足够大的时候，就会存在一定概率出现在一个进程的ticketCount=temp和temp=ticketCount两步操作之间切换进程的问题，得到的结果就不会类似2.2.1中的那样，而是类似在ticketCount=temp下面加了pthread_yield那样。

 <div align="center">
     <img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1pa2byvv1j30mp0hbtb2.jpg" width="600" div align="center"/></div>

再次运行，可以看到如下的实验结果：

<div align="center">      <img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1pagap9rwj30qm0do77w.jpg" width="600" div align="center"/></div>

#### 2.4.2 现象解释2

在2.2.2节中，当给售票进程和退票进程都加上同步机制后，保证了每个线程操作的原子性，每个线程操作的过程中其他的线程不能对共享的ticketCount变量进程修改，这样的话最终的结果就是正确的结果。

## Task 3

### 3.1 实验要求

$\qquad​$一个生产者一个消费者线程同步。设置一个线程共享的缓冲区， char buf[10]。一个线程不断从键盘输入字符到buf,一个线程不断的把buf的内容输出到显示器。要求输出的和输入的字符和顺序完全一致。（在输出线程中，每次输出睡眠一秒钟，然后以不同的速度输入测试输出是否正确）。要求多次测试添加同步机制前后的实验效果。

### 3.2 实验过程

1. 实验源码

   task3.c

   ```c
   #include<stdio.h>
   #include<stdlib.h>
   #include<pthread.h>
   #include<semaphore.h>
   #include<sys/stat.h>
   #include<fcntl.h>
   char buf[10];
   sem_t *empty=NULL;
   sem_t *full=NULL;
   void *worker1(void *arg)
   {
   	
   	for(int i=0;i<10;i++)
   	{
   		sem_wait(empty);
   		/* fflush(stdin); */
   		/* printf("输入："); */
   		scanf("%c",&buf[i]);
   		sem_post(full);
   		if(i==9)
   		{
   			i=-1;
   		}
   	}	
   	return NULL;
   }
   void *worker2(void *arg)
   {
   	for(int i=0;i<10;i++)
   	{
   		sem_wait(full);
   		printf("输出：%c\n",buf[i]);
   		sem_post(empty);
   		sleep(1);
   		if(i==9)
   		{
   			i=-1;		
   		}
   	}	
   	return NULL;
   }
   
   int main(int argc,char *argv[])
   {
   	empty=sem_open("empty_",O_CREAT,0666,10);
   	full=sem_open("full_",O_CREAT,0666,0);
   	pthread_t p1,p2;
   	pthread_create(&p1,NULL,worker1,NULL);
   	pthread_create(&p2,NULL,worker2,NULL);
   	pthread_join(p1,NULL);
   	pthread_join(p2,NULL);
   	sem_close(empty);
   	sem_close(full);
   	sem_unlink("empty_");
   	sem_unlink("full_");
   	return 0;
   }
   ```

2. 原理解释

   work1是输入线程调用的函数，worker2是输出线程调用的函数。如果没有使用信号量实现共享缓冲区同步的话