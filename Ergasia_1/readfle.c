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

typedef struct {
    unsigned int first;
    unsigned int second;
} edge;

// Find out if a node has no incoming edges
//the number of edges => size
static unsigned int is_root(const edge *graph, const unsigned int *edges, unsigned int size,
        unsigned int v)
{
    unsigned int a, root = 1;
    for (a = 0; a < size && root; a++) {
        root = !edges[a] || graph[a].second != v;
    }
    return root;
    printf("graph[%a]=\n",a,graph[a]);
}


// Get the nodes with no incoming edges
//the number of nodes => order
static unsigned int get_roots(const edge *graph, const unsigned int *edges, unsigned int size,
        unsigned int order, unsigned int *nodes)
{
    unsigned int v, nodes_size = 0;

    for (v = 0; v < order; v++) {
        if (is_root(graph, edges, size, v)) {
            nodes[v] = 1;
            nodes_size++;
        }
    }
    return nodes_size;
}

unsigned int topological_sort(node *graph, unsigned int size, unsigned int order, 
        unsigned int **sorted)
{
    unsigned int *nodes = calloc(order, sizeof(unsigned int));
    unsigned int *edges = malloc(size * sizeof(unsigned int));
    *sorted = malloc(order * sizeof(unsigned int));
    unsigned int v, a, nodes_size, sorted_size = 0,
            edges_size = size;
    if (!(nodes && edges && *sorted)) {
        free(nodes);
        free(edges);
        free(*sorted);
        *sorted = NULL;
        return 0;
    }
    // All edges start off in the graph
    for (a = 0; a < size; a++) {
        edges[a] = 1;
    }
    // Get the nodes with no incoming edges
    nodes_size = get_roots(graph, edges, size, order, nodes);
    // Main loop
    while (nodes_size > 0) {
        // Get first node
        for (v = 0; nodes[v] != 1; v++);
        // Remove from node set
        nodes[v] = 0;
        nodes_size--;
        // Add it to the sorted array
        (*sorted)[sorted_size++] = v;
        // Remove all edges connecting it to its neighbours
        for (a = 0; a < size; a++) {
            if (edges[a] && graph[a].first == v) {
                edges[a] = 0;
                edges_size--;
                // Check if neighbour is now a root
                if (is_root(graph, edges, size, graph[a].second)) {
                    // Add it to set of nodes
                    nodes[graph[a].second] = 1;
                    nodes_size++;
                }
            }
        }
    }
    free(nodes);
    free(edges);
    return edges_size == 0;
}

/* Connect two edges */
void edge_connect(edge *edges, unsigned int first, unsigned int second,
        unsigned int *pos)
{
    edges[*pos].first = first;
    edges[*pos].second = second;
    (*pos)++;
}



void main(){
  //posoi komvoi
  int m_size;
  int line[2];

  MM_typecode t;

  m_stack stack;

  FILE *f;
  f = fopen("C:/Users/user/ParallhlhEpeksergasia/Ergasia_1/mycielskian3.mtx","r+");

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
       cook(&arr[pros-1],&arr[apo-1],apo);
   }
   fclose(f);

  //Error Checking
  for(int i = 0; i<m_size; i++){
    printf("arr[%i]=arr[%i]\n",i,arr[i]);
    printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",i ,arr[i].va8mos ,arr[i].s );
  }

  stk_init(arr,m_size,&stack);

  printf("***Stack Time***\n\nStack Size = %i\n\n",stack.num);

  for(int j = 0; j<stack.num; j++){printf("*****\nNode : %i\nVa8mos Eisodou : %i\nGeitones : %i\n******\n\n",j ,stack.stk[j].va8mos ,stack.stk[j].s );}
  //gia test sto function pop_a_neighbor()
  //printf("%d\n", pop_a_neighbor(&arr[0]) );

  const unsigned int order = m_size; // nodes
  node acyclic[order];
  unsigned int i = 0;
  unsigned int *sorted;


    acyclic = topological_sort(edges, size, order, &sorted);
    printf("Graph is acyclic: %u\n", acyclic);
    for (i = 0; i < order; i++) {
        printf("%u ", sorted[i]);
    }
    putchar('\n');

    free(sorted);
    free(edges);

    return 0;
}
  printf("Telos\n");
}
