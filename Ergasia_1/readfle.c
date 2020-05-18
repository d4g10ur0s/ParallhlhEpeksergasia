#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
pairnei enan komvo kai vgazei ton prwto se seira geitona
xrhsh mono gia komvous me s>=1
*/
int pop_a_neighbor(node *n){

  int to_be_poped = n->geitones[0];

  int *temp = (int *)malloc(n->b_size);
  memcpy(temp,n->geitones,n->b_size);
  n->b_size-=sizeof(n->geitones[0]);
  n->s-=1;
  n->geitones = (int *)realloc(n->geitones,n->b_size);
  for(int i = 0; i<n->s; i++){n->geitones[i] = temp[i+1];}

  return to_be_poped;

}

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

 printf("\n[apo] = %i -> [pros] = %i\n",n2->num,n1->num);

 //node->num <=> node number
  n1->va8mos+=1;
  if(n2->s == 0){
    n2->geitones = (int *)malloc(sizeof(int *) );
    n2->b_size = sizeof(int *);
  }else{
    n2->b_size += sizeof(int *);
    n2->geitones = (int *)realloc(n2->geitones ,n2->b_size);
  }

  n2->geitones[n2->s] = geit;
  n2->s+=1;
}


unsigned int topological_sort(node *graph, unsigned int size,node *sorted){

    //allagh
    unsigned int sorted_size = 0;
    node first ;

    m_stack stack;
    stack.num = 0;
    stk_init(graph,size,&stack);

    // Get the nodes with no incoming edges
    //allagh
    unsigned int nodes_size = stack.num ; //To mege8os ths stoivas
    // Main loop
    while (nodes_size > 0) {
        // Get first node
        first = stk_pop(&stack);
        printf("Eksetazetai %i\n", first.num);
        //vlepw poioi komvoi exoun va8mo eisodou 0
        for(int i = 0; i<first.s; i++){

          graph[ first.geitones[i] ].va8mos -= 1;
          if(graph[ first.geitones[i] ].va8mos == 0){
            node temp = *(graph + first.geitones[i] );
            stk_push(&stack ,&temp );
          }

        }
        // Add it to the sorted array
        sorted_size++;
        sorted[sorted_size] = first;

        //ananewnw to mege8os ths stoivas
        nodes_size = stack.num ;
    }

    return 0;
}



void main(){
  //posoi komvoi
  int m_size;
  int line[2];

  MM_typecode t;

  m_stack stack;

  FILE *f;
  f = fopen("C:\\Users\\aleda\\Downloads\\ParallhlhEpeksergasia-master\\ParallhlhEpeksergasia-master\\Ergasia_1\\mycielskian3.mtx","r+");

  if (mm_read_banner(f,&t) != 0)
  {
      printf("Could not process Matrix Market banner.\n");
      exit(1);
  }
  //read file
  //returns 0 in case of error
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
       printf("apo=%i",apo);
       cook(&arr[pros-1],&arr[apo-1],pros);
   }
   fclose(f);

  //gia test sto function pop_a_neighbor()
  //printf("%d\n", pop_a_neighbor(&arr[0]) );

  const unsigned int order = m_size; // nodes
  node acyclic[order];

  topological_sort(arr, order, acyclic);
  for (int i = 0; i < order; i++) {
      printf("%u \n", acyclic[i].num);
  }

  printf("Telos\n");
}
