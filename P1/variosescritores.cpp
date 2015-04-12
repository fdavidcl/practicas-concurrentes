// José Carlos Entrena. 
// Modificación lector/escritor. Tiene 3 escritores y un lector. 
// Cuando se produce un elemento, se muestra en pantalla el productor que lo ha producido. 
// Esto se hace para ver que efectivamente trabajan todos los productores. 

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> // incluye ”rand()” y ”srand”
#include <queue> 

using namespace std; 

const unsigned num_items = 100; 
const unsigned tam_cola = 10; 
queue<int> cola; 
static unsigned int generados = 0;  

sem_t puede_leer;           // Lectura
sem_t puede_escribir;       // Escritura
sem_t e_mutua;              // Exclusión mutua. Para el acceso a la cola. 
sem_t sem_gen;              // Exclusión para la generación de elementos por los escritores. 
sem_t escritura;            // Salida por pantalla. 

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
 * Con el semáforo sem_gen evitaremos que distintos productores interfieran entre ellos. 
 */
void * productor(void * status){

   while(generados < num_items){
        sem_wait(&sem_gen); 
        int dato = producir_dato();
        sem_post(&sem_gen); 

        unsigned long numero = (unsigned long)status; 

        sem_wait(&escritura); 
        cerr << "Soy el productor " << numero << " y he producido el " << dato << endl; 
        sem_post(&escritura); 
        sem_wait(&puede_escribir);
        escribe(dato); 
        }
    return NULL;
}

/** 
 * Imprime el entero dato por pantalla. 
 */ 
void consumir_dato(int dato){ 
    sem_wait(&escritura); 
    cout << "He leído un dato: " << dato << endl; 
    sem_post(&escritura); 
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
    sem_init(&sem_gen, 0, 1); 
    sem_init(&escritura, 0, 1); 

    pthread_t hebralector, hebraescritor, hebraescritor2, hebraescritor3;  

    pthread_create(&hebraescritor, NULL, productor, (void *)1);
    pthread_create(&hebralector, NULL, consumidor, NULL);
    pthread_create(&hebraescritor2, NULL, productor, (void *)2); 
    pthread_create(&hebraescritor3, NULL, productor, (void *)3); 
    
    
    pthread_join(hebralector, NULL); 
    pthread_join(hebraescritor, NULL);  
    pthread_join(hebraescritor2, NULL); 
    pthread_join(hebraescritor3, NULL); 

    cout << "FIN" << endl; 

}

