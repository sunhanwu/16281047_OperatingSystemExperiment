



# <center>实验三 实验报告</center>

> <center>孙汉武	安全1601	16281047</center>

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

     <div align="center"><img src="https://ws1.sinaimg.cn/large/006tKfTcly1g1jizdjii3j30xm0gcq3w.jpg" width="600" /></div>

     

     

     前驱关系：`P1-->P2`、`P1-->P3`、`P2-->P4`、`P3-->P4`

   + 前驱关系实现

     题目要求产生的四个进程必须是P1最先执行，P2、P3在P1执行完后互斥执行，P4最后执行。于是根据要求有了上面的前驱关系和前驱图。但是如何实现这种进程间的前驱关系呢？比较自然的想到了是用信号量机制。如上面的代码所示，定义了三个信号量，P1_signal、P2_signal和P3_signal，其初值均为0

     ```c
     P1_signal=sem_open("P1_signal",O_CREAT,0666,0);
     P2_signal=sem_open("P2_signal",O_CREAT,0666,0);
     P3_signal=sem_open("P3_signal",O_CREAT,0666,0);
     ```

     P1进程执行完打印任务之后对P1_signal信号量进行V操作，产生一个资源让等待P1_signal的进程P2和P3其中之一可以执行。由于P2和P3都是等待P1_signal信号量，但是P1进程只产生一个单位的信号，所以P2和P3的执行是互斥的，这样就满足了题目要求。最后在P2和P3执行完打印任务后对信号量P2_signal和P3_signal进行V操作从各产生一个单位的信号量，而进程P4会等待P2_signal和P3_signal，所以知道当P2和P3进程都完成才能进行P4进程。通过控制这三个信号量，这四个进程之间的前驱关系就满足了题目要求。

     <div align="center"><img src="https://ws1.sinaimg.cn/large/006tKfTcly1g1juc5uvifj30uk0ss778.jpg" width="600" /></div>

   + 进程产生实现

     根据题目要求，通过fork的方式产生四个进程。fork函数会从当前位置复制进程，并且在父进程中返回的pid为复制进程的真实pid，在子进程中返回的pid为0。了解这些知识之后可以得到如下的流程图：

     下面的流程图仅表示进程间的关系，前驱关系的实现请看上一小节.

     <div align="center"><img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jsiqayyyj30z60l0acn.jpg" width="600" /></div>

   

3. 编译源码

   通过下面的命令编译源码，得到可执行程序

   ```shell
   gcc -g task1.c -o task1 -lpthrea
   ```

### 1.3 实验结果

$\qquad​$通过上面的实验已经得到满足实验要求的可执行程序task1,下面给出运行结果，经过多次测试，四个进程在屏幕上打印的顺序只有两种结果，分别如下：

1. 顺序1：`P1-->P2-->P3-->P4`

   <div align="center"><img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jiscj3k3j30oc02jq40.jpg" width="600" /></div>

2. 顺序2：`P1-->P3--P2-->P4`

<div align="center"><img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jit8cce5j30lu02imy5.jpg" width="600" /></div>

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

     <div align="center"><img src="https://ws3.sinaimg.cn/large/006tKfTcly1g1jzmct47hj30ze0h4n0i.jpg" width="600" /></div>

     

   + 售票数量比退票数量少：

     初始票数：1000	售票：50	退票：80

     <div align="center"><img src="https://ws2.sinaimg.cn/large/006tKfTcly1g1jznrmxgxj30z60g2whl.jpg" width="600" /></div>

     

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

     <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1md41scu2j30qi06z3za.jpg" width="600" /></div>

3. 程序运行结果

   编译上述程序，得到可执行程序task2_2

   ```shell
   gcc task2_2.c -o task2_2 -lpthread
   ```

   多次测试运行，测试主要分为两种类型，一种是售票数量比退票数量多，另一种是售票数量比退票数量少。两种情况的结果分别如下：

   + 售票数量比退票数量多：

     初始票数：1000  	售票：100 	退票：40

     <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1md9o8sw0j30q50dowh5.jpg" width="600" /></div>

   + 退票数量比售票数量多：

     初始票数：1000	售票：50	退票：80

     <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mdaxj1kyj30q40cu40y.jpg" width="600" /></div>

