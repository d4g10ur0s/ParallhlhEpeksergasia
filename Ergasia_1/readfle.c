#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "mmio.c"
#include "mmio.h"

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
//Done
void limit_neighbours(node *komvos,m_stack *stk){
    //arxikopoihsh komvou
    komvos->geitones = (int *)malloc(stk->b_size);
    komvos->b_size = stk->b_size;
    komvos->s = stk->num;
    //kratw tous geitones me va8mo eisodou = 0
    for(int i = 0; i<stk->num; i++){komvos->geitones[i] = stk->stk[i].num-1;}

    komvos->geitones = (int *)realloc(komvos->geitones,komvos->b_size + sizeof(int) );
    komvos->geitones[komvos->s] = -1;//to telos twn geitwnwn

}

void print_a_node(node *graph,node *n){
  printf("Node Number : %i\n",n->num );
  //printf("Neigbour Number : %i\n",n->s );
  for(int i = 0; i<n->s; i++){
    node temp = graph[n->geitones[i]];
    print_a_node(graph,&temp );
  }
}
//Done
void stk_push(m_stack *stk,node *n){
  stk->b_size+=sizeof(*n);
  stk->stk = (node *)realloc(stk->stk,stk->b_size);
  stk->stk[stk->num] = *n;
  stk->num+=1;
}
//Done
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
//Done
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

//Done
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
//Done
m_stack kahn(node *graph,node *sorted){

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
      node temp = graph[ sorted->geitones[sorted->s - i] ];
      printf("O kahn vrike : %i\n",temp.num );
      stk_push(&stack ,&temp );
    }
  }

    return stack;
}

void main(int argc,char *argv[]){

  if(argc!=3){
    printf("La8os plh8os argument, prwta to path meta to plh8os twn threads\n");
  }


  clock_t start, end;
  start = clock();
  //posoi komvoi
  int m_size;
  int line[2];
  //sxetika me mmio
  MM_typecode t;
  //h stoiva ths main
  m_stack main_stack;
  omp_init_lock(&main_stack.nlock);

  FILE *f;
  f = fopen(argv[1],"r+");

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
       cook(&arr[pros-1],&arr[apo-1],pros-1);
   }
   fclose(f);

  //gemizw thn main_stack
  stk_init(arr,m_size,&main_stack);
  m_stack arxikoi;
  stk_init(arr,m_size,&arxikoi);
  if(main_stack.num==0){
    printf("To grafhma einai kukliko\n");
    exit(0);
  }

  //posa threads
  omp_set_num_threads( (int ) *argv[2]);

  /*
  uparxei mia metavlhth shared koinh,
  gia ola ta threads shmatodotei
  to pote teleiwnei
  */
  int shrd = m_size;
  omp_lock_t shared_lock;
  omp_init_lock(&shared_lock);

  #pragma omp parallel shared(shrd, arxikoi)
  {
    //o komvos pou eksetazetai
    node on_process;
    while(shrd-1 > 0){

      #pragma omp single
      {

        if(main_stack.num==0 && shrd-1 > 0){
          printf("To Grafhma Einai kukliko\n");
          exit(0);
        }

        #pragma omp task shared(main_stack)
        {
          m_stack temp_stack;
          while(main_stack.num>0){
              omp_set_lock(&main_stack.nlock);
              on_process = stk_pop(&main_stack);
              omp_unset_lock(&main_stack.nlock);
              temp_stack = kahn(arr,&on_process);
              limit_neighbours(&arr[on_process.num-1] ,&temp_stack);

              omp_set_lock(&main_stack.nlock);
              //printf("H temp exei mege8os : %i\n", temp_stack.num);
              for(int i = 0; i<temp_stack.num; i++){printf("Ari8mos node : %i\n", temp_stack.stk[i].num);}
              while(temp_stack.num>0)
              {
                node temp = stk_pop(&temp_stack);
                //printf("8a mpei se main %i\n", temp.num);
                stk_push(&main_stack,&temp);
              }
              omp_unset_lock(&main_stack.nlock);

              omp_set_lock(&shared_lock);
              shrd-=1;
              omp_unset_lock(&shared_lock);
            }//end task
          }//end for

        //end omp for

      }//end omp single

    }//end of while

    #pragma omp master
    {
      end = clock();
      double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      while(arxikoi.num>0){
        node temp = stk_pop(&arxikoi);
        print_a_node(arr,&temp);
      }
      printf("Xronos : %f\n", cpu_time_used);
    }
    #pragma omp barrier
    {
      exit(0);
    }

 //end of omp parallel
}

}//end of main
