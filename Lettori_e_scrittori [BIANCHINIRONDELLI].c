/*!
 *    @authors Marco Rondelli
 *    @authors Nicholas Bianchini
 *    @version 0.1
 *    @date 18/12/2020
 *    @copyright None
*/

/*! @file
 * @brief Funzionamento dei thread attraverso l'esempio di scrittore e lettori
 *
 * Creiamo uno scrittore il cui scopo è quello di scrivere su un array,
 * l'array verrà poi letto da due lettori, in tempi uniformi ma diversi,
 * che a loro volta stamperanno su schermo il valore letto. \n
 * @var int readers
 * @brief L'indice che indica quanti lettori stanno leggendo
 * @var int cont
 * @brief Contatore all'interno dell'array
 * @fn void *read1 (void * arg)
 * @brief La prima funzione per la lettura (tempo di lettura 2)
 * @fn void *read2 (void * arg)
 * @brief La seconda funzione per la lettura (tempo di lettura 1)
 * @fn void *scrivi(void * arg)
 * @brief La funzione per la scrittura
*/

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

/* static sem_t mutex
 * Questo il semaforo che gestisce l'accesso alla variabile readers
 * static w_sem
 * Questo il semaforo che gestisce l'array int A[100]
*/

static sem_t mutex,wsem;  /// semafori
int A[100];
int readers=0;
int cont=0;

void *scrivi(void * arg){
    sem_wait(&wsem);
    int i=0;
    while (i<100){
        A[i]=i+1;
        printf("Scrittore ha aggiornato il buffer \n");
        sleep(1);
        i++;
    }
    printf("Scrittura terminata \n");
    sleep(1);
    printf("Lo scrittore sta uscendo...\n");
    sem_post(&wsem);
    pthread_exit(0);
}


void *read1 (void * arg){
    sem_wait(&mutex);
    readers++;
    if (readers==1)
        sem_wait(&wsem);
    sem_post(&mutex);
    sleep(3);
    printf("Lo scrittore e' uscito, i lettori possono iniziare \n");
    while(cont<20){
        printf("Lettore 1 legge valore %d\n", A[cont]);
        cont++;
        sleep(2);
    }
    sem_wait(&mutex);
    readers--;
    if (readers==0)
        sem_post(&wsem);
    sem_post(&mutex);
    pthread_exit (0);
}

void *read2 (void * arg){
    sem_wait(&mutex);
    readers++;
    if (readers==1)
        sem_wait(&wsem);
    sem_post(&mutex);
    sleep(4);
    while(cont<20){
        printf("Lettore 2 legge valore %d\n", A[cont]);
        cont++;
        sleep(1);
    }
    sem_wait(&mutex);
    readers--;
    if (readers==0)
        sem_post(&wsem);
    sem_post(&mutex);
    pthread_exit (0);
}

/************************  main() ***********************/
/*!
    @brief Dichiaro i 3 thread (due lettori e uno scrittore), imposto i semafori ed eseguo \n
    eventuali controlli, eseguo i thread e infine li termino.
*/

// dichiaro i 3 thread
int main()
{
  pthread_t r1,r2,scr;
  void * ret;

  sem_init(&mutex,0,1);
  sem_init(&wsem,0,1);

  if (pthread_create(&scr, NULL, scrivi, NULL) < 0)
       { fprintf (stderr, "pthread_create error for thread scr\n");
       exit (1);
       }
  sleep(2);
  printf("Inizio lettura in corso... \n");
  if (pthread_create(&r1, NULL, read1, NULL) < 0)
     { fprintf (stderr, "pthread_create error for thread r1\n");
     exit (1);
     }

   if (pthread_create(&r2, NULL, read2, NULL) < 0)
     { fprintf (stderr, "pthread_create error for thread r2\n");
     exit (1);
     }

   // termino i thread
   pthread_join (scr, &ret);
   pthread_join (r1, &ret);
   pthread_join (r2, &ret);

   printf("Exit\n");
 }


