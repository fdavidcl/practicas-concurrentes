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

sem_t puede_leer, puede_escribir; 

int producir_dato(){
    return generados++;
}

void escribe(int dato){
    cola.push(dato); 
    sem_post(&puede_leer);
}

void * productor(void *){

    for(unsigned i = 0 ; i < num_items ; i++){
        int dato = producir_dato();
        sem_wait(&puede_escribir);
        escribe(dato); 
        }
    return NULL;
}

void consumir_dato(int dato){ 
    cout << "He leído un dato: " << dato << endl; 
}

void * consumidor(void *){
    for(unsigned i = 0 ; i < num_items ; i++){
        int dato;
        sem_wait(&puede_leer); 
        dato = cola.front();            // Falta añadir exclusión mutua.
        cola.pop();  
        sem_post(&puede_escribir);
        consumir_dato(dato);    
    }
    return NULL;
}


int main(){

    sem_init(&puede_leer, 0, 0); 
    sem_init(&puede_escribir, 0, tam_cola); 
    pthread_t hebral, hebrae;  

    pthread_create(&hebrae, NULL, productor, NULL);
    pthread_create(&hebral, NULL, consumidor, NULL);
    
    pthread_join(hebral, NULL); 
    pthread_join(hebrae, NULL);  

}

