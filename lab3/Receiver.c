/*
 * Filename: Receiver.c
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
