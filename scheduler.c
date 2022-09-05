#include "headers.h"
#include "priorityQ2.h"
FILE *file;
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
struct msgbuff message;
int startTime, currentTime;
int rec_val, msgq_id;

void HPF(int n){
    for(int i=0; i<n; i++) PQ[i].priority = INT_MAX;
    int pid, wpid, status;
    char sruntime[20];
    //printf("id \t priority \t runtime \t arrival\n");

    int prevTime = getClk();
    bool isCurrentRunning = false;
    while(true){
        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, IPC_NOWAIT);

        if (rec_val == -1) {
        }
        else{
            printf("\nProcess received : id-> : %d  \n",message.id);
            Process newProcess;
            newProcess.id = message.id;
            newProcess.arrival = message.arrivaltime;
            newProcess.priority = message.priority;
            newProcess.runingTime = message.runningtime;
            newProcess.status = 'N';
            newProcess.memsize = message.memsize;
            PQinsert(newProcess, newProcess.priority);
            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nProcess received : id-> : %d  \n",message.id);
                newProcess.id = message.id;
                newProcess.arrival = message.arrivaltime;
                newProcess.priority = message.priority;
                newProcess.runingTime = message.runningtime;
                newProcess.status = 'N';
                newProcess.memsize = message.memsize;
                PQinsert(newProcess, newProcess.priority);
            }
        }

        if(currentTime == prevTime && isCurrentRunning) continue;

        if(!isCurrentRunning){
            if(PQ[0].status == 'F' && PQ[n-1].status == 'F') break;

            if(PQ[0].status != 'N') continue;
            freeListNode* allocationLimits = allocate(PQ[0].memsize);
            if(allocationLimits == NULL) continue;
            PQ[0].memstart = allocationLimits->start;
            printf("\nProcess %d forked at %d\n", PQ[0].id, currentTime);
            /////////////////////////////////////////////////////////////////////////////////
            PQ[0].waitingTime=currentTime-PQ[0].arrival;
            PQ[0].totalTime=PQ[0].runingTime;
            fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
            /////////////////////////////////////////////////////////////////////////////////
            prevTime = currentTime;
            pid = fork();
            if(pid == -1) perror("error in fork");
            else if(pid == 0){
                sprintf(sruntime, "%d", PQ[0].runingTime);
                execl("./process.out", "./process.out", sruntime, NULL);
            }else{
                PQ[0].pid = pid;
                PQ[0].status = 'R';
                isCurrentRunning = true;
            }
        }else if(currentTime != prevTime){
            PQ[0].runingTime--;
            //printf("\nCurrent Time: %d - Remaining: %d\n", getClk(), PQ[0].runingTime);
            prevTime = currentTime;
            if(!PQ[0].runingTime){
                kill(PQ[0].pid, SIGUSR1);
                wpid = wait(&status);
                status = status >> 8;
                ///////////////////////////////////////////////////////////////////////
                int TA=currentTime-PQ[0].arrival;
                float WTA =(float)TA/PQ[0].totalTime;
                fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime,TA,WTA);
                ///////////////////////////////////////////////////////////////////////
                printf("finished at %d\n\n", getClk());
                deallocate(PQ[0].memstart, PQ[0].memsize);
                isCurrentRunning = false;
                PQ[0].status = 'F';
                PQ[0].priority = INT_MAX;
                insertionSortHPF(PQ, n);
            }
        }
    }
}

