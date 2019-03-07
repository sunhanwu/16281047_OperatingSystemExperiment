#include<stdio.h>
#include<unistd.h>
int main()
{
	pid_t pid;
	pid = getpid();
	printf("%d\n",pid);
	return 0;
}
