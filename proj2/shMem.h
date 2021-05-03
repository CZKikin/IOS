#ifndef __SHMEM_H__
#define __SHMEM_H__
char * attachMemoryBlock(char* filename, int size);
char detachMemoryBlock(char* block);
char destroyMemoryBlock(char* filename);

#define BLOCK_SIZE 4096
#define FACTORY "main.c" //Shared memory has to be associated with an existing file
#define REINDEERS "shMem.c"
#define ELVES "shMem.h"
#endif 
