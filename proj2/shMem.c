#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include "shMem.h" 

static int getSharedBlock(char *filename, int size) { 
    key_t key; 
    key = ftok(filename, 0); 
    if (key == -1) return -1;
    
    return shmget(key, size, 0644 | IPC_CREAT); //get shared block || create if it does not exist
}

char* attachMemoryBlock(char *filename, int size) {
    int sharedBlockId = getSharedBlock(filename, size);
    char *result;

    if (sharedBlockId == -1) return NULL; 

    result = shmat(sharedBlockId, NULL, 0);
    if (result == (char *) -1) return NULL;

    return result;
}

char detachMemoryBlock(char *block){
    return (shmdt(block) == 0);
}

char destroyMemoryBlock(char *filename){
    int sharedBlockId = getSharedBlock(filename, 0);
    if (sharedBlockId == -1) return 0;

    return (shmctl(sharedBlockId, IPC_RMID, NULL) == 0);
}
