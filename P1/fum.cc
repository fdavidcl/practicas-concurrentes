#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

enum Ingrediente                                                            {
  TABACO  = 0                                                               ,
  PAPEL   = 1                                                               ,
  CERILLA = 2                                                               ,
                                                                            };
std::string to_s(Ingrediente ing)                                           {
  if (ing == TABACO)
    return "tabaco"                                                         ;
  else if (ing == PAPEL)
    return "papel"                                                          ;
  else
    return "cerillas"                                                       ;
                                                                            }/*
Array de semáforos, uno por ingrediente                                     */
sem_t sem_ingr[3]                                                           ;
sem_t sem_most                                                              ;
sem_t salida                                                                ;

Ingrediente mostrador                                                       ;

void fumar()                                                                {/*
  Calcular un número aleatorio de milisegundos                              */
  const unsigned miliseg = 100U + (rand() % 1900U)                          ;/*
  
  Retraso bloqueado durante `miliseg` ms                                    */
  usleep(1000U * miliseg)                                                   ;
                                                                            }

void * fumador(void * faltante)                                             {
  Ingrediente f = (Ingrediente)(long int)faltante                           ;
  
  while (true)                                                              {
    sem_wait(&sem_ingr[f])                                                  ;
    
    sem_wait(&salida)                                                       ;
    std::cout << "He recogido el ingrediente " << to_s(f) << 
      " y entro a fumar." << std::endl                                      ;
    sem_post(&salida)                                                       ;
    
    sem_post(&sem_most)                                                     ;
    fumar()                                                                 ;
    
    sem_wait(&salida)                                                       ;
    std::cout << "He terminado de fumar, me falta " << to_s(f) << 
      " de nuevo." << std::endl                                             ;
    sem_post(&salida)                                                       ;
                                                                            }
                                                                            }
void * estanquero(void *)                                                   {
  while (true)                                                              {
    Ingrediente ing = (Ingrediente) (rand() % 3)                            ;
    
    sem_wait(&salida)                                                       ;
    std::cout << "Estanquero: pongo " << to_s(ing) << 
      " en el mostrador." << std::endl                                      ;
    sem_post(&salida)                                                       ;
    
    mostrador = ing                                                         ;
    sem_post(&sem_ingr[ing])                                                ;
    sem_wait(&sem_most)                                                     ;
    
                                                                            }
                                                                            }

int main()                                                                  {/*
  Inicializar la semilla aleatoria                                          */
  srand(time(NULL))                                                         ;
  
  pthread_t fumadores[3]                                                    ;
  pthread_t estanq                                                          ;
  
  sem_init(&sem_most, 0, 1)                                                 ;
  sem_init(&salida, 0, 1)                                                   ;
  
  for (int i = 0; i < 3; ++i)
    sem_init(sem_ingr + i, 0, 0)                                            ;
  
  for (int i = 0; i < 3; ++i)                                               {
    pthread_create(fumadores + i, NULL, fumador, (void *)(long int)i)       ;
                                                                            }
  pthread_create(&estanq, NULL, estanquero, NULL)                           ;
  
  pthread_join(estanq, NULL)                                                ;
                                                                            }