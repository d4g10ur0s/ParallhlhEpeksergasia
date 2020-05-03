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

typedef struct{
  int b_size;
  int num;
  node *stk;
}m_stack;

void stk_push(m_stack *stk,node *n){
  stk->b_size+=sizeof(*n);
  stk->stk = (node *)realloc(stk->stk,stk->b_size);
  stk->stk[stk->num] = *n;
  stk->num+=1;
}

node stk_pop(m_stack *stk){
  stk->num-=1;
  stk->b_size-= sizeof(stk->stk[stk->num]);
  node ret = stk->stk[stk->num];
  stk->stk = (node *)realloc(stk->stk,stk->b_size);
  return ret;
}

void stk_init(node *arr,int num,m_stack *stk){
  stk->num = 0;
  stk->b_size = 1;
  stk->stk = (node *)malloc(sizeof(node *));
  for(int i = 0; i<num; i++){
    if(arr[i].va8mos == 0){

      stk->b_size+=sizeof(arr[i]);
      stk->stk=(node *)realloc(stk->stk,stk->b_size);
      stk->stk[stk->num] = arr[i];
      stk->num+=1;

    }
    else{continue;}

  }
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

  m_stack stack;

  FILE *f;
  f = fopen("F:\\Sxolh\\3o Etos\\B' eksamhno\\ParallhlhEpeksergasia\\Ergasia_1\\mycielskian3.mtx","r+");

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

  //Error Checking
  for(int i = 0; i<m_size; i++){
    printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",i ,arr[i].va8mos ,arr[i].s );
  }

  stk_init(arr,m_size,&stack);

  printf("***Stack Time***\n\nStack Size = %i\n\n",stack.num);

  for(int j = 0; j<stack.num; j++){printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",j ,stack.stk[j].va8mos ,stack.stk[j].s );}

  printf("Telos\n");
}
