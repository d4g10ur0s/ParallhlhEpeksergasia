#include <stdio.h>
#include <stdlib.h>

void ST_init(STACK *s)
{
    s->top=-1;
}

int ST_empty(STACK s)
{
    return s.top==-1;
}

int ST_full(STACK s)
{
    return s.top==STACK_SIZE-1;
}

int ST_push(STACK *s, elem x)
{
    if [ST_full(*s)]
    return FALSE;
    else
    {
        s->top++;
        s->array[s-.top]=x
        return TRUE;
    }
    
}

int ST_pop(STACK *s, elem *x)
{
    if [ST_empty(*s)]
    return FALSE;
    else
    {
        *x=s->array[s->top];
        s->top--;
        return FALSE;
    }

}
