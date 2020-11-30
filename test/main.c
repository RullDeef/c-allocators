#include <stdio.h>

#define CALLOCS_REDEFINE
#include "callocs.h"

int test_single(void);
int test_frag(void);

int main(void)
{
    int status = 0;

    status = status || test_single();
    status = status || test_frag();

    return status;
}

int test_single(void)
{
    int status = 0;

    callocs_init(100);
    callocs_fprintf_heap_info(stdout);

    int *array = realloc(NULL, 10 * sizeof(int));
    if (array != NULL)
    {
        printf("array initialized!\n");
        callocs_fprintf_heap_info(stdout);

        for (int i = 0; i < 10; i++)
            array[i] = i;

        printf("array members:");
        for (int i = 0; i < 10; i++)
            printf(" %d", array[i]);
        printf("\n");

        printf("trying to allocate 11 bytes... ");
        void *data = malloc(11);
        if (data != NULL)
        {
            printf("success!\n");
            callocs_fprintf_heap_info(stdout);
            free(data);
            printf("data freed!\n");
        }
        else
        {
            printf("fail!\n");
            status = -1;
        }

        realloc(array, 0);
        printf("array freed!\n");
        callocs_fprintf_heap_info(stdout);
    }
    else
    {
        printf("array not initialized!\n");
        status = -1;
    }

    callocs_destroy();
    return status;
}

int test_frag(void)
{
    int status = 0;

    callocs_init(1000);
    callocs_fprintf_heap_info(stdout);

    int *ptrs[4];
    ptrs[0] = malloc(16);
    ptrs[1] = malloc(16);
    ptrs[2] = malloc(16);
    ptrs[3] = malloc(16);

    free(ptrs[0]);
    free(ptrs[2]);
    free(ptrs[1]);
    callocs_fprintf_heap_info(stdout);

    free(ptrs[3]);
    printf("freed last pointer: %p\n", (void *)ptrs[3]);
    callocs_fprintf_heap_info(stdout);

    callocs_destroy();
    return status;
}
