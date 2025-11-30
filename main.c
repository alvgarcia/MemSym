#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
   int numAciertos=0;
   unsigned char Simul_Ram[4096];
   char leido[100];
   T_CACHE_LINE Cache[NUM_FILAS];
   unsigned int addr;
   int ETQ;
   int palabra;
   int linea;
   int bloque;
   LimpiarCACHE(Cache);
   FILE *fr;
   fr=fopen("CONTENTS_RAM.bin","rb");
   if(!fr){
      printf("Error abriendo CONTENTS_RAM.BIN");
      return -1;
   }
   fgets(Simul_Ram,4096,fr);
   fclose(fr);
   fr=fopen("accesos_memoria.txt","r");
   if(!fr){
      printf("Error abriendo accesos_memoria.txt");
      return -1;
   }
   while((fscanf(fr, "%x", &addr))==1){
      ParsearDireccion(addr, &ETQ,  &palabra, &linea, &bloque);
      if(ETQ != Cache[linea].ETQ){
         numFallos+=1;
         printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n",globaltime,numFallos,addr,ETQ,linea,palabra,bloque);
         globaltime+=20;
         TratarFallo(Cache, (char *) Simul_Ram, ETQ, linea, bloque);          
      }
      printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X \n", globaltime, addr, ETQ, linea, palabra, Cache[linea].Data[palabra]);
      leido[numAciertos++]=Cache[linea].Data[palabra];
      for(int i=0; i< NUM_FILAS;i++){
         printf("%02X\tDatos: ",Cache[i].ETQ);
         for(int j=TAM_LINEA-1;j>=0;j--){
            printf("%02X ",Cache[i].Data[j]);
         }
         printf("\n");
      }
      sleep(1);
   }
   printf("Accesos totales: %d; fallos: %d; Tiempo medio: %f\n",numAciertos,numFallos,(numFallos/numAciertos)*20);
   leido[numAciertos]='\0';
   printf("Texto leido: ");
   for(int i=0;i<numAciertos;i++){
      printf("%c",leido[i]);
   }
   VolcarCACHE(Cache);
   return 0;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
   for (int i = 0; i<NUM_FILAS; i++){
      tbl[i].ETQ = 0xFF; 
      for(int j = 0; j < TAM_LINEA; j++){
         tbl[i].Data[j] = 0x23;
      }
   }
}

void VolcarCACHE(T_CACHE_LINE *tbl){
   FILE *fd;
   fd=fopen("CONTENTS_CACHE.bin","wb");
   for (int i = 0; i<NUM_FILAS; i++){
      for(int j = 0; j < TAM_LINEA; j++){
         putc(tbl[i].Data[j],fd);
      }
      putc('\n',fd);
   }
   fclose(fd);
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
   *palabra = addr & 0x00F;
   *linea = (addr>>4) & 0x007;
   *ETQ = addr>>7;
   *bloque = addr>>4;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
   tbl[linea].ETQ=ETQ;
   memcpy(tbl[linea].Data, MRAM + (bloque*TAM_LINEA), TAM_LINEA);
   printf("Cargando el bloque %02X en la linea %02X\n",bloque,linea);
}