#include "headers.h"
#include"read.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

int msgq_id;
void clearResources(int);

struct msgbuff
{
    long mtype;
    char mtext[70];
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};

struct msgbuff1
{
    long mtype;
    int schNumber;
    int num;
    int quantum;
    int mypid;
};

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    key_t key_id,key_id2;
    int send_val,msgq_id2, send_val2;
    key_id = ftok("keyfile", 65);
    key_id2 = ftok("keyfile", 66);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id2, 0666 | IPC_CREAT);
    struct processData *ProcessData; 
    int processNum;
    GetFileContents("processes.txt",&ProcessData,&processNum);
    int schedularNum;
    printf("please choose your scheduler : \nHPF->1 \nSRTN->2 \nRR->3 \n");
    scanf("%d",&schedularNum);
    struct msgbuff1 message1;
    message1.schNumber=schedularNum;
    int RR_quantum=-1;
    message1.quantum=RR_quantum;

    if(schedularNum==3)
    {
        printf("Enter the quantum: \n");
        scanf("%d",&RR_quantum);
        message1.quantum=RR_quantum;
    }

    message1.mtype=2;
    message1.num=processNum;
    message1.mypid=getpid();
    send_val2 = msgsnd(msgq_id2, &message1, sizeof(message1.mypid)*4, !IPC_NOWAIT);
    system("./clk.out &");
    initClk();
    int currentTime;
    struct msgbuff message;
    int count=0;
    printf("id\tarrivaltime\trunningtime\tpriority\tmemsize\t\n");
    for (int  i = 0; i < processNum; i++)
    {
        printf("%d\t",ProcessData[i].id);
        printf("%d\t",ProcessData[i].arrivaltime);
        printf("%d\t",ProcessData[i].runningtime);
        printf("%d\t",ProcessData[i].priority);
        printf("%d\t\n",ProcessData[i].memsize);
    }
    while (count<processNum)
    {
        currentTime = getClk();
        for(int i=count;i<processNum;i++)
        {
            if(currentTime >= ProcessData[i].arrivaltime)
                {
                    message.mtype=1;
                    message.id=ProcessData[i].id;
                    message.priority=ProcessData[i].priority;
                    message.runningtime=ProcessData[i].runningtime;
                    message.arrivaltime=ProcessData[i].arrivaltime;
                    message.memsize=ProcessData[i].memsize;
                    printf("Process send arrivaltime -> %d \n",message.arrivaltime);
                    send_val = msgsnd(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, !IPC_NOWAIT);
                    count++;
                    if (send_val == -1)
                    {perror("Errror in send");}
                    else{
                    }                    
                    if(ProcessData[i].arrivaltime != ProcessData[i+1].arrivaltime)
                    {
                        message.mtype=1;
                        message.id=-1;
                        message.priority=-1;
                        message.runningtime=-1;
                        message.arrivaltime=-1;
                        message.memsize=-1;
                        printf("Process send arrivaltime -> %d \n",message.arrivaltime);
                        send_val = msgsnd(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, !IPC_NOWAIT);
                    
                        if (send_val == -1)
                        perror("Errror in send");
                        else{
                        }      
                        break;
                    }
                }
        }
    }    
    while(1){}
}

void clearResources(int signum)
{
    destroyClk(true);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    printf("message queue is free now \n \n");
    kill(getpid(),SIGKILL);
}