#include <stdlib.h>
#include <stdbool.h>

#define __CALLOCS_IMPL
#include "callocs.h"

struct __meta_node
{
    bool allocated;
    unsigned int size;
    struct __meta_node *prev;
    struct __meta_node *next;
};

static struct __meta_node *mem_pool;
static size_t pool_size;

int callocs_init(size_t size)
{
    // size /= sizeof(struct __meta_node);
    // size *= sizeof(struct __meta_node);

    // use original malloc only once
    mem_pool = malloc(size);
    if (mem_pool == NULL)
        return -1;

    // "init" first meta node
    struct __meta_node *node = mem_pool;
    node->allocated = false;
    node->size = size - sizeof(struct __meta_node);
    node->prev = NULL;
    node->next = NULL;
    pool_size = size;
    return 0;
}

void callocs_destroy(void)
{
    free(mem_pool);
}

void *callocs_malloc(size_t size)
{
    struct __meta_node *node = mem_pool;

    while (node != NULL && (node->allocated || node->size < size))
        node = node->next;

    if (node == NULL)
        return NULL;

    // break condition
    if (node->size > size + sizeof(struct __meta_node))
    {
        struct __meta_node *next = node->next;
        struct __meta_node *new_next = (struct __meta_node *)((char *)node + size + sizeof(struct __meta_node));
        new_next->allocated = false;
        new_next->size = node->size - size - sizeof(struct __meta_node);
        new_next->prev = node;
        new_next->next = next;
        node->size = size;
        node->next = new_next;
    }

    node->allocated = true;
    return node + 1;
}

void *callocs_сalloc(size_t num, size_t size)
{
    void *data = callocs_malloc(num * size);

    if (data != NULL)
        for (size_t i = 0; i < num * size; i++)
            *((char *)data + i) = '\0';

    return data;
}

void *callocs_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return size == 0 ? NULL : callocs_malloc(size);

    if ((char *)ptr < (char *)(mem_pool + 1) || (char *)ptr >= (char *)mem_pool + pool_size)
        return ptr;

    if (size == 0U)
    {
        callocs_free(ptr);
        return NULL;
    }
    else
    {
        struct __meta_node *node = ((struct __meta_node *)ptr - 1);
        if (node->size < size)
        {
            // actual realloc
            // 1. grab next blocks
            while (node->size < size && node->next != NULL && !node->next->allocated)
            {
                node->size += node->next->size + sizeof(struct __meta_node);
                node->next = node->next->next;
            }
            // 2. look for prev free block
            if (node->prev != NULL && node->size + node->prev->size + sizeof(struct __meta_node) >= size)
            {
                // ok. merge them and move memory
                node->prev->size += node->size + sizeof(struct __meta_node);
                node->prev->next = node->next;

                for (unsigned int mem_size = node->size, i = 0; i < mem_size; i++)
                    ((char *)(node->prev + 1))[i] = ((char *)(node + 1))[i];

                return node->prev + 1;
            }
            else // 3. look for completely new free space
            {
                void *res = callocs_malloc(size);
                if (res == NULL)
                    return NULL; // bad realloc

                for (unsigned int i = 0; i < node->size; i++)
                    ((char *)res)[i] = ((char *)(node + 1))[i];
                
                callocs_free(ptr);
                return res;
            }
        }
        else if (node->size > size + sizeof(struct __meta_node)) // shrink occupied memory
        {
            struct __meta_node *next = node->next;
            struct __meta_node *new_next = (struct __meta_node *)((char *)node + size + sizeof(struct __meta_node));
            new_next->allocated = false;
            new_next->size = node->size - size - sizeof(struct __meta_node);
            new_next->prev = node;
            new_next->next = next;
            node->size = size;
            node->next = new_next;
        }

        return ptr;
    }
}

void callocs_free(void *ptr)
{
    if ((char *)ptr < (char *)(mem_pool + 1) || (char *)ptr >= (char *)mem_pool + pool_size)
        return;

    struct __meta_node *node = (struct __meta_node *)((char *)ptr - sizeof(struct __meta_node));
    if (node->allocated)
    {
        // merge with prev.
        if (node->prev != NULL && !node->prev->allocated)
        {
            node->prev->size += node->size + sizeof(struct __meta_node);
            node->prev->next = node->next;
            node = node->prev;
        }
        else
            node->allocated = false;

        // merge with next
        if (node->next != NULL && !node->next->allocated)
        {
            node->size += node->next->size + sizeof(struct __meta_node);
            node->next = node->next->next;
        }
    }
}

int printf_heap_info(void)
{
    int alloc_blocks_count = 0;

    printf("Heap info:\n");
    for (struct __meta_node *node = mem_pool; node != NULL; node = node->next)
    {
        if (node->allocated)
            alloc_blocks_count++;
        printf("  %s block ", (node->allocated ? "allocated" : "free"));
        printf("of size %u(%lu) with data pointer %p\n", node->size, node->size + sizeof(struct __meta_node), (void *)(node + 1));
    }

    return alloc_blocks_count;
}
