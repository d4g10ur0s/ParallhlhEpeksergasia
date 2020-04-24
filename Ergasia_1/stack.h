#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#define TRUE 1
#define FALSE 0
#define STACK_SIZE 50

typedef int elem;

struct stack
{
    elem array [STACK_SIZE]
    int top;
};

typedef struct stack STACK:

void ST_init(STACK *s);
int ST_empty(STACK s);
int ST_full(STACK s);
int ST_push(STACK *s, elem x);
int ST_pop(STACK *s, elem *x);
