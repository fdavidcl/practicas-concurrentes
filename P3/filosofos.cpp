// Versión de los archivos de prácticas modificado por José Carlos Entrena
// Problema de los filósofos sin interbloqueo. 

#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor (int id, int nprocesos);

int main(int argc,char** argv ){
 int rank,size;
 srand(time(0));
 MPI_Init( &argc, &argv );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );
 MPI_Comm_size( MPI_COMM_WORLD, &size );
 if( size!=10){
   if(rank == 0) 
      cout<<"El numero de procesos debe ser 10"<<endl<<flush;
   MPI_Finalize( ); return 0; 
 }
 if ((rank%2) == 0)  
      Filosofo(rank,size); // Los pares son Filosofos 
 else Tenedor(rank,size);  // Los impares son Tenedores 
 MPI_Finalize( );
 return 0;
}  

void Filosofo(int id, int nprocesos){
 int a = 0; 
 MPI_Status status; 
 int izq=(id+1)%nprocesos;
 int der=(id-1+nprocesos)%nprocesos;  
 // Tomaremos los tenedores por orden izquierdo-derecho, salvo en el primer filósofo, que será derecho-izquierdo. 
 // Hacemos esto para evitar que todos los filósofos tengan su tenedor izquierdo y no puedan acceder al derecho, provocando bloqueo. 
 int primero = izq; 
 int segundo = der; 
 if (id == 0){
   primero = der; 
   segundo = izq; 
 }
 while(1){
  //Solicita primer tenedor
  cout<<"Filosofo "<<id<< " solicita primer tenedor ..."<< primero <<endl<<flush;
  
  MPI_Ssend(&a, 1, MPI_INT, primero, 0, MPI_COMM_WORLD); 
  MPI_Recv(&a, 1, MPI_INT, primero, 0, MPI_COMM_WORLD,&status );

  //Solicita segundo tenedor
  cout<<"Filosofo "<<id<< " solicita segundo tenedor ..."<< segundo <<endl<<flush;
  
  MPI_Ssend(&a, 1, MPI_INT, segundo, 0, MPI_COMM_WORLD); 
  MPI_Recv(&a, 1, MPI_INT, segundo, 0, MPI_COMM_WORLD,&status );

  cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
  usleep(rand() % 500000);  //comiendo
  //Suelta el tenedor izquierdo
  cout<<"Filosofo "<<id<< " suelta tenedor izquierdo ..."<<izq <<endl<<flush;
  
  MPI_Ssend(&a, 1, MPI_INT, izq, 0, MPI_COMM_WORLD); 

  //suelta el tenedor derecho
  cout<<"Filosofo "<<id<< " suelta tenedor derecho ..."<<der <<endl<<flush;
 
  MPI_Ssend(&a, 1, MPI_INT, der, 0, MPI_COMM_WORLD); 

  cout<<"Filosofo "<<id<< " PENSANDO"<<endl<<flush;
  usleep((rand()%500000) );//pensando
 }
}

void Tenedor(int id, int nprocesos){
  int buf; MPI_Status status; int Filo;
  while(1){
    // Espera un peticion desde cualquier filosofo vecino
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,&status );
    Filo = status.MPI_SOURCE; 
    // Recibe la peticion del filosofo
    MPI_Ssend(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD);
    cout<<"Tenedor "<<id<<" recibe petición de "<<Filo<<endl<<flush;
    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD,&status );
    cout<<"Tenedor "<<id<<" recibe liberación de "<<Filo<<endl<<flush; 
  }
}
