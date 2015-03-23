// José Carlos Entrena

#include <time.h>
#include <unistd.h> // incluye ”usleep(...)”
#include <stdlib.h> // incluye ”rand()” y ”srand”
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std; 

/* 
    Identificaremos los elementos para fumar como: 1: Tabaco. 2: Cerillas. 3: Papel. El 0 simboliza el mostrador vacío.
*/

static int elem_mostrador = 0; 

sem_t tabaco; 
sem_t cerillas; 
sem_t papel; 
sem_t mostrador; 


// Función que simula la acción de fumar 
// como un retardo aleatorio de la hebra entre 0'1 y 2 segundos. 

void fumar(){
    const unsigned miliseg = 100U + (rand() % 1900U) ;
    usleep( 1000U*miliseg ); // Retraso bloqueado durante miliseg 
}


void * fumador_sin_tabaco(void *){
    while (true){
        sem_wait(&tabaco); 
	elem_mostrador = 0; 
        sem_post(&mostrador); 
        
        cout << "Soy el fumador 1, he conseguido tabaco y estoy fumando.\n"; 
        fumar(); 
    }    
}

void * fumador_sin_cerillas(void *){
    while (true){
        sem_wait(&cerillas); 
	elem_mostrador = 0; 
        sem_post(&mostrador); 
        
        cout << "Soy el fumador 2, he conseguido cerillas y estoy fumando.\n"; 
        fumar(); 
    }  
}

void * fumador_sin_papel(void *){
    while (true){
        sem_wait(&papel); 
	elem_mostrador = 0; 
        sem_post(&mostrador); 
        
        cout << "Soy el fumador 3, he conseguido papel y estoy fumando.\n"; 
        fumar(); 
    }  
}

void * estanquero(void *){

    while (true){
        if (elem_mostrador == 0){
            sem_wait(&mostrador); 
            int nuevo = rand() % 3 + 1; 
            elem_mostrador = nuevo;    
            switch(elem_mostrador){
                case 1: sem_post(&tabaco); 
                    break; 
                case 2: sem_post(&cerillas); 
                    break; 
                case 3: sem_post(&papel); 
                    break; 
            }
        }
    }
}

int main(){

    srand( time(NULL) ); // Inicializa la semilla aleatoria
    
    // Hebras
    pthread_t fumador_tabaco, fumador_papel, fumador_cerillas, h_estanquero; 

    // Semáforos
    sem_init(&mostrador, 0, 1); 
    sem_init(&cerillas, 0, 0); 
    sem_init(&papel, 0, 0); 
    sem_init(&tabaco, 0, 0); 


    // Creación de hebras
    pthread_create(&fumador_tabaco, NULL, fumador_sin_tabaco, NULL);  
    pthread_create(&fumador_papel, NULL, fumador_sin_papel, NULL); 
    pthread_create(&fumador_cerillas, NULL, fumador_sin_cerillas, NULL); 
    pthread_create(&h_estanquero, NULL, estanquero, NULL); 

    // Espera de hebras (evita que el programa acabe al finalizar el main)
    pthread_join(fumador_tabaco, NULL); 
    pthread_join(fumador_papel, NULL); 
    pthread_join(fumador_cerillas, NULL); 
    pthread_join(h_estanquero, NULL); 
     

}



