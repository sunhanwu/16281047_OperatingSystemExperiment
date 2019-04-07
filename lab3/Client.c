#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>

#define BUF_SIZE 128

//Rebuild the strcut (must be)
struct msgbuf
{
    long mtype;
    char mtext[BUF_SIZE];
};


int main(int argc, char *argv[])
{
    //1. creat a mseg queue
    key_t key;
    int msgId;
    
    printf("THe process(%s),pid=%d started~\n", argv[0], getpid());

    key = ftok(".", 0xFF);
    msgId = msgget(key, IPC_CREAT|0644);
    if(-1 == msgId)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    //2. creat a sub process, wait the server message
    pid_t pid;
    if(-1 == (pid = fork()))
    {
        perror("vfork");
        exit(EXIT_FAILURE);
    }

    //In child process
    if(0 == pid)
    {
        while(1)
        {
            alarm(0);
            alarm(100);     //if doesn't receive messge in 100s, timeout & exit
            struct msgbuf rcvBuf;
            memset(&rcvBuf, '\0', sizeof(struct msgbuf));
            msgrcv(msgId, &rcvBuf, BUF_SIZE, 2, 0);                
            printf("Server said: %s\n", rcvBuf.mtext);
        }
        
        exit(EXIT_SUCCESS);
    }

    else    //parent process
    {
        while(1)
        {
            usleep(100);
            struct msgbuf sndBuf;
            memset(&sndBuf, '\0', sizeof(sndBuf));
            char buf[BUF_SIZE] ;
            memset(buf, '\0', sizeof(buf));
            
            printf("\nInput snd mesg: ");
            scanf("%s", buf);
            
            strncpy(sndBuf.mtext, buf, strlen(buf)+1);
            sndBuf.mtype = 1;

            if(-1 == msgsnd(msgId, &sndBuf, strlen(buf)+1, 0))
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            
            //if scanf "end~", exit
            if(!strcmp("end~", buf))
                break;
        }
        
        printf("THe process(%s),pid=%d exit~\n", argv[0], getpid());
    }

    return 0;
}
