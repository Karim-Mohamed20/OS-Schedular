#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>
typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
}processData;

void GetFileContents(char *file,processData** PD2,int *c)
{
    char line[260];
    FILE *fp;
    char *buf=0;
    char temp[80];
    int i = -1;

    fp = fopen("processes.txt", "r");
     *c=0;
     int h=0;
    while (fgets(line, 260, fp))
    {
        h++;
    }
   fclose(fp); 
   fp = fopen("processes.txt", "r");
    *c=h;
   *PD2=malloc(sizeof(processData)*(h));
    while(fgets(line, 260, fp))
    {
        char s [30];
        buf = strtok(line, "\t\n");
        if(buf) strcpy(s, buf);
        if(s[0]=='#')
        {
            continue;
        }
        else{
            i++;
        }
        (*((*PD2)+i)).id=atoi(s);
        buf = strtok(NULL, "\t\n");
        if(buf) strcpy(s, buf);
        (*((*PD2)+i)).arrivaltime = atoi(s);
        buf = strtok(NULL, "\t\n");
        if(buf) strcpy(s, buf);
        (*((*PD2)+i)).runningtime=atoi(s);
        buf = strtok(NULL, "\t\n");
        if(buf) strcpy(s, buf);
        (*((*PD2)+i)).priority=atoi(s);
        buf = strtok(NULL, "\t\n");
        if(buf) strcpy(s, buf);
        (*((*PD2)+i)).memsize=atoi(s);
    }
    *c=i+1;
    fclose(fp);
}
