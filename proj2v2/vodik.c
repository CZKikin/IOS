// // Created by kikin on 07.04.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "vodik.h"


void checkIfOxygenLeft(sem_t *wa, sem_t *lo, sem_t *wo, sem_t *lh, uint16_t **ptrsToSharedMem, unsigned index, FILE *filePtr){
    sem_wait(lo);
    sem_wait(wo);
    sem_wait(lh);
    sem_wait(wa);
    if(*ptrsToSharedMem[2] == 1 && *ptrsToSharedMem[1] < 1 && *ptrsToSharedMem[3] == 1){
        fprintf(filePtr, "%d: H %d: not enough O or H\n", (*ptrsToSharedMem[6]), index);
        (*ptrsToSharedMem[6])++;
        (*ptrsToSharedMem[0])--;
        sem_post(lo);
        sem_post(wo);
        sem_post(wa);
        sem_post(lh);
        exit(0);
    }
    sem_post(lo);
    sem_post(wa);
    sem_post(lh);
    sem_post(wo);
}

void createHydrogen(unsigned index, unsigned TI, unsigned max, uint16_t **ptrsToSharedMem, FILE* filePtr){
    int id = fork();
    if (id != 0)    return;
    int enough = 0;
    sem_t *lh = sem_open("lastHydro", 0);
    sem_t *lo = sem_open("lastOxy", 0);
    sem_t *wo = sem_open("writeOxygens", 0);
    sem_t *wh = sem_open("writeHydrogens", 0);
    sem_t *wm = sem_open("writeMolecules", 0);
    sem_t *wa = sem_open("writeActions", 0);
    sem_t *hk = sem_open("hydroKill", 0);

    sem_t *oxygens= sem_open("oxygen", 0);
    sem_t *hydrogen= sem_open("hydrogen", 0);
    sem_t *molDone= sem_open("molDone", 0);

    sem_wait(wa);
    fprintf(filePtr, "%d: H %d: started\n", (*ptrsToSharedMem[6]), index);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);
    usleep(rand()%TI);

    sem_wait(wa);
    fprintf(filePtr, "%d: H %d: going to queue\n", (*ptrsToSharedMem[6]), index);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);

    sem_wait(wh);
    (*ptrsToSharedMem[0])++;
    sem_post(wh);

    if (index == max){
        sem_wait(lh);
        *ptrsToSharedMem[2] = 1;
        sem_post(lh);
    }

    while(!enough) {
        checkIfOxygenLeft(wa, lo, wo, lh, ptrsToSharedMem, index, filePtr);
        sem_wait(oxygens);
        if(*ptrsToSharedMem[5] > 0){
            enough = 1;
            (*ptrsToSharedMem[5])--;
        }
        sem_post(oxygens);
    }

    sem_post(hydrogen);
    sem_wait(wm);

    sem_wait(wa);
    fprintf(filePtr, "%d: H %d: creating molecule %d\n", (*ptrsToSharedMem[6]), index, *ptrsToSharedMem[4]);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);

    sem_post(wm);

    sem_wait(molDone);

    sem_wait(wm);
    sem_wait(wa);
    fprintf(filePtr, "%d: H %d: molecule %d created\n", (*ptrsToSharedMem[6]), index, *ptrsToSharedMem[4]);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);
    sem_post(wm);

    sem_post(hk);
    exit(0);
}
