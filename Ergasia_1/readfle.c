#include <stdio.h>
#include <stdlib.h>
#include "mmio.c"
#include "mmio.h"

typedef struct {
  int va8mos;
  int s; //size of geitones (int)
  size_t b_size;//se bytes,gia geitones
  int *geitones;
}node;


void cook(node *n,int geit){
  //n --> node
 //geit --> ari8mos geitona
  n->va8mos+=1;
  if(n->va8mos == 1){
    n->geitones = (int *)malloc(sizeof(int));
    n->b_size = sizeof(int);
  }else{
    n->b_size += sizeof(int);
    n->geitones = (int *)realloc(n->geitones ,n->b_size);
  }
  printf("%d\n", sizeof(*n->geitones));

  n->geitones[n->s] = geit;
  n->s+=1;
}

void main(){
  int m_size;
  int line[2];
  MM_typecode t;

  FILE *f;
  f = fopen("F:\\Sxolh\\3o Etos\\B' eksamhno\\ParallhlhEpeksergasia\\Ergasia_1\\ibm32.mtx","r+");

  mm_read_banner(f,&t);
  mm_read_mtx_array_size(f,&m_size,&m_size);
  printf("%i\n", m_size);


  //diavazei line
  //.
  //.
  //.
  node arr[m_size];
  //Arxikopoihsh
  for(int j=0; j<=m_size; j++){
    arr[j].va8mos = 0;
    arr[j].s = 0;
    printf("%i %i %i\n",arr[j].va8mos , arr[j].s ,j );
  }
  while(!feof(f))
   {
     int apo,pros;
       fscanf(f, "%d %d\n", &apo, &pros);
       cook(&arr[pros],apo);
       printf("**********\nApo : %d \nPros : %d\n**********\n\n",apo, pros );
   }
   fclose(f);
  //loop gia lines
  //.
  //.
  //.
  //cook(&arr[m_size-1],line[1]);
  for(int i = 0; i<m_size; i++){
    printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",i ,arr[i].va8mos ,arr[i].s );
  }
  printf("Telos\n");
}
