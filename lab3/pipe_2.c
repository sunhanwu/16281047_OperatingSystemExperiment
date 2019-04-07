/*
 * Filename: pipe.c
 */
 
#include <stdio.h>
#include <unistd.h>     //for pipe()
#include <string.h>     //for memset()
#include <stdlib.h>     //for exit()

int main()
{
    int fd[2];
    char buf[200]={0};
	pid_t child;
    //创建管道
	if(-1 == pipe(fd))
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
	//创建子进程
	child=fork();
	if(child==-1)
	{
		perror("fork");
		exit("EXIT_FAILURE");
	}
	if(child==0)
	{
		//关闭子进程中不需要的写描述符
		close(fd[1]);
			
		while(1)
		{

			if(read(fd[0],buf,sizeof(buf))>0)
			{	
				printf("子进程接收的消息是:%s\n",buf);
			}
			else
			{
				printf("子进程:管道中没有数据\n");
			}
			sleep(2);
				
			if(strcmp(buf,"end")==0)
				break;
			memset(buf,0,sizeof(buf));
		}
	}
	if(child>0)
	{
		close(fd[0]);
		while(1)
		{
			printf("父进程中-请输入消息:");
			scanf("%s",buf);
			write(fd[1],buf,strlen(buf));
			if(strcmp(buf,"end")==0)
				break;
		}	
	}
    return 0;
}
