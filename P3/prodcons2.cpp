// Versión de los archivos de prácticas modificada por José Carlos Entrena
// Problema del productor-consumidor con 5 productores, 4 consumidores y un buffer. 
// Usando MPI.

#include "mpi.h"
#include <iostream>
#include "math.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#define Productor 4
#define Buffer  5
#define Consumidor 9
#define ITERSP 4
#define ITERSC 5
#define TAM 5

using namespace std;

void productor(int id){
int value;  
for (unsigned int i=0;i<ITERSP;i++){ 
  value=i;
  cout<< "Productor " << id << " produce valor "<<value<<endl<<flush;
  usleep(rand() % 1000 );
  MPI_Ssend( &value, 1, MPI_INT, Buffer, 0, MPI_COMM_WORLD );
 }
}


void buffer(){
 int value[TAM], peticion, pos=0,rama;
 MPI_Status status;
 int destino; 
 for (unsigned int i=0;i<ITERSP*10;i++){  
   if (pos==0) rama=0;        // El consumidor no puede consumir
   else if (pos==TAM) rama=1; // El productor no puede producir   
   else{  //Ambas guardas son ciertas
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_SOURCE<=Productor) rama=0; else rama=1;
   }
   switch(rama){
     case 0:
       MPI_Recv( &value[pos], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
       cout<< "Buffer recibe "<< value[pos] << " de Productor " << status.MPI_SOURCE << ". Tamaño: "<<pos+1<<endl<<flush;  
       pos++;
       break;
     case 1:
       MPI_Recv( &peticion, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
       destino = status.MPI_SOURCE; 
       MPI_Ssend( &value[pos-1], 1, MPI_INT, destino, 1, MPI_COMM_WORLD);
       cout<< "Buffer envía "<< value[pos-1] << " a Consumidor " << destino << ". Tamaño: "<<pos-1<<endl<<flush;  
       pos--;
       break;
   }     
 }
}   
   
     
void consumidor(int id){
 int value,peticion=1; float raiz;
 MPI_Status status;
 for (unsigned int i=0;i<ITERSC;i++){
  MPI_Ssend(&peticion, 1, MPI_INT, Buffer, 1, MPI_COMM_WORLD); 
  MPI_Recv(&value, 1,     MPI_INT, Buffer, 1, MPI_COMM_WORLD,&status );
  cout<< "Consumidor " << id << " recibe valor "<< value <<" de Buffer "<<endl<<flush;
  usleep(rand() % 1000 );
  raiz=sqrt(value);
 }
}


int main(int argc, char *argv[]) {
  int rank,size; 
  cout.flush();
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  /* Inicializa la semilla aleatoria */
  srand ( time(NULL) );
  if (size!=10) 
   {cout<< "El numero de procesos debe ser 10 "<<endl;return 0;} 
  if (rank <= Productor) productor(rank);
   else if (rank==Buffer) buffer();
    else consumidor(rank);
  MPI_Finalize( );
  return 0;
}

