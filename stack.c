/*	stack.c

	Implementation of a LIFO stack abstract data type.

	by: Steven Skiena
	begun: March 27, 2002

Copyright 2003 by Steven S. Skiena; all rights reserved.

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

These routines were adapted by Mauricio Roman, April 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"

#define STACKSIZE   2000

/* This function frees existing memory and initializes the stack
   It should not be called unless a stack has already been created */
void clear_stack(stack *s)
{
       free(s->s);
       init_stack(s);
}

void init_stack(stack *s)
{
        s->top = -1;
        //printf("Max Stack Count %d\n", s->count);
        s->count = 0;
        s->s=malloc(STACKSIZE*sizeof(int));
}

int push(stack *s, int x)
{
        if (s->count >= STACKSIZE){
            printf("Warning: stack overflow x=%d\n",x);
            printf("Size = %d\n",s->count);
            exit(EXIT_FAILURE);
        }
        else {
            s->top = s->top + 1;
            s->s[ s->top ] = x;
            s->count = s->count + 1;
            return 0;
        }
}

int pop(stack *s)
{
        int x;

        if (s->count <= 0)
            printf("Warning: empty stack.\n");
        else {
                x = s->s[ s->top ];
                s->top = s->top - 1;
                s->count = s->count - 1;
        }

        return(x);
}

int empty_stack(stack *s)
{
        if (s->count <= 0) return (TRUE);
        else return (FALSE);
}

int stack_size(stack *s)
{
        return s->count;
}

void print_stack(stack *s)
{
        int i;				/* counter */
        for (i=0; i<s->count; i++)
            printf("%d ",s->s[i]);

        printf("\n");
}