void SRTN(int n){
    for(int i=0; i<n; i++) waitingQ[i].runingTime = INT_MAX;
    int pid, wpid, status, currentRunningId = -1, nextIndex = 0, currentRunningIdOriginal;
    char sruntime[20];
    Process  RunProcess;
    RunProcess.id = 0;
    for(int i = 0; i < n; i++){PQ[i].runingTime = INT_MAX;}
    int prevTime = getClk();
    while(true){
        int waitingIndex = 0;
        while(waitingIndex < n){
            if(waitingQ[waitingIndex].status == 'W'){
                freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                if(allocationLimits != NULL){
                    waitingQ[waitingIndex].status = 'N';
                    waitingQ[waitingIndex].memstart = allocationLimits->start;
                    PQinsert2(waitingQ[waitingIndex], waitingQ[waitingIndex].runingTime);
                    waitingQ[waitingIndex].runingTime = INT_MAX;
                }
            }
            waitingIndex++;
        }
        insertionSort(waitingQ, n);

        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 0, IPC_NOWAIT);
        if (rec_val == -1){
        }
        else{
            printf("\nProcess received : id-> : %d  \n",message.id);
            Process newProcess;
            newProcess.id = message.id;
            newProcess.arrival = message.arrivaltime;
            newProcess.priority = message.priority;
            newProcess.runingTime = message.runningtime;
            newProcess.status = 'W';
            newProcess.memsize = message.memsize;
            PQinsertWaiting(newProcess, newProcess.runingTime);

            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nProcess received : id-> : %d  \n",message.id);
                newProcess.id = message.id;
                newProcess.arrival = message.arrivaltime;
                newProcess.priority = message.priority;
                newProcess.runingTime = message.runningtime;
                newProcess.status = 'W';
                newProcess.memsize = message.memsize;

                PQinsertWaiting(newProcess, newProcess.runingTime);

            }

            int waitingIndex = 0;
            while(waitingIndex < n){
                //printf("\nindex: %d\n", waitingIndex);
                if(waitingQ[waitingIndex].status == 'W'){
                    freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                    if(allocationLimits != NULL){
                        printf("\nProcess %d Allocateed, start: %d, end: %d\n", waitingQ[waitingIndex].id, allocationLimits->start, allocationLimits->end);
                        waitingQ[waitingIndex].status = 'N';
                        waitingQ[waitingIndex].memstart = allocationLimits->start;
                        PQinsert2(waitingQ[waitingIndex], waitingQ[waitingIndex].runingTime);
                        waitingQ[waitingIndex].runingTime = INT_MAX;
                    }
                }
                waitingIndex++;
            }
            insertionSort(waitingQ, n);

            // preemption
            if(RunProcess.id != 0 && RunProcess.id != PQ[0].id){
                    printf("\nProcprev %d SWAP PROC id %d    \n",RunProcess.id,PQ[0].id);
                    kill(RunProcess.pid, SIGSTOP);
                    for (int i = 0; i < n; i++){
                        if(RunProcess.id == PQ[i].id){
                            PQ[i].status = 'T';
                            /////////////////////////////////////////////////////////////////////
                            PQ[i].lastStop=currentTime;
                            fprintf(file, "At time  %d  process  %d  stopped  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[i].id,PQ[i].arrival,PQ[i].totalTime,PQ[i].runingTime,PQ[i].waitingTime);
                            /////////////////////////////////////////////////////////////////////
                            break;
                        }
                    }
                    RunProcess.id=0;
                }
        }

        if(currentTime == prevTime && RunProcess.id != 0) continue;
                
        if(RunProcess.id == 0){
            
            if(PQ[0].status == 'F' && PQ[n-1].status == 'F') break;
                        
            if(PQ[0].status == 'N'){
                ///////////////////////////////////////////////////////////////////////
                PQ[0].waitingTime=currentTime-PQ[0].arrival;
                PQ[0].totalTime=PQ[0].runingTime;
                fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
                ///////////////////////////////////////////////////////////////////////
                pid = fork();
                if(pid == -1) perror("error in fork");
                else if(pid == 0){
                    sprintf(sruntime, "%d", PQ[0].runingTime);
                    execl("./process.out", "./process.out", sruntime, NULL);
                }else{
                    printf("\nProcess %d Start at %d\n", pid, getClk());
                    currentRunningId=PQ[0].pid;
                    PQ[0].pid = pid;
                    PQ[0].status = 'R';
                    RunProcess.id = PQ[0].id;
                    RunProcess.pid = pid;
                    prevTime = currentTime;
                }
            }else if(PQ[0].status == 'T'){
                printf("\nProcess %d Continue at %d\n", PQ[0].pid, getClk());
                currentRunningId = PQ[0].pid;
                PQ[0].status = 'R';
                ////////////////////////////////////////////////////////////////////////
                PQ[0].waitingTime=PQ[0].waitingTime+(currentTime-PQ[0].lastStop);
                fprintf(file, "At time  %d  process  %d  resumed  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
                ////////////////////////////////////////////////////////////////////////
                kill(PQ[0].pid,SIGCONT);
                RunProcess.id = PQ[0].id;
                prevTime = currentTime;
            }
        }

        if(prevTime != currentTime){
            if(RunProcess.id != 0){
                PQ[0].runingTime--;
                //printf("\nRT: %d - %d\n", PQ[0].runingTime, getClk());
                prevTime = currentTime;
                if(!PQ[0].runingTime){
                    kill(PQ[0].pid, SIGUSR1);
                    wpid = wait(&status);
                    status = status >> 8;
                    printf("\nFinished at %d\n", getClk());
                    ///////////////////////////////////////////////////////////////////////
                    int TA=currentTime-PQ[0].arrival;
                    float WTA =(float)TA/PQ[0].totalTime;
                    fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime,TA,WTA);
                    ///////////////////////////////////////////////////////////////////////
                    RunProcess.id = 0;
                    PQ[0].status = 'F';
                    PQ[0].runingTime = INT_MAX;
                    deallocate(PQ[0].memstart, PQ[0].memsize);
                    printf("\nDeallocation (%d): start=%d, size=%d\n", PQ[0].id, PQ[0].memstart, PQ[0].memsize);
                    insertionSort(PQ, n);
                }
            }
        }
    }
}

