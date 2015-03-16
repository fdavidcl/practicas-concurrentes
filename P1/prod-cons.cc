#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

using namespace std                                         ;

// Variables globales
static unsigned num_items = 50                              ;
vector<int> buffer                                          ;
sem_t no_vacio                                              ;
sem_t no_lleno                                              ;
sem_t critical                                              ;

int producir_dato()                                         {
  static int contador = 1                                   ;
  return contador++                                         ;
                                                            }
void consumir_dato(int dato)                                {
  cout << "Dato recibido: " << dato << endl                 ;
                                                            }
// Procedimiento para la hebra productora
void * productor(void *)                                    {
  for (unsigned i = 0; i < num_items; i++)                  {
    if (buffer.size() == 10)
      sem_wait(&no_lleno)                                   ;
    
    sem_wait(&critical)                                     ;
    int dato = producir_dato()                              ;
    buffer.push_back(dato)                                  ;
    sem_post(&critical)                                     ;
    
    sem_post(&no_vacio)                                     ;
                                                            }
  return NULL                                               ;
                                                            }
// Procedimiento para la hebra consumidora
void * consumidor(void *)                                   {
  for (unsigned i = 0; i < num_items; i++)                  {
    sem_wait(&no_vacio)                                     ;
    
    sem_wait(&critical)                                     ;
    int dato = buffer.back()                                ;
    buffer.pop_back()                                       ;
    consumir_dato(dato)                                     ;
    sem_post(&critical)                                     ;
    
    sem_post(&no_lleno)                                     ;
                                                            }
  return NULL                                               ;
                                                            }

int main(int argc, char * argv[])                           {
  pthread_t hebra1, hebra2                                  ;

  sem_init(&no_vacio, 0, 0)                                 ;
  
  pthread_create(&hebra1, NULL, productor, NULL)            ;
  pthread_create(&hebra2, NULL, consumidor, NULL)           ;

  pthread_join(hebra1, NULL)                                ;
  pthread_join(hebra2, NULL)                                ;
  return 0                                                  ;
                                                            }