4. 实验现象归纳

   - 在第一个测试样例中，初始票数为1000，售票100并且退票40，最终总票数为940，结果正确；

   - 在第二个测试样例中，初始票数为1000，售票50并且退票80，最终总票数为1030，结果正确。

   + 通过上面的测试结果可以看出，不论是售票数量比退票数量多还是少，都不会发生类似前面2.2.1的问题，最终的票数是期待得到的结果。

   + 上面的实验证实了增加了同步机制之后的多线程并发程序有效的解决了脏数据的读取问题

### 2.3 实验结果

 通过2.2节的对比实验可以看出，在执行多进程并发程序的时候，由于多进程的切换可能发生在某个进程的中间，会导致在一个进程处理的数据未写入ticketCount之前另外一个进程读取该数据，这样就导致了脏数据的读取，导致最终结果的不正确。

在2.2节的后半部分通过怎加同步机制，保证售票进程和退票进程的的原子性，就是指在某个进程操作的时候，在它完成操作之前另外一个进程无法操作共享变量ticketCount,这样就避免了脏数据的发生，得到了预期的正确结果。

### 2.4 现象解释

#### 2.4.1 现象解释1

在2.2.1节的实验中，以售票线程为例（代码如下图所示），没有添加同步机制，并且在进行`temp=temp-1`和`temp=ticketCount`的后面均加上了pthread_yield，这个函数的作用是放弃对CPU的使用权，切换到其他进程中，本实验中就是切换到退票进程中。

这样就能解释为什么2.2.1节中的实验现象，在2.2.1节中，不论售票数多还是退票数多，最终结果都是总票数加上或减去值比较大的那个数。通过分析可以得出解释，售票进程和退票进程同时进行，初始票数均为1000，售票进程完成一次是票数为999，售票进程开始下一次售票，但是在运行`temp=ticketCount`之前，退票进程处理的数据还没有写入到内存中，导致售票进程读取的还是自己之前计算的ticketCount值，而不是全局的值。退票进程也是同理。

但是为什么刚好就是总票数加上或减去值比较大的那个呢？按照道理来说因该售票进程执行temp=ticketCount在退票进程写入ticketCount值之前发生是存在一定概率的，但是在目前为止的所有测试结果全部都是在写入之前读取ticketCount值，对此的解释是由于ticketCount=temp和temp=ticketCount之间没有加pthread_yield操作，而现代的处理器运算速度足够快，在退票进程放弃CPU控制权的那个时间片已经完成了这两步操作，所以相当于售票进程读取的ticketCount一直是自己本身的值，退票进程处理的数据对售票进程并没有影响。

<div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mg7l5m5jj30hu0ast9k.jpg" width="600" /></div>

为了验证上面的猜想，在=如下图所示代码，在ticketCount之后增加一行代码，pthread_yield，放弃当前进程对CPU的控制权，即售票进程放弃对CPU的控制权转而交给退票进程，这个时候退票进程处理的数据就能写入到内存中，而当售票进程再次处理temp=ticketCount的时候，读取的就是退票进程已经写入的数据。如果猜想正确的话，期待的最终票数因该还会发生错误，但并不是像第一中那种恰好等于总票数加减数值大的那个数。

<div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mg5o8rzxj30ht0am0tk.jpg" width="600" /></div>

得到的结果如下，发现最终的票数不在是1000+50=1050，而是分布在1000~1050之间的数值。猜想得到验证。

<div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1mx3kkmlxj30hh05ndg8.jpg" width="600" /></div>

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

   `task3.c`

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

