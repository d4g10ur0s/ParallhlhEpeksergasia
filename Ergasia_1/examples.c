#include <stdio.h>
#include <stdlib.h>

void do_i(int *k){
  *k = 5;
}

void main(){
  int arr[3];
  for(int i = 0; i<=2; i++){
    do_i(&arr[i]);
    printf("%d\n", arr[i]);
  }
}
