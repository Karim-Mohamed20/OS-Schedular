// Priority Queue implementation in C
#include <stdio.h>
int size = 0;
void swap(Process* a, Process* b) {
  Process temp = *b;
  *b = *a;
  *a = temp;
}

// Function to heapify the tree
void heapify(Process array[], int size, int i) {
  if (size == 1) {
    printf("Single element in the heap");
  } else {
    // Find the largest among root, left child and right child
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < size && array[l].runingTime > array[largest].runingTime)
      largest = l;
    if (r < size && array[r].runingTime > array[largest].runingTime)
      largest = r;

    // Swap and continue heapifying if root is not largest
    if (largest != i) {
      swap(&array[i], &array[largest]);
      heapify(array, size, largest);
    }
  }
}

// Function to insert an element into the tree
void insert(Process array[], Process newProcess) {
  if (size == 0) {
    array[0] = newProcess;
    size += 1;
  } else {
    array[size] = newProcess;
    size += 1;
    for (int i = size / 2 - 1; i >= 0; i--) {
      heapify(array, size, i);
    }
  }
}

// Function to delete an element from the tree
void deleteRoot(Process array[], int id) {
  int i;
  for (i = 0; i < size; i++) {
    if (id == array[i].id)
      break;
  }

  swap(&array[i], &array[size - 1]);
  size -= 1;
  for (int i = size / 2 - 1; i >= 0; i--) {
    heapify(array, size, i);
  }
}

// Print the array
void printArray(Process array[], int size) {
  for (int i = 0; i < size; ++i)
    printf("%d ", array[i].runingTime);
  printf("\n");
}

// Driver code
int main() {
  int n = 3;
  Process* processes = malloc(sizeof(Process)*n);
  for(int i = 0; i < n; i++){
      Process newProcess;
      newProcess.id = i;
      newProcess.priority = (i) * 2;
      newProcess.runingTime = rand() % 10 +1;
      newProcess.arrival = (i+1) * 2;
      insert(processes, newProcess);
  }

  printf("Max-Heap array: ");
  printArray(processes, size);

  deleteRoot(processes, 1);

  printf("After deleting an element: ");

  printArray(processes, size);
}