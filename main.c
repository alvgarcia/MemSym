#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINEA 16
#define NUM_FILAS 8 


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
   LimpiarCACHE(Cache);


   return 0;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
   for (int i = 0; i<NUM_FILAS -1 ; i++){
      tbl[i].ETQ = 0xFF; 
      for(int j = 0; j < TAM_LINEA - 1; j++){
         tbl[i].Data[j] = 0x23;
      }
   }
}

void VolcarCACHE(T_CACHE_LINE *tbl){
   FILE *fd;
   fd=fopen("CONTENTS_CACHE.bin","wb");
   for (int i = 0; i<NUM_FILAS -1 ; i++){
      for(int j = 0; j < TAM_LINEA - 1; j++){
         putc(tbl[i].Data[j],fd);
      }
      putc("\n",fd);
   }
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){

}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

}