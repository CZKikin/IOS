#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#include "shMem.h"

#define ELF_LIM (1000)
#define REINDEER_LIM (20)
#define errorMSG(msg, returnCode) {fprintf(stderr, "%s:%d -> %s\n", __FILE__, __LINE__, (msg)); exit((returnCode));}

void createReindeer(int rid, int TR){
    int id = fork();
    if (id != 0)    return;

    char* reindeerQueue= attachMemoryBlock(REINDEERS, BLOCK_SIZE); 
    if (reindeerQueue == NULL) errorMSG("Failed to share memory!\n", 1);

    sem_t *reiSem = sem_open("reindeer", 0);
    sem_t *wtr = sem_open("writingToReindeer", 0);
    sem_t *ready = sem_open("reindeersHiched", 0);
    if (reiSem == SEM_FAILED)
        errorMSG("Opening of the semaphres failed", 1);

    printf("%-10s %d: Rstarted\n", "A: RD", rid);
    usleep((rand() % (TR - TR/2 + 1)) + TR/2);
    printf("%-10s %d: Return home\n", "A: RD", rid);

    sem_wait(wtr);
    snprintf(reindeerQueue, BLOCK_SIZE, "%d", atoi(reindeerQueue)+1);
    sem_post(wtr);
    
    sem_wait(reiSem);
    printf("%-10s %d: Gets hitched\n", "A: RD", rid);

    sem_wait(wtr);
    snprintf(reindeerQueue, BLOCK_SIZE, "%d", atoi(reindeerQueue)-1);
    if (atoi(reindeerQueue) == 0)
        sem_post(ready);
    sem_post(wtr);
    exit(0);
}

void createElf(int eid, int TE){

    int id = fork();
    if (id != 0)    return;

    sem_t *elfSem = sem_open("elf", 0);
    sem_t *santaHelping = sem_open("santaHelping", 0);

    sem_t *wtf = sem_open("writingToFactory", 0);
    sem_t *wte = sem_open("writingToElf", 0);

    printf("%-10s %d: Started\n", "A: Elf", eid);
    usleep(rand()%TE);
    printf("%-10s %d: Needs help\n", "A: Elf", eid);

    char* sharedMemory = attachMemoryBlock(FACTORY, BLOCK_SIZE);
    if (sharedMemory == NULL) errorMSG("FAILED TO GET SHARED MEMORY!\n", 1);

    char* elfMem = attachMemoryBlock(ELVES, BLOCK_SIZE);
    if (elfMem == NULL) errorMSG("Failed to share memory!\n", 1);


    sem_wait(wtf);
    if (atoi(sharedMemory) == 1){
        printf("%-10s %d: Taking holidays\n", "A: Elf", eid);
        sem_post(wtf);
        exit(0);
    }
    sem_post(wtf);

    sem_wait(wte);
    snprintf(elfMem, BLOCK_SIZE, "%d", atoi(elfMem)+1);
    sem_post(wte);

    sem_wait(elfSem);
    sem_wait(wtf);
    if (atoi(sharedMemory) == 1){
        printf("%-10s %d: Taking holidays\n", "A: Elf", eid);
        sem_post(wtf);
        exit(0);
    }
    sem_post(wtf);
    printf("%-10s %d: Getting help\n", "A: Elf", eid);
    sem_post(santaHelping);
    exit(0);
}

void createSanta(int reindeers){
    int id = fork();
    if (id != 0)    return;

    sem_t *elfSem = sem_open("elf", 0);
    sem_t *reiSem = sem_open("reindeer", 0);
    sem_t *santaSem = sem_open("santa", 0);
    sem_t *santaHelping = sem_open("santaHelping", 0);
    sem_t *reindeersReady = sem_open("reindeersHiched", 0);

    sem_t *wtr = sem_open("writingToReindeer", 0);
    sem_t *wte = sem_open("writingToElf", 0);
    sem_t *wtf = sem_open("writingToFactory", 0);

    if (elfSem == SEM_FAILED || reiSem == SEM_FAILED || santaSem == SEM_FAILED)
        errorMSG("Opening of the semaphres failed", 1);

    char santaCycle = 1;
    char helpElves = 0;

    char* elfMem = attachMemoryBlock(ELVES, BLOCK_SIZE);
    if (elfMem == NULL) errorMSG("Failed to share memory!\n", 1);

    char* reindeerQueue= attachMemoryBlock(REINDEERS, BLOCK_SIZE); 
    if (reindeerQueue == NULL) errorMSG("Failed to share memory!\n", 1);

    char* factory = attachMemoryBlock(FACTORY, BLOCK_SIZE); 
    if (reindeerQueue == NULL) errorMSG("Failed to share memory!\n", 1);


    printf("A: Santa: Going to sleep\n");
    while (santaCycle){
        sem_wait(wte);
        if (atoi(elfMem) >= 3){
            snprintf(elfMem, BLOCK_SIZE, "%d", atoi(elfMem)-3);
            helpElves = 1;
        }
        sem_post(wte);

        if (helpElves){
            helpElves = 0;
            printf("%-10s Helping elves \n", "A: Santa:");
            for (int i = 0; i<3; i++){ //pustí 3 elfíky
                sem_post(elfSem);
                sem_wait(santaHelping);
            }
            printf("%-10s Going to sleep \n", "A: Santa:");
        }

        sem_wait(wtr);
        if (atoi(reindeerQueue) == reindeers) // reindeers ready
            santaCycle = 0;
        sem_post(wtr);
    }

    printf("%-10s Closing workshop\n", "A: Santa:");
    sem_wait(wtf);
    strncpy(factory, "1", BLOCK_SIZE);
    sem_post(wtf);

    sem_wait(wte);
    for(helpElves = 0; helpElves < atoi(elfMem); helpElves++)
        sem_post(elfSem);
    sem_post(wte);

    for (int i = 0; i<=reindeers; i++)
        sem_post(reiSem);
     
    sem_wait(reindeersReady);
    printf("%-10s Christmass started\n", "A: Santa:");
    sem_post(santaSem);
    exit(0);
}

