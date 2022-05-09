#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

#include "vodik.h"
#include "kyslik.h"

#define TILIMIT (1000)
#define TBLIMIT (1000)
#define SOUBOR "proj2.out"

#define errorMSG(msg, returnCode) {fprintf(stderr, "%s:%d -> %s\n", __FILE__, __LINE__, (msg)); exit((returnCode));}

typedef struct {
    char **semaphores;
    unsigned len;
} semArray;

void semArrayInit(semArray *arr) {
    arr->semaphores = NULL;
    arr->len = 0;
}

int createSemaphore(const char *name, semArray *arr, unsigned state){
    sem_unlink(name);
    sem_t *semaphore = sem_open(name, O_CREAT, 0660, state);
    if (semaphore == SEM_FAILED)
        return 1;
    arr->len++;
    arr->semaphores = realloc(arr->semaphores, sizeof(char*) * arr->len);
    if (arr->semaphores == NULL)    return 1;

    char *placeForName = malloc(sizeof(char) * strlen(name) + 1 );
    if (placeForName == NULL)  return 1;

    strncpy(placeForName, name, strlen(name));
    arr->semaphores[arr->len-1] = placeForName;

    return 0;
}

void closeAllSemaphores(semArray *arr){
    for(unsigned i=0; i<arr->len; i++){
        sem_unlink(arr->semaphores[i]);
        free(arr->semaphores[i]);
    }
    free(arr->semaphores);
    arr->semaphores = NULL;
}

int parseValues(int* NO, int* NH, int* TI, int* TB, char** argv){
    *NO = atoi(argv[1]);
    *NH = atoi(argv[2]);
    *TI = atoi(argv[3]);
    *TB = atoi(argv[4]);

    if (*NO < 0)    return 1;
    if (*NH < 0)    return 1;
    if (*TI < 0 || *TI > TILIMIT)   return 1;
    if (*TB < 0 || *TB > TBLIMIT)   return 1;

    return 0;
}

char createNeededSemaphores(semArray *array) {
    if ( createSemaphore("writeHydrogens", array, 1)!=0) return 1;
    if ( createSemaphore("writeOxygens", array, 1)!=0) return 1;
    if ( createSemaphore("writeLastHydro", array, 1)!=0) return 1;
    if ( createSemaphore("writeLastOxy", array, 1)!=0) return 1;
    if ( createSemaphore("writeMolecules", array, 1)!=0) return 1;
    if ( createSemaphore("writeActions", array, 1)!=0) return 1;
    if ( createSemaphore("lastHydro", array,  1)!=0) return 1;
    if ( createSemaphore("lastOxy", array, 1)!=0) return 1;
    if ( createSemaphore("creatingMolecule", array, 1)!=0) return 1;
    if ( createSemaphore("oxygen", array, 1)!=0) return 1;


    if ( createSemaphore("hydroKill", array, 0)!=0) return 1;
    if ( createSemaphore("hydrogen", array, 0)!=0) return 1;
    if ( createSemaphore("molDone", array, 0)!=0) return 1;
    return 0;
}
void unmapAndFree(uint16_t **ptrs){
    for (int i = 0; i < 7; ++i) {
        munmap(ptrs[i], sizeof(uint16_t));
    }
    free(ptrs);
}

int main(int argc, char** argv) {
    char errCode = 1;
    semArray array;
    semArrayInit(&array);
    if (argc != 5) errorMSG("./proj2 NO NH TI TB\nWrong number of arguments!", 1);

    int NO, NH, TI, TB;
    errCode = parseValues(&NO, &NH, &TI, &TB, argv);
    if (errCode != 0) errorMSG("Couldn't parse values\n", 1)

        errCode = createNeededSemaphores(&array);
    if (errCode != 0) errorMSG("Semaphore creation failed\n", 1)

    uint16_t **ptrsToSharedMem = malloc(7*sizeof(uint16_t*));
    for (int i = 0; i < 7; ++i) {
        ptrsToSharedMem[i] = mmap(NULL, sizeof(uint16_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        /* hydrogensDone | oxygensDone | lastHydrogenFlag | lastOxygenFlag | moleculeNumber | hydrogenBinder | action count*/
    }

    if (NO == 0)    *ptrsToSharedMem[3] = 1;
    if (NH == 0)    *ptrsToSharedMem[2] = 1;
    *ptrsToSharedMem[6] = 1;
    *ptrsToSharedMem[4] = 1;

    FILE *filePtr = fopen(SOUBOR, "w");
    if (filePtr == NULL)    return 1;
    pid_t childPid;
    if ((childPid = fork()) == 0) {
        for (int i = 1; i <= NO; ++i) {
           createOxygen(i, TI, TB, NO, ptrsToSharedMem, filePtr);
        }

        for (int i = 1; i <= NH; ++i) {
            createHydrogen(i, TI, NH, ptrsToSharedMem, filePtr);
        }

        while(wait(NULL)>0);
        exit(0);
    }
    while(wait(NULL)>0);
    fclose(filePtr);
    closeAllSemaphores(&array);
    unmapAndFree(ptrsToSharedMem);
    return errCode;
}
