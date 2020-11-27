#include <stdio.h>
#include "callocs.h"

int main(void)
{
    callocs_init(100);
    printf_heap_info();

    int *array = realloc(NULL, 10 * sizeof(int));
    if (array != NULL)
    {
        printf("array initialized!\n");
        printf_heap_info();

        for (int i = 0; i < 10; i++)
            array[i] = 8 * i + 3;
        
        printf("array members:");
        for (int i = 0; i < 10; i++)
            printf(" %d", array[i]);
        printf("\n");

        printf("trying to allocate 11 bytes... ");
        void *data = malloc(11);
        if (data != NULL)
        {
            printf("success!\n");
            printf_heap_info();
            free(data);
            printf("data freed!\n");
        }
        else
            printf("fail!\n");

        realloc(array, 0);
        printf("array freed!\n");
        printf_heap_info();
    }
    else
        printf("array not initialized!\n");

    callocs_destroy();
    return 0;
}