2. 程序解释

   + work1是输入线程调用的函数，worker2是输出线程调用的函数。
   + 设置两个信号量empty和ful来控制程序的执行，其中empty信号量用于保证输入线程在写入数据到缓存的时候缓存中还有空余的位置，保证写入线程后写入的数据不会把前面写入但是为输出的数据给覆盖掉，其初始值为10，表示最开始缓存中有10个空余的位置供给写入线程写入数据；full信号量是用于保证输出线程有数据输出，避免在写入线程还没有写入数据的情况下输出线程输出随机数据，其初始值为0，表示初始状态下缓存中没有数据可以输出
   + 输入线程在写入一个数据前要等待empty信号量，进入后便消耗一个信号量；完成写入数据操作之后post一个full信号量，通知输出线程输出数据。
   + 输出线程在输出一个数据之前哟啊等待full信号量，进出输出操作后便消耗一个full信号量；完成输出操作后post一个empty信号量，通知写入线程缓存又多一个空余位置以供写入数据。
   + 输出线程每输出一个字符等待一秒钟，方便实验结果的查看。

3. 编译源代码

   ```shell
   gcc task3.c -o task3 -lpthread
   ```

### 3.3 实验结果

#### 3.3.1 实验运行现象

1. 随机输入字母和数字（10个以内）：124365abc

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1t6937h4hj31c80d2wg6.jpg" width="600" /></div>

2. 随机输入字母和数字(10个以上)：123456789abcdefg

<div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1t6andb8sj31ay0k60vf.jpg" width="600" /></div>

3. 不间断输入：

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1t6c6h7qjj31au0nujty.jpg" width="600" /></div>

   通过观察上面的实验现象，可以看到已经满足了实验题目的要求。

#### 3.3.2 实验现象解释

+ 在第一种类型的测试中，输入数据不大于10个字符的时候，由于empty的信号量初始值为10，所以输入进程会一直连续不断的向缓存中写入数据，每写入一个数据，便post一个full信号量，输出线程便能按序输出字符。
+ 在第二种类型的测试中，输入数据大于10个字符的时候，由于empty的初始值为10，所以输入的字符中开始的时候只有前10个字符被写入缓存中，其他的在I/O缓冲区等待输入，当输出线程接收到输入线程post的信号量的时候便会开始输出，每输出一个字符便会post一个empty信号量，当输入线程接收到empty信号量的时候有开始从I/O缓冲区读取数据写入到缓存中。

+ 第三种测试和第二种类似，在输出的过程中间输入数据，原理其实是一样的。

## Task 4

### 4.1 实验要求

1. 通过实验测试，验证共享内存的代码中，receiver能否正确读出sender发送的字符串？如果把其中互斥的代码删除，观察实验结果有何不同？如果在发送和接收进程中打印输出共享内存地址，他们是否相同，为什么？

2. 有名管道和无名管道通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？
3. 消息通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？



### 4.2 实验过程

> 实验过程根据实验要求的三个部分，对应的过程也分为三个部分，具体如下所示

#### 4.2.1 内存共享

