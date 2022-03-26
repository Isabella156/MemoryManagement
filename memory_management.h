#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// size of the structrue of the memory block
# define METABLOCK_SIZE sizeof(metaBlock)
// size of initialize memory block
# define MEMBLOCK_SIZE 2 * 1024 * 1024
// size of memory alignment
# define ALIGN 8

// structure of the memory block
typedef struct _metaBlock{
    size_t size;
    int free;
    struct _metaBlock* next;
    struct _metaBlock* prev;
} metaBlock;

// head node of the memory block
metaBlock* head;

// current memory block pointer
metaBlock* currentPtr;

metaBlock* extendBlock(size_t size);
void splitMemBlock(metaBlock* fit, size_t size);
size_t memAlignment(size_t size);
int checkAddress(void *p);
metaBlock* checkNode(void* p);
int merge(metaBlock* free);
int init();
void* _allocate(size_t size, metaBlock* block);
void* _malloc (size_t size);
void*_free(void*ptr);