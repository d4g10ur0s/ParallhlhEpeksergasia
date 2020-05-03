#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "readfle.c"

typedef struct {
  int size;
  size_t size_in_bytes;
  node** nodes;
}m_hashtable;

m_hashtable constructor(node *array,int arrsize,int arrsizeinbytes){
  m_hashtable to_be_returned;
  to_be_returned.size = 0;
  to_be_returned.size_in_bytes = 0;
  to_be_returned.nodes = (node **)malloc(arrsizeinbytes);

  //Arxikopoihsh
  for(int i = 0; i<arrsize; i++){
    to_be_returned.nodes[i] = (node *)malloc( sizeof(array[i]) );
    to_be_returned.nodes[i][0] = array[i];
    to_be_returned.size_in_bytes += sizeof(array[i]);
  }

  return to_be_returned;
}


int *get_size(node *array){
  
  int size = 0;
  int counter = 0;
  while(errno!=12 || errno!=13){
    size+=sizeof(array[counter]);
    counter++;
  }
  return size;
}

//add an element to position i
m_hashtable add_to_position(m_hashtable * hashtb,node *n){

  //Aukshsh mege8ous tou hashtable
  hashtb->size_in_bytes += sizeof(*n);
  hashtb->nodes = (node **)realloc(hashtb,hashtb->size_in_bytes);

  //Aukshsh mege8ous tou entry tou hashtable
  int temp_size = get_size(hashtb->nodes[ n->geitones[0] ]);
  hashtb->nodes[ n->geitones[0] ] = (node *)realloc(hashtb->nodes[ n->geitones[0] ], temp_size);


}