int clearSharedMemBlocks(){
    char* fac = attachMemoryBlock(FACTORY, BLOCK_SIZE);
    char* rei = attachMemoryBlock(REINDEERS, BLOCK_SIZE);
    char* elf = attachMemoryBlock(ELVES, BLOCK_SIZE);
    if (fac == NULL || rei == NULL || elf == NULL)
        return 1;

    strncpy(fac, "0", BLOCK_SIZE);
    strncpy(rei, "0", BLOCK_SIZE);
    strncpy(elf, "0", BLOCK_SIZE);
    
    return 0;
}

int unlinkAndCreateSemaphores(){
    sem_unlink("reindeer");
    sem_unlink("elf");
    sem_unlink("santa");
    sem_unlink("santaHelping");
    sem_unlink("reindeersHiched");

    sem_unlink("writingToReindeer");
    sem_unlink("writingToElf");
    sem_unlink("writingToFactory");

    sem_t *elfSem = sem_open("elf", O_CREAT, 0660, 0);
    sem_t *reiSem = sem_open("reindeer", O_CREAT, 0660, 0);
    sem_t *reindeersHiched = sem_open("reindeersHiched", O_CREAT, 0660, 0);
    sem_t *santaSem = sem_open("santa", O_CREAT, 0660, 0);
    sem_t *santaHelping = sem_open("santaHelping", O_CREAT, 0660, 0);

    sem_t *writingToReiSem = sem_open("writingToReindeer", O_CREAT, 0660, 1);
    sem_t *writingToElfSem= sem_open("writingToElf", O_CREAT, 0660, 1);
    sem_t *writingToFacSem= sem_open("writingToFactory", O_CREAT, 0660, 1);

    if (elfSem == SEM_FAILED || reiSem == SEM_FAILED 
            || santaSem == SEM_FAILED || writingToReiSem == SEM_FAILED
            || writingToElfSem == SEM_FAILED || writingToFacSem == SEM_FAILED
            || santaHelping == SEM_FAILED || reindeersHiched == SEM_FAILED)
        return 1;
    return 0;
}

int parseValues(int* NE, int* NR, int* TE, int* TR, char** argv){
    *NE = atoi(argv[1]);
    *NR = atoi(argv[2]);
    *TE = atoi(argv[3]);
    *TR = atoi(argv[4]);
 
    if (*NE < 0 || *NE > ELF_LIM)
        return 1;
    if (*NR < 0 || *NR > REINDEER_LIM)
        return 1;
    if (*TE < 0)
        return 1;
    if (*TR < 0)
        return 1;

    return 0;
}

int main(int argc, char** argv){
    if (argc != 5) errorMSG("./proj2 NE NR TE TR\nThis program needs exactly these arguments", 1);

    int NE, NR, TE, TR;
    parseValues(&NE, &NR, &TE, &TR, argv);
       if (clearSharedMemBlocks() != 0)
        errorMSG("Failed to clear shared memory", 1);

    if (unlinkAndCreateSemaphores() != 0)
        errorMSG("Failed to create semaphores", 1);



    pid_t childPid;
    if ((childPid = fork()) == 0) {
        createSanta(NR);
        for (int i = 1; i<=NE; i++)
            createElf(i, TE);

        for (int i = 1; i<=NR; i++){
            createReindeer(i, TR);
        }

        sem_t *santaSem = sem_open("santa", 0);

        sem_wait(santaSem);
        wait(0);//wait for elves
        exit(0);
    }

    wait(NULL);
    if(!destroyMemoryBlock(FACTORY) || !destroyMemoryBlock(REINDEERS) || !destroyMemoryBlock(ELVES))
        exit(1);
    return 0;
}

