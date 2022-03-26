#include "memory_management.h"
// use first fit to find the suitable memory block
// return NULL if no suitable block
// return the block pointer if found
metaBlock* findBlock(size_t size){
    currentPtr = head;
    while(currentPtr){
        if(currentPtr->free == 1 && currentPtr->size >= size){
            break;
        }
        if(currentPtr->next){
            currentPtr = currentPtr->next;
        }else{
            return NULL;
        }
    }
    return currentPtr;
}

// extend the heap if there is no enough space
metaBlock* extendBlock(size_t size){
    // get the current program break
    metaBlock* extend = (metaBlock*)sbrk(0);
    // check if the heap is extended successfully
    // !!! size of meta data block + size of the requested memory block
    if(sbrk(METABLOCK_SIZE + size) == (void*)-1){
        printf("Unable to allocate memory!\n");
        return NULL;
    }else{
        extend->size = size;
        extend->free = 0;
        // link the newly extended block to the end of the linked list
        currentPtr->next = extend;
        extend->next = NULL;
        extend->prev = currentPtr;
        return extend;
    }
}

// split the suitable memory block into two blocks
void splitMemBlock(metaBlock* fit, size_t size){
    metaBlock* new = (void*)((void*)fit + METABLOCK_SIZE+ size);
    metaBlock* next = fit->next;
    // change the property of new
    new->size = fit->size - size - METABLOCK_SIZE;
    new->free = 1;
    new->next = next;
    new->prev = fit;
    // change the property of fit
    fit->size = size;
    fit->free = 0;
    fit->next = new;
    // change the property of next
    if(next){
        next->prev = new;
    }
}

// implement memory alignment 
size_t memAlignment(size_t size){
    if(size % ALIGN != 0){
        size = size + (ALIGN - size % ALIGN);
    }
    return size;
}

// check if the address of p is valid
int checkAddress(void *p){
    void* tmp = p - METABLOCK_SIZE;
    // head node exists, p within the malloc range, and p 
    // is the address of the memory block not the meta data block
    if(head && p > (void*)head && p < sbrk(0)){
        return 1;
    }else{
        return 0;
    }
}

metaBlock* checkNode(void* p){
    metaBlock* tmp = p - METABLOCK_SIZE;
    metaBlock* ptr = head;
    while(ptr){
        if(tmp == ptr){
            return tmp;
        }
        ptr = ptr->next;
    }
    return NULL;
}

// merge two free memory blocks into one
int merge(metaBlock* free){
    int isMerge = 0;
    // check the next memory block
    metaBlock *next;
    next = free->next;
    if((next && next->free)){
        free->size += next->size + METABLOCK_SIZE;
        free->next = next->next;
        if(next->next){
            next->next->prev = free;
        }
        isMerge = 1;
    }
    
    // check the previous memory block
    metaBlock * prev;
    prev = free->prev;
    if(prev && prev->free){
        prev->size += free->size + METABLOCK_SIZE;
        next = free->next;
        prev->next = next;
        if(next){
            next->prev = prev;
        }
        isMerge = 1;
    }
    return isMerge;
}

// initialize the availabe memory block
int init(){
    // get current program break
    void* programBreak = (void*)sbrk(0);
    // assign the program break to the head
    head = (metaBlock*)programBreak;
    // move the program break upwards (2GB)
    if(sbrk(MEMBLOCK_SIZE + METABLOCK_SIZE) == (void*)-1){
        printf("Unable to allocate memory!\n");
        return -1;
    }else{
        head->size = MEMBLOCK_SIZE;
        head->free = 1;
        head->next = NULL;
        printf("2MB memory block allocated!\n");
        return 0;
    }
}

void* _allocate(size_t size, metaBlock* block){
    // memory alignment
    size = memAlignment(size);
    if(head){
        // find the fitting block
        block = findBlock(size);
        if(block && block->size - size > METABLOCK_SIZE){
            // split the memory block
            splitMemBlock(block, size);
        }else if(block && block->size == size){
            block->free = 0;
            return (void*)((size_t)block + METABLOCK_SIZE);
        }else if(!block){
            // extend the heap
            block = extendBlock(size);
            if(!block){
                // fail to extend
                return NULL;
            }
        }
    }else{
        // extend the head
        block = extendBlock(size);
        if(!block){
            return NULL;
        }
        head = block;
    }
    return (void*)((size_t)block + METABLOCK_SIZE);
}

void* _malloc (size_t size){
    if(size == 0){
        return NULL;
    }
    // count the number of calls
    static first = 0;
    // the allocated memory
    metaBlock* block;
    // the first call
    if(!first){
        first++;
        // initialize
        if(!init()){
            return _allocate(size, block);
        }else{
            return NULL;
        }
    // the consequent call
    }else{
        first++;
        return _allocate(size, block);
    }
}

void*_free(void*ptr){
    if(!ptr){
        return NULL;
    }
    metaBlock* block;
    block = checkNode(ptr);
    
    if(checkAddress(ptr)){
        if(block){
            block->free = 1;
            if(block->next){
                int i = merge(block);
                if(i){
                    if(!block->next){
                        if(block->prev){
                            block->prev->next = NULL;
                        }
                        brk(block);
                    }
                    if(block->prev){
                        if(!block->prev->next && block->prev->free){
                            if(block->prev->prev){
                                block->prev->prev->next = NULL;
                            }
                            brk(block->prev);
                        }
                    }
                }
            }else{
                block->next = NULL;
                brk(block);
            }
        }else{
            return NULL;
        }
    }else{
        return NULL;
    }
}