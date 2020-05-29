#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmio.c"
#include "mmio.h"

#define T_NUM 2

typedef struct {
  int num;
  int va8mos;
  int s;  //  size of geitones (int)
  int b_size;  //  se bytes,gia geitones
  int *geitones;  //  ari8mos geitona
  omp_lock_t nlock;
}node;

typedef struct{
  int b_size;  //  mege8os stoivas se byte
  int num;  //  mege8os stoivas (int)
  node *stk;  //  h stoiva
  omp_lock_t nlock;
}m_stack;

/*
 sunarthsh gia na
 kratw mono tous geitones,
 pou me endiaferoun
*/
void limit_neighbours(node *komvos,m_stack *stk){
    //arxikopoihsh komvou
    free(komvos->geitones);
    komvos->geitones = (int *)malloc(sizeof(int));
    komvos->b_size = 0;
    //orismos twn geitonwn me va8mo eisodou = 0
    for(int i = 0; i<stk->num; i++){
      komvos->b_size += sizeof(stk->stk[i].num);
      komvos->geitones = (int *)realloc(komvos->geitones,sizeof(komvos->b_size));
      komvos->geitones[i] = stk->stk[i].num;
    }
    komvos->s = stk->num;
    komvos->geitones = (int *)realloc(komvos->geitones,komvos->b_size + sizeof(int) );
    komvos->geitones[komvos->s] = -1;//to telos twn geitwnwn
}

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
  //vlepw an h stoiva einai kenh
  if(stk->num==0){
    node n;
    n.num = -1;
    return n;
  }
  //pop an element
  stk->num-=1;
  stk->b_size-= sizeof(stk->stk[stk->num]);
  node ret = stk->stk[stk->num];
  stk->stk = (node *)realloc(stk->stk,stk->b_size);
  return ret;
}

/*
 pairnei ena sunolo komvwn kai topo8etei tous komvous me va8mo eisodou 0 se mia stoivas
          **** orismata ****
           arr -> sunolo komvwn
           num -> mege8os arr se int
           stk -> h stoiva
*/
void stk_init(node *arr,int num,m_stack *stk){
  //stk->on_process = 0;
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

/*
o algori8mos tou kahn
orismata :
        graph ->  oloklhro to grafhma
        sorted -> komvos me va8mo eisodou == 0
*/
m_stack topological_sort(node *graph,node *sorted){

  printf("Eksetazetai %i\n", sorted->num);
  m_stack stack;
  //arxikopoihsh ths stoivas
  stack.num = 0;
  stack.b_size=0;
  stack.stk = (node *)malloc(sizeof(node *));

  if(sorted->num == -1){return stack;}
  // Main loop
  for(int i = sorted->s; i>0; i--){
    //vlepw poioi komvoi exoun va8mo eisodou 0
    omp_set_lock( &graph[ sorted->geitones[sorted->s-i] ].nlock );
    graph[ sorted->geitones[sorted->s-i] ].va8mos -= 1;
    omp_unset_lock( &graph[ sorted->geitones[sorted->s-i] ].nlock );
    //sto shmeio auto uparxei mono 1 thread to
    //opoio vlepei pws o va8mos eisodou einai isos me 0
    if(graph[ sorted->geitones[sorted->s-i] ].va8mos == 0){
      node temp = *(graph + sorted->geitones[sorted->s-i] );
      stk_push(&stack ,&temp );
    }
  }

    return stack;
}



void main(){
  //posoi komvoi
  int m_size;
  int line[2];
  //sxetika me mmio
  MM_typecode t;
  //h stoiva ths main
  m_stack main_stack;

  FILE *f;
  f = fopen("/home/g4d/Desktop/eeee/ibm32.mtx","r+");

  if (mm_read_banner(f,&t) != 0)
  {
      printf("Could not process Matrix Market banner.\n");
      exit(1);
  }
  //read file
  //returns 0 in case of error
  mm_read_mtx_array_size(f,&m_size,&m_size);
  printf("%i\n", m_size);
  //apo8hkeush olwn twn komvwn se enan pinaka
  node arr[m_size];
  //uparxei ena lock gia ka8e komvo tou grafhmatos
  //init locks
  //Arxikopoihsh
  for(int j=0; j<m_size; j++){

    arr[j].num = j+1;
    arr[j].va8mos = 0;
    arr[j].s = 0;
    arr[j].b_size = 0;
    omp_init_lock(&arr[j].nlock);
  }
  //Diavasma Arxeiou
  while(!feof(f))
   {
     int apo,pros;
       fscanf(f, "%d %d\n", &apo, &pros);
       printf("apo=%i",apo);
       cook(&arr[pros-1],&arr[apo-1],pros-1);
   }
   fclose(f);

  //gemizw thn main_stack
  stk_init(arr,m_size,&main_stack);
  if(main_stack.num==0){
    printf("To grafhma einai kukliko\n");
    exit(0);
  }

  //o komvos pou eksetazetai
  node on_process;

  //posa threads
  omp_set_num_threads(T_NUM);

  /*
  uparxei mia metavlhth shared koinh,
  gia ola ta threads shmatodotei
  to pote teleiwnei
  */
  int shared = m_size;
  omp_lock_t shared_lock;
  omp_init_lock(&shared_lock);

  #pragma omp parallel
  {
    while(shared>0){
      omp_set_lock(&main_stack.nlock);
      on_process = stk_pop(&main_stack);
      omp_unset_lock(&main_stack.nlock);


    //ean h stoiva einai kenh to thread perimenei
    while(1){
      #pragma omp critical
      {
        if(on_process.num==-1){
          //h stoiva einai kenh
          on_process = stk_pop(&main_stack);
        }
      }
      if( !(on_process.num==-1) ){break;}
    }
    //sto shmeio auto ka8e thread exei enan komvo me va8mo eisodou == 0
    m_stack temp_stack = topological_sort(arr,&on_process);
    /*

    h metavlhth temp stack
    periexei tous geitones tou komvou
    pou eksetasthke me va8mo eisodou == 0

    */

    //ston pinaka oi geitones tou komvou meiwnontai se aytous pou exoun va8mo eisodou == 0
    limit_neighbours(&arr[on_process.num],&temp_stack);
    omp_set_lock(&main_stack.nlock);
    for(int j=0; j<=temp_stack.num; j++){
      node n_temp = stk_pop(&temp_stack);
      stk_push(&main_stack,&n_temp );
    }
    omp_unset_lock(&main_stack.nlock);

    omp_set_lock(&shared_lock);
    shared-=1;
    omp_unset_lock(&shared_lock);
    //end of while loop
  //end of omp parallel
  }
 }
}
//end of main
