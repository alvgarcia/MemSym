#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINEA 16


typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

int main(){
    int globaltime = 0;
    int numFallos = 0;
    unsigned char Simul_Ram[4096];
    T_CACHE_LINE Cache[NUM_FILAS];
    



    return 0;
}