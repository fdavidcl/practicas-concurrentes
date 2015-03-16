// José Carlos Entrena. 

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> // incluye ”rand()” y ”srand”
#include <queue> 

using namespace std; 

const unsigned num_items = 100; 
const unsigned tam_cola = 10; 
queue<int> cola; 
static int generados = 0;  

sem_t puede_leer, puede_escribir, e_mutua; 

int producir_dato(){
    return generados++;
}

/**
 * Función que escribe un dato entero en la cola.
 */
void escribe(int dato){
    // Inicio de la exclusión mutua para acceder a la cola
    sem_wait(&e_mutua); 
    cola.push(dato); 
    sem_post(&e_mutua); 
    
    sem_post(&puede_leer);
}

/**
 * Función que irá produciendo datos y llamará a escribe(int) para
 * escribirlos en la cola. Le asignaremos una hebra. 
 */
void * productor(void *){

    for(unsigned i = 0 ; i < num_items ; i++){
        int dato = producir_dato();
        sem_wait(&puede_escribir);
        escribe(dato); 
        }
    return NULL;
}

/** 
 * Imprime el entero dato por pantalla. 
 */ 
void consumir_dato(int dato){ 
    cout << "He leído un dato: " << dato << endl; 
}

/**
 * Función que irá leyendo datos de la cola. Le asignaremos una hebra. 
 */
void * consumidor(void *){
    for(unsigned i = 0 ; i < num_items ; i++){
        int dato;
        sem_wait(&puede_leer);          // Esperamos a poder leer. 
        // Exclusión mutua
        sem_wait(&e_mutua); 
        dato = cola.front();            
        cola.pop();  
        sem_post(&e_mutua); 

        sem_post(&puede_escribir);
        consumir_dato(dato);    
    }
    return NULL;
}


int main(){

    sem_init(&puede_leer, 0, 0); 
    sem_init(&puede_escribir, 0, tam_cola); 
    sem_init(&e_mutua, 0, 1); 

    pthread_t hebral, hebrae;  

    pthread_create(&hebrae, NULL, productor, NULL);
    pthread_create(&hebral, NULL, consumidor, NULL);
    
    pthread_join(hebral, NULL); 
    pthread_join(hebrae, NULL);  

}

