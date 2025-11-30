//Librerias a usar
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Constantes
#define TAM_LINEA 16
#define NUM_FILAS 8 

//Estructura de la linea cache
typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

//Variables globales
int globaltime = 0;
int numFallos = 0;
unsigned char Simul_Ram[4096];
char leido[100];

//Prototipado de funciones
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

int main(){
	//Variables
   int numAciertos=0;
   float tMedio;
   T_CACHE_LINE Cache[NUM_FILAS];
   unsigned int addr;
   int ETQ;
   int palabra;
   int linea;
   int bloque;
   LimpiarCACHE(Cache);
   FILE *fr;									//Abrir al fichero de los contenidos de la RAM y guardarlo en Simul_Ram
   fr=fopen("CONTENTS_RAM.bin","rb");
   if(!fr){
      printf("Error abriendo CONTENTS_RAM.BIN");
      return -1;
   }
   fread(Simul_Ram,sizeof (unsigned char),4096,fr);
   fclose(fr);
   fr=fopen("accesos_memoria.txt","r");			//Abrir el fichero de las direcciones de memoria a las que tenemos que acceder
   if(!fr){
      printf("Error abriendo accesos_memoria.txt");
      return -1;
   }
   while((fscanf(fr, "%x", &addr))==1){									//Se lee la siguiente dirección de memoria a la que hay que acceder
      ParsearDireccion(addr, &ETQ,  &palabra, &linea, &bloque);			//Se llama a la función para que parsee la dirección de memoria 
      if(ETQ != Cache[linea].ETQ){										//Se comprueba que la etiqueta sea la misma sino es la misam da fallo y se hace el procedimiento de tratar fallo
         numFallos+=1;
         printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n",globaltime,numFallos,addr,ETQ,linea,palabra,bloque);
         globaltime+=20;
         TratarFallo(Cache, (char *) Simul_Ram, ETQ, linea, bloque);     
      }
      printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X \n", globaltime, addr, ETQ, linea, palabra, Cache[linea].Data[palabra]);
      leido[numAciertos++]=Cache[linea].Data[palabra];					//Se hace el procedimiento de acierto da igual si ha habido fallo o no
      for(int i=0; i< NUM_FILAS;i++){
         printf("%02X\tDatos: ",Cache[i].ETQ);
         for(int j=TAM_LINEA-1;j>=0;j--){
            printf("%02X ",Cache[i].Data[j]);
         }
         printf("\n");
      }
      sleep(1); 														//Se hace el sleep de 1s entre consultas de direcciones
   }
   fclose(fr);															//Se cierra fichero direcciones
   tMedio=(float)globaltime/(float)numAciertos;							//Calculamos tiempo de acceso medio casteando los ints a float para que el resultado sea float
   printf("Accesos totales: %d; fallos: %d; Tiempo medio: %f\n",numAciertos,numFallos,tMedio);
   printf("Texto leido: ");
   for(int i=0;i<numAciertos;i++){
      printf("%c",leido[i]);
   }
   VolcarCACHE(Cache);  												//Volcamos cache en fichero
   return 0;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){							//Inicializamos la cache
   for (int i = 0; i<NUM_FILAS; i++){
      tbl[i].ETQ = 0xFF; 
      for(int j = 0; j < TAM_LINEA; j++){
         tbl[i].Data[j] = 0x23;
      }
   }
}

void VolcarCACHE(T_CACHE_LINE *tbl){									//Volcado datos cache, se vuelva en contents_cache.bin
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
   //Se parsean la direccion de memoria mediante desplazamiento de bits y máscaras para que la palabra sean los 4 bits más a la derecha, luego los 3 bits de la linea y los últimos 5 de la etiqueta
   *palabra = addr & 0x00F;
   *linea = (addr>>4) & 0x007;
   *ETQ = addr>>7;
   *bloque = addr>>4;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
   tbl[linea].ETQ=ETQ;						//Lo primero que e hace cuando se trata un fallo se pone la etiqueta a la de la linea para que sean iguales
   for(int i=0;i<TAM_LINEA;i++){			//Buscamos el bloque correspondiente en los datos de la ram que tenemos de antes y escribimos los datos en la caché
	   tbl[linea].Data[i]=MRAM[bloque*TAM_LINEA+i];   
   }
   printf("Cargando el bloque %02X en la linea %02X\n",bloque,linea);		//Printeamos que se escribe el bloque en la linea correspondiente
}
