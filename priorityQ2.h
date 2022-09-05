
#define SIZE 5            /* Size of Queue */
int f=0,r=-1;       /* Global declarations */

// Process PQ[SIZE];
Process* PQ;

int Qfull()
{                     /* Function to Check Queue Full */
 if(r==SIZE-1) return 1;
 return 0;
}

int Qempty()
{                    /* Function to Check Queue Empty */
 if(f > r) return 1;
 return 0;
}

void PQinsert(Process elem, int pre)
{
 int i;       /* Function for Insert operation */
 if( Qfull()) printf("nn Overflow!!!!nn");
 else
 {
  i=r;
  ++r;
//   while(PQ[i].runingTime >= pre && i >= 0)
  while(PQ[i].priority >= pre && i >= 1) /* Find location for new elem */
  {
   PQ[i+1]=PQ[i];
   i--;
  }
  PQ[i+1] = elem;
 }
}
void PQinsert2(Process elem, int pre)
{
 int i;       /* Function for Insert operation */
 if( Qfull()) printf("nn Overflow!!!!nn");
 else
 {
  i=r;
  ++r;
   while(PQ[i].runingTime >= pre && i >= 0)
//  while(PQ[i].priority >= pre && i >= 0) /* Find location for new elem */
  {
   PQ[i+1]=PQ[i];
   i--;
  }
  PQ[i+1] = elem;
 }
}


Process PQdelete()
{                      /* Function for Delete operation */
 Process p;
 if(Qempty()){ printf("nnUnderflow!!!!nn");
 return(p); }
 else
 {
  p=PQ[f];
  f=f+1;
  return(p);
 }
}

void dequeue(){
    Process temp = PQ[0];
    for(int i = 0; i < SIZE-1; i++){
        PQ[i] = PQ[i+1];
    }
    PQ[SIZE-1] = temp;
}

void display()
{                  /* Function to display status of Queue */
 int i;
 if(Qempty()) printf(" n Empty Queuen");
 else
 {
  printf("Front->");
  for(i=f;i<=r;i++)
   printf("[%d,%d] ",PQ[i].id,PQ[i].runingTime);
  printf("<-Rear");
 }
}


// Process waitingQ[SIZE];
Process* waitingQ;
int waitingFront = 0;
int waitingBack = -1;

void PQinsertWaiting(Process elem, int pre) {
  int i = waitingBack;
  ++waitingBack;
  while(waitingQ[i].runingTime >= pre && i >= 0) {
    waitingQ[i+1] = waitingQ[i];
    i--;
  }
  waitingQ[i+1] = elem;
}