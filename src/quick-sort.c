#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 5

typedef struct a {
  int a;
}A;
int show_A(A array[]);

void swap(int *x, int *y)
{
  int temp; // 値を一時保存する変数

  temp = *x;
  *x = *y;
  *y = temp;
}

int partition(A array[], int left, int right, int index[])
{
  int i, j;
  i = left;
  j = right;
  int pivot_value = array[(left+right)/2].a; // 中央の要素をpivotとする

  do
  {
    while(array[i].a < pivot_value) i++;
    while(array[j].a > pivot_value) j--;
    // pivotより小さいものを左へ、大きいものを右へ
    swap(&array[i].a, &array[j].a);
    swap(&index[i], &index[j]);
    // show_A(array);
  } while (i < j);
  swap(&array[i].a, &array[j].a);
  swap(&index[i], &index[j]);

  return j;
}

int quick_sort(A array[], int left, int right, int index[]){
  int j;

  if (left < right){
    j = partition(array, left, right, index);
    // printf("partition\n");
    // show_A(array);
    // printf("j :%d\n\n",j);
    quick_sort(array, left, j-1, index);
    quick_sort(array, j+1, right, index);
  }
}

int show_A(A *arr){
  for (int i = 0; i < N; i++)
  {
    printf("%d: %d\n", i, arr[i].a);
  }
  printf("\n");
}

int show_array(int *arr){
  for (int i = 0; i < N; i++)
  {
    printf("%d: %d", i, arr[i]);
  }
  printf("\n");
}

int main(){
  A sta[5];
  sta[0].a = 5;
  sta[1].a = 3;
  sta[2].a = 9;
  sta[3].a = 6;
  sta[4].a = 1;
  int data[N] = {5,3,9,6,1};
  int index[N];
  for (int i=0; i < N; i++) index[i] = i;

  show_A(sta);
  quick_sort(sta, 0, N-1, index);
  printf("Sort Complete\n");
  show_A(sta);
}
