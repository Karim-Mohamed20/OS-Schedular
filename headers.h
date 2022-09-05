#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <math.h>
#include<string.h>


typedef short bool;
#define true 1
#define false 0

#define SHKEY 300


///==============================
//don't mess with this variable//
int * shmaddr;  
int shmid;               //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
     shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
        killpg(getpgrp(), SIGINT);

         printf("clock destroyed\n");
    }
   
}

typedef struct Process {
    int id;
    int priority;
    int runingTime;
    int arrival;
    char status;
    int pid;
    int memsize;
    int memstart;
    int waitingTime;
    int totalTime;
    int lastStop;
}Process;

typedef struct node
{
  Process *data;

  struct node *next;
} node;

void insertionSort(Process array[], int size) {
  for (int step = 1; step < size; step++) {
    Process key = array[step];
    int j = step - 1;

    while (key.runingTime < array[j].runingTime && j >= 0) {
      array[j + 1] = array[j];
      --j;
    }
    array[j + 1] = key;
  }
}

void insertionSortHPF(Process array[], int size) {
  for (int step = 1; step < size; step++) {
      printf("%d\n", step);
    Process key = array[step];
    int j = step - 1;

    while (key.priority < array[j].priority && j >= 0) {
      array[j + 1] = array[j];
      --j;
    }
    array[j + 1] = key;
  }
}

typedef struct freeListNode {
  int start;
  int end;
  struct freeListNode *next;
} freeListNode;

// Size of vector of pairs
int freeListSize;
 
// Global vector of pairs to track all
// the free nodes of various sizes
freeListNode* arr[100];
 
void Buddy(int s) {
    // Maximum number of powers of 2 possible
    int n = ceil(log(s) / log(2));
    freeListSize = n + 1;
 
    // Initially whole block of specified
    // size is available
    arr[n] = malloc(sizeof(freeListNode));
    arr[n]->start = 0;
    arr[n]->end = s - 1;
}

freeListNode* allocate(int s) {
     
    // Calculate index in free list
    // to search for block if available
    int x = ceil(log(s) / log(2));
     
    // Block available
    if (arr[x] != NULL)
    {
        freeListNode* temp = malloc(sizeof(freeListNode));
        temp = arr[x];
        
        // Remove block from free list
        arr[x] = arr[x]->next;
        
        printf("Memory from %d to %d allocated\n", temp->start, temp->end);
 
        // Map starting address with
        // size to make deallocating easy
        return temp;
    }
    else
    {
        int i;
         
        // If not, search for a larger block
        for(i = x + 1; i < freeListSize; i++)
        {
             
            // Find block size greater
            // than request
            if (arr[i] != NULL)
                break;
        }
        // printf("\nFound Memory At %d\n", i);
        // If no such block is found
        // i.e., no memory block available
        if (i == freeListSize)
        {
            // printf("Sorry, failed to allocate memory\n");
            return NULL;
        }
         
        // If found
        else
        {
            // pair<int, int> temp;
            freeListNode* temp = malloc(sizeof(freeListNode));
            temp = arr[i];
            // temp = arr[i][0];
 
            // Remove first block to split
            // it into halves
            arr[i] = arr[i]->next;
            i--;
             
            for(;i >= x; i--)
            {
                 
                // Divide block into two halves
                freeListNode* pair1 = malloc(sizeof(freeListNode));
                freeListNode* pair2 = malloc(sizeof(freeListNode));
                pair1->start = temp->start;
                pair1->end = temp->start + (temp->end - temp->start) / 2;

                pair2->start = temp->start + (temp->end - temp->start + 1) / 2;
                pair2->end = temp->end;
                
                if(arr[i] == NULL){
                    arr[i] = pair1;
                }else{
                    freeListNode* temp2 = arr[i];
                    while(temp2->next != NULL){
                        temp2 = temp2->next;
                    }
                    temp2->next = pair1;
                }
                // arr[i].push_back(pair1);
 
                // Push them in free list
                // arr[i].push_back(pair2);
                pair1->next = pair2;
                // temp = arr[i][0];
                temp = arr[i];
 
                // Remove first free block to
                // further split
                arr[i] = arr[i]->next;
            }
            
            printf("Memory from %d to %d allocated\n", temp->start, temp->end);
                  
            return temp;
        }
    }
}
 
void deallocate(int start, int size) {
     
    // Size of block to be searched
    int n = ceil(log(size) / log(2));
     
    int i, buddyNumber, buddyAddress;
 
    freeListNode* temp = malloc(sizeof(freeListNode));
    temp->start = start;
    temp->end = start + pow(2, n) - 1;
    freeListNode* temp2 = arr[n];
    if(arr[n] == NULL) {
        arr[n] = temp;
    }
    else {
        while(temp2->next != NULL){
            temp2 = temp2->next;
        }
        temp2->next = temp;
    }

    printf("Memory block from %d to %d freed\n", temp->start, temp->end);
         
    // Search in free list to find it's buddy
    bool isMerged = true;
    while(isMerged == true){
        /*for(int i = 0; i < freeListSize; i++){
            printf("\n");
            freeListNode* tempAA = arr[i];
            printf("\nFree Of Size %d\n", i);
            while(tempAA){
                printf("%d    ", tempAA->start);
                tempAA = tempAA->next;
            }
            printf("\n");
        }*/
        buddyNumber = start / size;
    
        if (buddyNumber % 2 != 0)
            buddyAddress = start - pow(2, n);
        else
            buddyAddress = start + pow(2, n);
        temp2 = arr[n];
        while(temp2 != NULL){
            // printf("\nNo: %d - Add: %d\n", buddyNumber, buddyAddress);
            if (temp2->start == buddyAddress){
                if (buddyNumber % 2 == 0){
                    // printf("\nWill Merge 1\n");
                    temp = malloc(sizeof(freeListNode));
                    temp->start = start;
                    temp->end = start + 2 * (pow(2, n)) - 1;

                    freeListNode* temp3 = arr[n+1];
                    if(arr[n+1] == NULL){
                        arr[n+1] = temp;
                    }else{
                        while(temp3->next != NULL){
                            temp3 = temp3->next;
                        }
                        temp3->next = temp;
                    }
                    printf("Coalescing of blocks starting at %d and %d was done\n", temp->start, temp->end);
                }else{
                    // printf("\nWill Merge 2\n");
                    temp = malloc(sizeof(freeListNode));
                    temp->start = buddyAddress;
                    temp->end = buddyAddress + 2 * pow(2, n) - 1;

                    freeListNode* temp3 = arr[n+1];
                    if(arr[n+1] == NULL){
                        arr[n+1] = temp;
                    }else{
                        while(temp3->next != NULL){
                            temp3 = temp3->next;
                        }
                        temp3->next = temp;
                    }
                    printf("Coalescing of blocks starting at %d and %d was done\n", temp->start, temp->end);
                }
                start = temp->start;
                temp = arr[n];
                if(temp == temp2){
                    arr[n] = temp->next;
                }
                while(temp && temp->next != NULL){
                    if(temp->next == temp2){
                        temp->next = temp->next->next;
                    }
                    temp = temp->next;
                }

                temp = arr[n];
                if(temp->next == NULL){
                    arr[n] = NULL;
                }
                while(temp && temp->next && temp->next->next != NULL){
                    temp = temp->next;
                }
                temp->next = NULL;

                n = n + 1;
                size = size * 2;
                // start = temp->start;

                isMerged = true;
                break;
            }else{
                isMerged = false;
            }
            
            temp2 = temp2->next;
        }
    }
}