void RR(int n, int Q){
    for(int i=0; i<n; i++) waitingQ[i].runingTime = INT_MAX;
    int  pid, wpid, status, currentRunningId = -1, nextIndex = 0, currentRunningIdOriginal, actualQ, spentTime = 0;
    char sruntime[20];
    struct node* head= NULL ;
    struct node *tail = NULL;
    struct node *currentRunning = NULL;
    struct node *previousProcess = NULL;
    int prevTime = getClk();
    while(true){


        int waitingIndex = 0;
        while(waitingIndex < n){
            if(waitingQ[waitingIndex].status == 'W'){
                freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                if(allocationLimits != NULL){
                    waitingQ[waitingIndex].status = 'N';
                    waitingQ[waitingIndex].memstart = allocationLimits->start;


                    struct node *newProcess = malloc(sizeof(node));
                    Process* newProcess2 = malloc(sizeof(Process));
                    *newProcess2 = waitingQ[waitingIndex];
                    newProcess->data = newProcess2;
                    if (!head) {
                        head = newProcess;
                        tail = newProcess;
                        head->next = tail;
                        tail->next = head;
                        currentRunning=head;
                    }else {
                        tail->next = newProcess;
                        tail = newProcess;
                        tail->next = head;
                    }

                    waitingQ[waitingIndex].runingTime = INT_MAX;

                    struct node *newProcessBB = head;
                    printf("\n");
                    printf("%d    ", newProcessBB->data->id);
                    newProcessBB = newProcessBB->next;
                    while(newProcessBB != head){
                        printf("%d    ", newProcessBB->data->id);
                        newProcessBB = newProcessBB->next;
                    }
                    printf("\n");
                    printf("tail %d\n", tail->data->id);
                }
            }
            waitingIndex++;
        }
        insertionSort(waitingQ, n);

        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext) + sizeof(message.id) * 5, 0, IPC_NOWAIT);
        if (rec_val == -1)
        {
        }
        else
        {
            // printf("\nPROCESS %d CREATED AT %d\n", processes[nextIndex].id, getClk());
            printf("\nMessage received: %s : %d  \n", message.mtext, message.id);
            Process*newProcess2=malloc(sizeof(Process));
            newProcess2->id = message.id;
            newProcess2->arrival = message.arrivaltime;
            newProcess2->priority = message.priority;
            newProcess2->runingTime = message.runningtime;
            newProcess2->status = 'W';
            newProcess2->memsize = message.memsize;

            PQinsertWaiting(*newProcess2, newProcess2->runingTime);

            nextIndex++;


            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nMessage received: %s : %d  \n", message.mtext,message.id);
                Process* newProcess3 = malloc(sizeof(Process));
                newProcess3->id = message.id;
                newProcess3->arrival = message.arrivaltime;
                newProcess3->priority = message.priority;
                newProcess3->runingTime = message.runningtime;
                newProcess3->status = 'W';
                newProcess3->memsize = message.memsize;

                nextIndex++;
                // struct node *newProcess4 = malloc(sizeof(node));
                // newProcess4->data = newProcess3;
                // tail->next = newProcess4;
                // tail = newProcess4;
                // tail->next = head;
                PQinsertWaiting(*newProcess3, newProcess3->runingTime);
            }

            int waitingIndex = 0;
            while(waitingIndex < n){
                if(waitingQ[waitingIndex].status == 'W'){
                    freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                    if(allocationLimits != NULL){
                        waitingQ[waitingIndex].status = 'N';
                        waitingQ[waitingIndex].memstart = allocationLimits->start;


                        struct node *newProcess = malloc(sizeof(node));
                        newProcess2 = malloc(sizeof(Process));
                        *newProcess2 = waitingQ[waitingIndex];
                        newProcess->data = newProcess2;
                        if (!head) {
                            head = newProcess;
                            tail = newProcess;
                            head->next = tail;
                            tail->next = head;
                            currentRunning=head;
                        }else {
                            tail->next = newProcess;
                            tail = newProcess;
                            tail->next = head;
                        }
                        waitingQ[waitingIndex].runingTime = INT_MAX;

                        struct node *newProcessBB = head;
                        printf("\n");
                        printf("%d    ", newProcessBB->data->id);
                        newProcessBB = newProcessBB->next;
                        while(newProcessBB != head){
                            printf("%d    ", newProcessBB->data->id);
                            newProcessBB = newProcessBB->next;
                        }
                        printf("\n");
                        printf("tail %d\n", tail->data->id);
                    }
                }
                waitingIndex++;
            }
            insertionSort(waitingQ, n);
        }

        if(!head){
            if (nextIndex == n) break;
            continue;
        }

        actualQ = Q;
        if(currentRunning->data->runingTime < Q) actualQ = currentRunning->data->runingTime;

        if(currentRunning->data->status == 'N'){
            //////////////////////////////////////////////////////////////////////
            currentRunning->data->waitingTime=currentTime-currentRunning->data->arrival;
            currentRunning->data->totalTime=currentRunning->data->runingTime;
            fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime);
            //////////////////////////////////////////////////////////////////////
            pid = fork();
            if(pid == -1) perror("error in fork");
            else if(pid == 0) {
                sprintf(sruntime, "%d", currentRunning->data->runingTime);
                execl("./process.out", "./process.out", sruntime, NULL);
            }else{
                currentRunning->data->pid = pid;
                printf("\nPROCESS %d START AT %d\n", currentRunning->data->id,getClk());
                currentRunning->data->status = 'R';
                spentTime = 0;
                prevTime=currentTime;
            }
        }else if(currentRunning->data->status == 'T'){
            kill(currentRunning->data->pid, SIGCONT);
            printf("\nPROCESS %d CONTINUE AT %d\n", currentRunning->data->id, getClk());
            ////////////////////////////////////////////////////////////////////////
            currentRunning->data->waitingTime=currentRunning->data->waitingTime+(currentTime-currentRunning->data->lastStop);
            fprintf(file, "At time  %d  process  %d  resumed  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime);
            ////////////////////////////////////////////////////////////////////////
        currentRunning->data->status = 'R';
        spentTime = 0;
        }

        if (prevTime==currentTime)
        {
            continue;
        }
        else{
            spentTime++;
            prevTime=currentTime;
        }

        if(spentTime != actualQ){
            continue;
        }

        currentRunning->data->status = 'T';

        printf("\nPROCESS %d STOP AT %d\n", currentRunning->data->id, getClk());
        /////////////////////////////////////////////////////////////////////
        currentRunning->data->lastStop=currentTime;
        int tempRemain=currentRunning->data->runingTime - actualQ;
        if(tempRemain > 0)
        {
            fprintf(file, "At time  %d  process  %d  stopped  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,tempRemain,currentRunning->data->waitingTime);
        }
        /////////////////////////////////////////////////////////////////////

        kill(currentRunning->data->pid, SIGSTOP);
        currentRunning->data->runingTime -= actualQ;

        if(!currentRunning->data->runingTime){
            printf("\nFINISHED %d AT %d\n", currentRunning->data->pid, getClk());
            ///////////////////////////////////////////////////////////////////////
            int TA=currentTime-currentRunning->data->arrival;
            float WTA =(float)TA/currentRunning->data->totalTime;
            fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime,TA,WTA);
            ///////////////////////////////////////////////////////////////////////
            deallocate(currentRunning->data->memstart, currentRunning->data->memsize);
            printf("\nDeallocation (%d): start=%d, size=%d\n", currentRunning->data->id, currentRunning->data->memstart, currentRunning->data->memsize);
            currentRunning->data->status = 'F';
            kill(currentRunning->data->pid, SIGUSR1);

            struct node *newProcessAA = head;
            printf("\n");
            printf("%d    ", newProcessAA->data->id);
            newProcessAA = newProcessAA->next;
            while(newProcessAA != head){
                printf("%d    ", newProcessAA->data->id);
                newProcessAA = newProcessAA->next;
            }
            printf("\n");
            printf("\n%d\n", currentRunning->next->data->id);
            if(currentRunning == head){
                printf("%d -------------- %d\n", head->data->id, tail->data->id);
                previousProcess = tail;
                head = head->next;
                tail->next = head->next;
            }
            if(currentRunning == tail){
                tail = previousProcess;
                tail->next = head;
            }
            if(previousProcess&&previousProcess!=currentRunning) previousProcess->next = currentRunning->next;
            else{
                head=NULL;
                printf("\n head=null\n");
            }
            if(currentRunning == currentRunning->next){
                head=NULL;
                printf("\n head=null\n");
            }
        }
        if(currentRunning->data->runingTime) previousProcess = currentRunning;
        currentRunning = currentRunning->next;
       // if(currentRunning == currentRunning->next && currentRunning->data->status == 'F') head = NULL;
    }
}

