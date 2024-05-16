#include <dynam.h>

int main()
{

    vector *v = initVector(5);
    setValue(v, 0, 5);
    setValue(v, 1, 10);
    setValue(v, 2, 20);
    setValue(v, 3, 30);
    setValue(v, 4, 40);

    for(int i = 0; i < v->size; i++)
    {
        printf("Vector value at index %i: %i\n", i, v->data[i]);
    }

    printf("Vector size: %i\n", v->size);

    resizeVector(v, 10);

    printf("New Vector size: %i\n", v->size);

    setValue(v, 5, 50);
    setValue(v, 6, 60);
    setValue(v, 7, 70);
    setValue(v, 8, 80);
    setValue(v, 9, 84);

    for(int i = 0; i < v->size; i++)
    {
        printf("Vector value at index %i: %i\n", i, v->data[i]);
    }

    deleteVector(v);

    return 0;
}

