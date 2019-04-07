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
    char buf[20];
    if(-1 == pipe(fd))
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    write(fd[1], "hello,world", 12);
    memset(buf, '\0', sizeof(buf));

    read(fd[0], buf, 12);
    printf("The message is: %s\n", buf);

    return 0;
}
