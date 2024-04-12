#include "dynam.h"

vector *initVector(size_t size)
{
    vector *temp = malloc(sizeof(vector));
    if(temp)
    {
        temp->data = malloc(size * sizeof(int));
        temp->size = size;
    }
    return temp;
}

void deleteVector(vector *v)
{
    if(v)
    {
        free(v->data);
        free(v);
    }
}

size_t resizeVector(vector *v, size_t n)
{
    if(v) 
    {
        int *temp = realloc(v->data, n * sizeof(int));
        if(temp)
        {
            v->data = temp;
            v->size = n;
        }
        return v->size;
    }
    return 0;
}

int getValue(vector *v, size_t n)
{
    if(v && v->size)
    {
        return v->data[n];
    }

    return -1; // vector to small or to big
}

void setValue(vector *v, size_t n, int x) 
{
    if(v)
    {
        if(n >= v->size)
        {
            resizeVector(v, n);
        }
        v->data[n] = x;
    }
}
