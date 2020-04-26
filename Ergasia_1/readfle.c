#include <stdio.h>
#include <stdlib.h>

#include "mmio.c"
#include "mmio.h"

typedef struct {
  int num;
  int va8mos;
  int s; //size of geitones (int)
  int b_size;//se bytes,gia geitones
  int *geitones;
}node;


node * stack_init(node *array,int size,node *stk,int *counter){
  int btsz = 0;
  //Apla desmeuw ligo xwro sthn mnhmh
  stk = (node *)malloc(sizeof(node));
  //loop gia na gemisw stack me komvous pou exoun va8mo eisodou 0
  for(int i = 0; i<=size; i++){

    if(array[i].va8mos == 0){
      //to mege8os (se byte) tou stack prepei na auksh8ei kata to mege8os (se byte) tou epomenou pros apo8hkeush komvou
      btsz+=sizeof(array[i]);
      //epanaupologismos mege8ous ths stack
      stk = (node *)realloc(stk,btsz);
      //apo8hkeush tou neou stoixeioy sthn stack
      stk[*counter] = array[i];
      //aykshsh tou counter kata ena, vasei aytou euresh tou top
      *counter+=1;
    }else{
      continue;
    }

  }

  return stk;
}

void cook(node *n1,node *n2,int geit){
  //n --> node
 //geit --> ari8mos geitona
  n2->va8mos+=1;
  if(n1->s == 0){
    n1->geitones = (int *)malloc(sizeof(int *));
    n1->b_size = sizeof(int *);
  }else{
    n1->b_size += sizeof(int *);
    n1->geitones = (int *)realloc(n1->geitones ,n1->b_size);
  }

  n1->geitones[n1->s] = geit;
  n1->s+=1;
}


void main(){
  //posoi komvoi
  int m_size;
  int line[2];
  MM_typecode t;

  FILE *f;
  f = fopen("F:\\Sxolh\\3o Etos\\B' eksamhno\\ParallhlhEpeksergasia\\Ergasia_1\\ibm32.mtx","r+");

  mm_read_banner(f,&t);
  mm_read_mtx_array_size(f,&m_size,&m_size);
  printf("%i\n", m_size);

  node arr[m_size];

  //Arxikopoihsh
  for(int j=0; j<m_size; j++){
    arr[j].num = j+1;
    arr[j].va8mos = 0;
    arr[j].s = 0;
    arr[j].b_size = 0;
  }
  //Diavasma Arxeiou
  while(!feof(f))
   {
     int apo,pros;
       fscanf(f, "%d %d\n", &apo, &pros);
       cook(&arr[pros-1],&arr[apo-1],apo);
   }
   fclose(f);

   //Stack
   int m_counter = 0;
   node *m_stack = (node *)malloc(sizeof(node));
   m_stack = stack_init(&arr,m_size,m_stack,&m_counter);
   for(int j = 0; j<m_counter; j++){ printf("%d\n", m_stack[j].num); }

  //Error Checking
  for(int i = 0; i<m_size; i++){
    printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",i ,arr[i].va8mos ,arr[i].s );
  }

  printf("Telos\n");
}