1. 实验源码

   > 内存内存共享实验的源码分为两个部分，分别是Sender.c和Receive.c,

   `Sender.c`：

   ```c
   /*
    * Filename: Sender.c
    * Description: 
    */
   
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
   #include <sys/sem.h>
   #include <sys/ipc.h>
   #include <sys/shm.h>
   #include <sys/types.h>
   #include <string.h>
   
   int main(int argc, char *argv[])
   {
       key_t  key;
       int shm_id;
       int sem_id;
       int value = 0;
       //1.Product the key
       key = ftok(".", 0xFF);
       //2. Creat semaphore for visit the shared memory
       sem_id = semget(key, 1, IPC_CREAT|0644);
       if(-1 == sem_id)
       {
           perror("semget");
           exit(EXIT_FAILURE);
       }
       //3. init the semaphore, sem=0
       if(-1 == (semctl(sem_id, 0, SETVAL, value)))
       {
           perror("semctl");
           exit(EXIT_FAILURE);
       }
       //4. Creat the shared memory(1K bytes)
       shm_id = shmget(key, 1024, IPC_CREAT|0644);
       if(-1 == shm_id)
       {
           perror("shmget");
           exit(EXIT_FAILURE);
       }
       //5. attach the shm_id to this process
       char *shm_ptr;
       shm_ptr = shmat(shm_id, NULL, 0);
       if(NULL == shm_ptr)
       {
           perror("shmat");
           exit(EXIT_FAILURE);
       }
       //6. Operation procedure
       struct sembuf sem_b;
       sem_b.sem_num = 0;      //first sem(index=0)
       sem_b.sem_flg = SEM_UNDO;
       sem_b.sem_op = 1;           //Increase 1,make sem=1
       
       while(1)
       {
           if(0 == (value = semctl(sem_id, 0, GETVAL)))
           {
               printf("\nNow, snd message process running:\n");
               printf("\tInput the snd message:  ");
               scanf("%s", shm_ptr);
   
               if(-1 == semop(sem_id, &sem_b, 1))
               {
                   perror("semop");
                   exit(EXIT_FAILURE);
               }
           }
           //if enter "end", then end the process
           if(0 == (strcmp(shm_ptr ,"end")))
           {
               printf("\nExit sender process now!\n");
               break;
           }
       }
       shmdt(shm_ptr);
       return 0;
   }
   
   ```

   `Receiver.c`

   ```c
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>
   #include <sys/sem.h>
   #include <sys/ipc.h>
   #include <sys/shm.h>
   #include <sys/types.h>
   #include <string.h>
   
   int main(int argc, char *argv[])
   {
       key_t  key;
       int shm_id;
       int sem_id;
       int value = 0;
       //1.Product the key
       key = ftok(".", 0xFF);
       //2. Creat semaphore for visit the shared memory
       sem_id = semget(key, 1, IPC_CREAT|0644);
       if(-1 == sem_id)
       {
           perror("semget");
           exit(EXIT_FAILURE);
       }
       //3. init the semaphore, sem=0
       if(-1 == (semctl(sem_id, 0, SETVAL, value)))
       {
           perror("semctl");
           exit(EXIT_FAILURE);
       }
       //4. Creat the shared memory(1K bytes)
       shm_id = shmget(key, 1024, IPC_CREAT|0644);
       if(-1 == shm_id)
       {
           perror("shmget");
           exit(EXIT_FAILURE);
       }
       //5. attach the shm_id to this process
       char *shm_ptr;
       shm_ptr = shmat(shm_id, NULL, 0);
       if(NULL == shm_ptr)
       {
           perror("shmat");
           exit(EXIT_FAILURE);
       }
   
       //6. Operation procedure
       struct sembuf sem_b;
       sem_b.sem_num = 0;      //first sem(index=0)
       sem_b.sem_flg = SEM_UNDO;
       sem_b.sem_op = -1;           //Increase 1,make sem=1
       
       while(1)
       {
           if(1 == (value = semctl(sem_id, 0, GETVAL)))
           {
               printf("\nNow, receive message process running:\n");
               printf("\tThe message is : %s\n", shm_ptr);
   
               if(-1 == semop(sem_id, &sem_b, 1))
               {
                   perror("semop");
                   exit(EXIT_FAILURE);
               }
           }
           //if enter "end", then end the process
           if(0 == (strcmp(shm_ptr ,"end")))
           {
               printf("\nExit the receiver process now!\n");
               break;
           }
       }
       shmdt(shm_ptr);
       //7. delete the shared memory
       if(-1 == shmctl(shm_id, IPC_RMID, NULL))
       {
           perror("shmctl");
           exit(EXIT_FAILURE);
       }
       //8. delete the semaphore
       if(-1 == semctl(sem_id, 0, IPC_RMID))
       {
           perror("semctl");
           exit(EXIT_FAILURE);
       }
       return 0;
   }
   ```

