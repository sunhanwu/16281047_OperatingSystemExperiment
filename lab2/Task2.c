#include<stdio.h>
#include<unistd.h>
int main()
{
	pid_t pid,fpid;//进程id号
	fpid=fork();//创建子进程
	if(fpid<0)
	{
		printf("创建子进程出错！");		
	}
	if(fpid==0)
	{
		int ret;//接收函数返回值
		ret = execl("/usr/bin/vi","vi",NULL);
		if(ret==-1)
			perror("excel l error");
	} 
	if(fpid>0)
	{
		printf("子进程的pid为%d\n",fpid);
		printf("父进程的pid为%d\n",getpid());
		/* 父进程执行空循环 */
		while(1)
		{
			sleep(3);
		}

	}

	return 0;
	
}
