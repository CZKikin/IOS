//
// Created by kikin on 07.04.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "kyslik.h"



void checkIfHydrogenLeft(sem_t *wa,  sem_t *lo, sem_t *wo, sem_t *lh, uint16_t **ptrsToSharedMem, unsigned index, FILE* filePtr){
    sem_wait(lo);
    sem_wait(wo);
    sem_wait(wa);
    sem_wait(lh);
    if(*ptrsToSharedMem[2] == 1 && *ptrsToSharedMem[0] < 2 && *ptrsToSharedMem[3] == 1){
        fprintf(filePtr, "%d: O %d: not enough O or H\n", (*ptrsToSharedMem[6]), index);
        (*ptrsToSharedMem[6])++;
        (*ptrsToSharedMem[1])--;
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

void createOxygen(unsigned index, unsigned TI, unsigned TB,  unsigned max, uint16_t **ptrsToSharedMem, FILE *filePtr){
    int id = fork();
    if (id != 0)    return;
    int enough = 0;
    sem_t *lh = sem_open("lastHydro", 0);
    sem_t *lo = sem_open("lastOxy", 0);
    sem_t *wo = sem_open("writeOxygens", 0);
    sem_t *wh = sem_open("writeHydrogens", 0);
    sem_t *wm = sem_open("writeMolecules", 0);
    sem_t *wa = sem_open("writeActions", 0);
    sem_t *cm = sem_open("creatingMolecule", 0);
    sem_t *hk = sem_open("hydroKill", 0);

    sem_t *oxygens= sem_open("oxygen", 0);
    sem_t *hydrogen= sem_open("hydrogen", 0);
    sem_t *molDone= sem_open("molDone", 0);

    sem_wait(wa);
    fprintf(filePtr, "%d: O %d: started\n", (*ptrsToSharedMem[6]), index);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);

    usleep(rand()%TI);
    sem_wait(wo);
    (*ptrsToSharedMem[1])++;
    sem_post(wo);

    if (index == max){
        sem_wait(lo);
        *ptrsToSharedMem[3] = 1;
        sem_post(lo);
    }


    checkIfHydrogenLeft(wa, lo, wo, lh, ptrsToSharedMem, index, filePtr);


    sem_wait(wa);
    fprintf(filePtr, "%d: O %d: going to queue\n", (*ptrsToSharedMem[6]), index);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);

    while(!enough) {
        checkIfHydrogenLeft(wa, lo, wo, lh, ptrsToSharedMem, index, filePtr);
        sem_wait(wh);
        if(*ptrsToSharedMem[0] >= 2){
            enough = 1;
            (*ptrsToSharedMem[0])-=2;
        }
        sem_post(wh);
    }
    sem_wait(oxygens);
    (*ptrsToSharedMem[5])+=2;
    sem_post(oxygens);

    sem_wait(hydrogen);
    sem_wait(hydrogen);

    sem_wait(cm);
    sem_wait(wo);
    (*ptrsToSharedMem[1])--;
    sem_post(wo);

    sem_wait(wm);
    sem_wait(wa);
    fprintf(filePtr, "%d: O %d: creating molecule %d\n", (*ptrsToSharedMem[6]), index, *ptrsToSharedMem[4]);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);
    sem_post(wm);

    usleep(rand()%TB);
    sem_post(molDone);
    sem_post(molDone);

    sem_wait(wm);
    sem_wait(wa);
    fprintf(filePtr, "%d: O %d: molecule %d created\n", (*ptrsToSharedMem[6]), index, *ptrsToSharedMem[4]);
    (*ptrsToSharedMem[6])++;
    sem_post(wa);
    sem_post(wm);

    sem_wait(hk);
    sem_wait(hk);

    sem_wait(wm);
    (*ptrsToSharedMem[4])++;
    sem_post(wm);
    sem_post(cm);

    exit(0);
}