2. 程序解释

   > 下面以sender.c为例解释一下如何创建共享内存并通过信号量机制实现互斥访问从而达到进程间通信的目的。

   + 创建一个共享内存的ID,就是代码中的key

     ```c
      key_t  key;
      key = ftok(".", 0xFF);
     ```

     >  通过ftok函数创建一个key_t类型的变量，作为共享内存的key，ftok函数的两个参数分别是文档名(一个存在的路径),上例中的路径是`.`表示当前路径，另一个参数是子序号

   + 创建并初始化信号量

     ```c
     int sem_id;
     sem_id = semget(key, 1, IPC_CREAT|0644);
     if(-1 == sem_id)
     {
         perror("semget");
         exit(EXIT_FAILURE);
     }
     if(-1 == (semctl(sem_id, 0, SETVAL, value)))
     {
         perror("semctl");
         exit(EXIT_FAILURE);
     }
     ```

     > 通过semget()函数创建一个信号量，初始值为1，再通过semctl()函数初始化该信号量

   + 创建共享内存并挂载在进程中

     ```c
     //4. Creat the shared memory(1K bytes)
     shm_id = shmget(key, 1024, IPC_CREAT|0644);
     if(-1 == shm_id)
     {
         perror("shmget");
         exit(EXIT_FAILURE);
     }
     //5. attach the shm_id to this process
     char *shm_ptr;
     shm_ptr = shmat(shm_id, NULL, 0);
     if(NULL == shm_ptr)
     {
         perror("shmat");
         exit(EXIT_FAILURE);
     }
     ```

     > 在这部分代码中，首先通过shmget()函数创建了一个大小为1000B的共享内存，然后通过shmat函数，将刚刚创建的共享内存以可读写的方式挂载在进程上，并且指定系统将自动选择一个合适的地址给共享内存，将挂载的共享内存地址赋值给char型指针shm_ptr

   + Sender主循环

     ```c
     while(1)
     {
         if(0 == (value = semctl(sem_id, 0, GETVAL)))
         {
             printf("\nNow, snd message process running:\n");
             printf("\tInput the snd message:  ");
             scanf("%s", shm_ptr);
     
             if(-1 == semop(sem_id, &sem_b, 1))
             {
                 perror("semop");
                 exit(EXIT_FAILURE);
             }
         }
         //if enter "end", then end the process
         if(0 == (strcmp(shm_ptr ,"end")))
         {
             printf("\nExit sender process now!\n");
             break;
         }
     }
     ```

     > 主循环中首先判断表示共享内存访问情况的信号量是否为0(为0表示共享内存空闲)，如果为0的话提示用户输入想要输入的消息，并将用户输入的消息写入共享内存中，写完后通过semop函数将信号量加一，通知receiver读取消息。并且定义一个`end`命令表示退出当前进程。循环退出的时候取消共享内存的挂载

   + Receiver主循环

     ```c
     while(1)
     {
         if(1 == (value = semctl(sem_id, 0, GETVAL)))
         {
             printf("\nNow, receive message process running:\n");
             printf("\tThe message is : %s\n", shm_ptr);
     
             if(-1 == semop(sem_id, &sem_b, 1))
             {
                 perror("semop");
                 exit(EXIT_FAILURE);
             }
         }
         //if enter "end", then end the process
         if(0 == (strcmp(shm_ptr ,"end")))
         {
             printf("\nExit the receiver process now!\n");
             break;
         }
     }
     ```

     > 主循环中首先判断表示共享内存访问情况的信号量是否为1(为1表示共享内存已经写入消息，可以读取)，如果为1的话输出该消息，输出后通过semop函数将信号量减1，通知Sender可以再次写入消息。并且定义一个`end`命令表示退出当前进程。循环退出的时候取消共享内存的挂载

3. 实验现象

   将上述源码编译后进行测试，得到下面的结果。

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1t9bsd3s5j31uq0fa0xm.jpg" width="800" /></div>

   > 可以看到sender进程发出的消息receiver进程均准确无误的收到

