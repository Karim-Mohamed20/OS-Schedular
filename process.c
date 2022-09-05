#include "headers.h"

int remainingtime;

void handleRemainingTime(){
    remainingtime = 0;
}

int main(int agrc, char * argv[])
{
    initClk();
    int x=getClk();
    printf("Process started : id-> %d ",getpid());
    signal(SIGUSR1, handleRemainingTime);
    remainingtime = atoi(argv[1]);
    while (remainingtime > 0)
    {
        sleep(1);
    }
    printf("Process terminated : id-> %d ",getpid());    
    destroyClk(false);
    return 0;
}
