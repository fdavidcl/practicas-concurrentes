#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

using namespace std                                         ;

// Variables globales
static unsigned num_items = 50                              ;
const int tam_vector = 10                                   ;
int p_ocupada = 0                                           ;
int p_libre = 0                                             ;
int buffer[tam_vector]                                      ;
sem_t lecturas                                              ;
sem_t espacio                                               ;
sem_t critical                                              ;
sem_t salida                                                ;

int producir_dato()                                         {
  static int contador = 1                                   ;
  return contador++                                         ;
                                                            }
void consumir_dato(int dato)                                {
  sem_wait(&salida)                                         ;
  cout << "Dato recibido: " << dato << endl                 ;
  sem_post(&salida)                                         ;
                                                            }/*
Procedimiento para la hebra productora                      */
void * productor(void *)                                    {
  for (unsigned i = 0; i < num_items; i++)                  {
    int dato = producir_dato()                              ;/*
    
    Mientras haya espacio, podemos escribir
    nuevos datos en el buffer                               */
    sem_wait(&espacio)                                      ;
    sem_wait(&critical)                                     ;/*
    
    Colocamos el dato en la próxima posición libre          */
    buffer[p_libre] = dato                                  ;
    p_libre = (p_libre + 1) % tam_vector                    ;
    sem_post(&critical)                                     ;/*
    
    Notificamos una nueva lectura pendiente                 */    
    sem_post(&lecturas)                                     ;
                                                            }
  return NULL                                               ;
                                                            }
// Procedimiento para la hebra consumidora
void * consumidor(void *)                                   {
  for (unsigned i = 0; i < num_items; i++)                  {
    sem_wait(&lecturas)                                     ;
    
    sem_wait(&critical)                                     ;
    int dato = buffer[p_ocupada]                            ;
    p_ocupada = (p_ocupada + 1) % tam_vector                ;
    sem_post(&critical)                                     ;
    sem_post(&espacio)                                      ;
    
    consumir_dato(dato)                                     ;
                                                            }
  return NULL                                               ;
                                                            }

int main(int argc, char * argv[])                           {
  pthread_t hebra1, hebra2                                  ;
  
  sem_init(&lecturas, 0, 0)                                 ;
  sem_init(&espacio, 0, tam_vector)                         ;
  sem_init(&critical, 0, 1)                                 ;
  sem_init(&salida, 0, 1)                                   ;
  
  pthread_create(&hebra1, NULL, productor, NULL)            ;
  pthread_create(&hebra2, NULL, consumidor, NULL)           ;

  pthread_join(hebra1, NULL)                                ;
  pthread_join(hebra2, NULL)                                ;
  
  cout << "fin" << endl                                     ;
  
  return 0                                                  ;
                                                            }