4. 删除互斥访问相关的代码

   程序主要的代码没有变化，只是在Sender和Receiver进程的主循环中将用于控制互斥访问共享内存的相关代码删除，注释后的结果如下：

   `Sender_2.c:`

   ```c
   while(1)
   {
       printf("\nNow, snd message process running:\n");
       printf("\tInput the snd message:  ");
       scanf("%s", shm_ptr);
       //if enter "end", then end the process
       if(0 == (strcmp(shm_ptr ,"end")))
       {
           printf("\nExit sender process now!\n");
           break;
       }
   }
   ```

   `Receiver_2.c:`

   ```CQL
   while(1)
   {
       printf("\nNow, receive message process running:\n");
       printf("\tThe message is : %s\n", shm_ptr);
   
       //if enter "end", then end the process
       if(0 == (strcmp(shm_ptr ,"end")))
       {
           printf("\nExit the receiver process now!\n");
           break;
       }
       sleep(3);
   }
   ```

   > 最后加一个sleep(1)用于控制打印的速度，便于观察现象

5. 删除互斥访问后的实验现象

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1t9sdljhhj31uq0n8799.jpg" width="800" /></div>

   > 实验现象解释：当删除互斥访问之后，两个进程便没有限制的访问共享内存，Sender进程由于受限于用户输入的速度，会停留一直等待用户输入数据，但是Receiver进程会一直输出共享内存中的消息。

6. 打印Sender和Receiver进程中共享内存的地址

   在原始代码的基础上修改，具体代码文件分别是`Sender_3.c`和`Receiver_3.c`，具体修改就是如下：

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1ta8ocwdvj30wy044t9i.jpg" width="600" /></div>

   在挂载共享内存后打印挂载后的地址

7. 打印共享内存地址实验现象

   <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1ta7ecrb4j31uq0fe7b0.jpg" width="800" /></div>

   > 可以看到实验现象，在两个进程中共享内存的地址不一样

   ==**现象解释：**==

   通过上面的现象可以看到共享内存在不同进程中是不相同的，总结有以下的原因导致共享内存在不同进程中的地址不一样：

   + 进程在挂载内存的时候使用的`shmat()`函数中的第二个参数使用的是NULL，NULL参数的含义是进程让系统分配给共享内存合适的地址。在`shmat()`函数中，第二个参数有三种选择，分别是：

   | 参数值 |             NULL             |                             addr                             |                             addr                             |
   | :----: | :--------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
   |  含义  | 系统将自动选择一个合适的地址 | 如果shmaddr非0 并且指定了SHM_RND 则此段连接到shmaddr -（shmaddr mod SHMLAB)所表示的地址上。 | 第三个参数如果在flag中指定了SHM_RDONLY位，则以只读方式连接此段，否则以读写的方式连接此 段。 |

   ​	可以看到，当addr有具体的值的时候，便将共享内存挂载到指定的地址上

   + 现代操作系统中都存在ASLR(地址空间随机化)，ASLR是⼀种针对缓冲区溢出的安全保护机制，具有ASLR机制的操作系统每次加载到内存的程序起始地址会随机变化。系统的这个随机化操作可能导致共享内存的地址不一致。

   ==验证：==

   1. 指定Sender_4.c和Receiver_4.c中共享内存的挂载地址为`0x7fcc2c0bb000`

      + 修改具体的代码如下：

        <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1taz7pfxdj30x408w404.jpg" width="600" /></div>

      + 运行结果：

        <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1tb56qlhmj31us0d0jx1.jpg" width="800" /></div>

        > 实验结果结果佐证了上面的现象解释，通过指定挂载共享内存的地址，可以使共享内存的地址一致，可以随意指定改地址

   2. 关闭系统的ASLR操作

      + 具体的关闭命令如下：

      ```
      sudo su
      sysctl -w kernel.randomize_va_space=0
      ```

      + 运行结果：

        <div align="center"><img src="https://ws1.sinaimg.cn/large/006CotQ3ly1g1tb3biwvpj31uw0cc794.jpg" width="800" /></div>

        > 这个实验现象也佐证了系统的ASLR也对导致挂载的共享内存地址不一样

#### 4.2.2 管道通信

#### 4.2.3 消息队列

