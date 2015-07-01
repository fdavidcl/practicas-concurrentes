// Versión de los archivos de pŕacticas modificada por José Carlos Entrena. 
// Problema de los filósofos con camarero. 

#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXFILOSOFOS 4 
#define CAMARERO 10 

using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor (int id, int nprocesos); 
void Camarero(int id, int nprocesos); 

int main(int argc,char** argv ){
 int rank,size;
 srand(time(0));
 MPI_Init( &argc, &argv );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );
 MPI_Comm_size( MPI_COMM_WORLD, &size );
 if( size!=11){
   if(rank == 0) 
      cout<<"El numero de procesos debe ser 11"<<endl<<flush;
   MPI_Finalize( ); return 0; 
 }
 if (rank==10) 
      Camarero(rank, size); // El 10 es el camarero
 else if ((rank%2) == 0	)  
      Filosofo(rank,size); // Los pares son filósofos 
 else Tenedor(rank,size);  // Los impares son tenedores 
 MPI_Finalize( );
 return 0;
}  

void Filosofo(int id, int nprocesos){
 int a = 0; 
 MPI_Status status; 
 int izq=id+1; 
 int der=(id-2+nprocesos)%(nprocesos-1);   
 while(1){
  // Sentarse
    cout << "Filósofo " << id << " solicita sentarse." << endl << flush; 
    MPI_Ssend(&a, 1, MPI_INT, CAMARERO, 0, MPI_COMM_WORLD); 
    MPI_Recv(&a, 1, MPI_INT, CAMARERO, 0, MPI_COMM_WORLD, &status); 

  //Solicita tenedor izquierda
  cout<<"Filosofo "<<id<< " solicita tenedor izquierdo ..."<<izq <<endl<<flush;
  MPI_Ssend(&a, 1, MPI_INT, izq, 0, MPI_COMM_WORLD); 
  MPI_Recv(&a, 1, MPI_INT, izq, 0, MPI_COMM_WORLD,&status );

  //Solicita tenedor derecho
  cout<<"Filosofo "<<id<< " solicita tenedor derecho ..."<<der <<endl<<flush;
  MPI_Ssend(&a, 1, MPI_INT, der, 0, MPI_COMM_WORLD); 
  MPI_Recv(&a, 1, MPI_INT, der, 0, MPI_COMM_WORLD,&status );

  // Comer
  cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
  usleep(rand() % 500000); 

  //Suelta el tenedor izquierdo
  cout<<"Filosofo "<<id<< " suelta tenedor izquierdo ..."<<izq <<endl<<flush;
  MPI_Ssend(&a, 1, MPI_INT, izq, 0, MPI_COMM_WORLD); 

  //suelta el tenedor derecho
  cout<<"Filosofo "<<id<< " suelta tenedor derecho ..."<<der <<endl<<flush;
  MPI_Ssend(&a, 1, MPI_INT, der, 0, MPI_COMM_WORLD); 
  cout<<"Filosofo "<<id<< " PENSANDO"<<endl<<flush;

  // Levantarse 
    cout << "Filósofo " << id << " se levanta." << endl << flush; 
    MPI_Ssend(&a, 1, MPI_INT, CAMARERO, 1, MPI_COMM_WORLD); 
    MPI_Recv(&a, 1, MPI_INT, CAMARERO, 1, MPI_COMM_WORLD, &status); 


  // Pensar
  usleep((rand()%500000) );
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

void Camarero(int id, int nprocesos){
    int nfil = 0; 
    int a = 0; 
    int tipo; 
    MPI_Status status; 
    while(true){
        // Si aun nos queda espacio esperamos liberación (tag 1) o petición (tag 0).
        if (nfil < MAXFILOSOFOS){
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            tipo = status.MPI_TAG; 
            if (tipo == 0){                 // Petición
                MPI_Ssend(&a, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD); 
                nfil++; 
                cout << "El filósofo " << status.MPI_SOURCE << " puede sentarse." << endl << flush; 
                cout << "Filósofos en la mesa: " << nfil << endl << flush; 
            }
            else{                           // Liberación
                MPI_Ssend(&a, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD); 
                nfil--; 
                cout << "El filósofo " << status.MPI_SOURCE << " se levanta." << endl << flush; 
                cout << "Filósofos en la mesa: " << nfil << endl << flush; 
            }
        }
        // Si la mesa está llena solo recibimos liberaciones. 
        else{                  
            MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); 
            MPI_Ssend(&a, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD); 
            nfil--; 
            cout << "El filósofo " << status.MPI_SOURCE << " se levanta." << endl << flush; 
            cout << "Filósofos en la mesa: " << nfil << endl << flush; 
        }
    }             
}