int main(int argc, char * argv[])
{
    initClk();

    Buddy(1024);
    file = fopen("scheduler.log", "w");
    fprintf(file, "#At time  x  process  y  state  arr  w  total  z  remain  y  wait  k \n");
    key_t key_id,key_id2;
    int rec_val2,msgq_id2;
    key_id = ftok("keyfile", 65);              
    key_id2 = ftok("keyfile", 66);             
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id2, 0666 | IPC_CREAT);

    if (msgq_id == -1 || msgq_id2==-1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff1 message1;
    rec_val2 = msgrcv(msgq_id2, &message1, sizeof(message1.mypid)*4, 2, !IPC_NOWAIT);
    if(rec_val2 == -1) perror("Error in receive");
    else{
    }

    printf("Message Queue ID = %d\n", msgq_id);

    int n = message1.num;

    PQ = malloc(sizeof(Process)*n);
    waitingQ = malloc(sizeof(Process)*n);

    startTime = getClk();
    printf("\nSCHEDULER STARTED AT %d\n", startTime);
    if(message1.schNumber == 1)
    {
        HPF(message1.num);
    }
    else if(message1.schNumber == 2)
    {
        SRTN(message1.num);
    }
    else
    {
        RR(message1.num,message1.quantum);
    }
    printf("\nSCHEDULER TERMINATED AT %d\n", getClk());

    kill(message1.mypid,SIGINT);
    return 0;
}
