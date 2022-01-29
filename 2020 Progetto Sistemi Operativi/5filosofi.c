
/*!
 *    @authors Marco Rondelli
 *    @authors Nicholas Bianchini
 *    @version 0.1
 *    @date 18/12/2020
 *    @copyright None
*/

/*! @file 5filosofi.c
 * @brief Funzionamento dei semafori attraverso l'esempio dei 5 filosofi.
 *
 * La creazione dei semafori permette un accesso alle risorse condivise
 * (le bacchette) senza intrusione dei filosofi.
 *
 * @var int filosofo[5]
 * @brief Creiamo un array globale dove contenere i filosofi
 * @var int* state
 * @brief Variabile condivisa per lo state dei filosofi
 * @fn void sem_up(int ipcid, int nsem)
 * @brief Funzione per eseguire la up del semaforo
 * @fn void void sem_down(int ipcid, int nsem)
 * @brief Funzione per eseguire la down del semaforo
 * @fn void sem_set(int ipcid, int nsem, int initial)
 * @brief Funzione per impostare il valore iniziale del semaforo
 * @fn void test(int index, int ipcid)
 * @brief Test sul filosofo index
 * @fn void prendi(int index, int ipcid)
 * @brief Funzione per permetterci di prendere le "bacchette"
 * @fn void posa(int index, int ipcid)
 * @brief Funzione per permetterci di posare le "bacchette"
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DOWN (-1)    //!< Questa è la down per i semafori
#define UP (1)    //<! Questa è la up per i semafori
#define MYCODE 'a' //!< Per ottenere poi chiave IPC
#define SIZE 60 //!< Definiamo per comodità
#define M 0    //!< Il filosofo mangia
#define A 1    //!< Il filosofo è affamato
#define P 2    //!< Il fisolofo pensa


// Definizione della struttura semun
#ifdef _SEM_SEMUN_UNDEFINED
#define _SEM_SEMUN_UNDEFINED  1
union semun /* definita in bits/sem.h */
{
  int val;                              /* value for SETVAL */
  struct semid_ds *buf;                 /* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;            /* array for GETALL & SETALL */
  struct seminfo *__buf;                /* buffer for IPC_INFO */
};
#endif

int filos[5];
int* state;

void sem_up(int ipcid, int nsem)
{
    struct sembuf sops[1];
    sops[0].sem_num = nsem;
    sops[0].sem_op = UP;
    sops[0].sem_flg = 0;
    if (semop(ipcid, sops, 1) == -1) {
      perror("Errore in semop");
      exit(3);
    }
}
void sem_down(int ipcid, int nsem)
{
    struct sembuf sops[1];
    sops[0].sem_num = nsem;
    sops[0].sem_op = DOWN;
    sops[0].sem_flg = 0;
    if (semop(ipcid, sops, 1) == -1) {
      perror("Errore in semop");
      exit(3);
    }
}

void sem_set(int ipcid, int nsem, int initial)
{
  union semun arg;
  arg.val = initial;
  if (semctl(ipcid, nsem, SETVAL, arg) == -1) {
    perror("set value sem");
    exit(2);
  }
}


void test(int index, int ipcid){
if (state[index]==A && state[(index+1)%5]!=M && state[(index+4)%5]!=M){
    state[index]=M;
    printf("Il filosofo %d mangia \n", index+1);
    sem_up(ipcid,index);
 }
}

void prendi(int index, int ipcid){
    sem_down(ipcid,5);
    printf("Il filosofo %d ha fame \n", index+1);
    state[index]=A;
    test(index,ipcid);
    sem_up(ipcid,5);
    sem_down(ipcid,index);
}

void posa(int index, int ipcid){
    sem_down(ipcid,5);
    printf("Filosofo %d posa \n", index+1);
    state[index]=P;
    test(((index+1)%5) , ipcid);
    test(((index+4)%5) , ipcid);
    sem_up(ipcid,5);
}

/************************  main() ***********************/
/*!
    @brief Crea l'oggetto IPC(state) e i semafori,
    inizializza i filosofi allo state P, richiama le funzioni. \n
    Infine chiude IPC e termina i processi.
*/

int main()
{
  key_t key;
  int sid, pid, shmid, att;
  char s[SIZE];
  union semun arg;
  struct sembuf sops[6];

  // per accedere all'oggetto condiviso (nel nostro caso lo stato)
  // siccome non tutti possono accedervi
  // creiamo chiave e la passiamo a chi ne ha bisogno
  if ((key = ftok(".", MYCODE)) == -1) { /* ottieni chiave IPC
					       da pathname (".") e 
					       codice (MYCODE) */
    perror("ftok");
    exit(1);
  }

  // creano IPC state, allochiamo memoria in condivisa

  // all'oggetto può accedervi solo chi ha la chiave 'key'
  shmid=shmget(key, sizeof(char)*SIZE, IPC_CREAT | IPC_EXCL | 0600 );
  state=shmat(shmid, NULL, 0);
    
    // creo semaforo
    // creo oggetto contenente 6 semafori
    // il sesto semaforo per evitare race condition nella modifica dello state
    
  if ((sid = semget(key, 6, 0600 | IPC_CREAT )) == -1) {
    perror("creazione 6 semafori");
    exit(1);
  }
  
  // i semafori vanno lasciati a 0
  // inizializziamo a 1 solo il semaforo per la mutex
  sem_set(sid,5,1);
    
  //tutti i filosofi pensano
  int tmptmp=1; // appoggio
    for (int k=0;k<5;k++){
        state[k]=P;
    }

// creo i filosofi, inserisco il pid di ciascuno nella corssipondente posizione dell'array
// 1 processo è già in corso, quello del codice stesso, quindi faccio altri 4 processi per arrivare ai 5 filosofi
// prendo per il filosofo corrente (figlio) il suo pid se pid==0
  for(int i=0;i<4;i++){
        pid=fork();
        if (pid==0){
            filos[i+1]=getpid();
            break;
        }
// tmptmp fa si che entro in questo if solo una volta
// entro qui nel caso pid>0
// nell'array dei filosofi metto per primo il processo che è padre, poi inserisco gli altri
       if (tmptmp==1 && pid>0){
            filos[0]=getpid();
            tmptmp==0;
      }
        }

    int giusto=0; //funge da index per identificare il filosofo

// ricavo l'index di ciascun filosofo
// eseguito da tutti, serve per autoidentificare ciascun filosofo
// il processo padre entrerà e poì uscirà subito
// questo ci serve per le funzioni sotto prendi() posa()
    while (1==1){
        if (filos[giusto]==getpid())
            break;
        else
            giusto=giusto+1;
    }

    prendi(giusto, sid);
    sleep(5);
    posa(giusto, sid);
    pid=wait(&att);
    sleep(5);
    printf("Padre: terminato figlio. Rimuove shm\n");
    if (shmdt(state) == -1)
       perror("Padre: detach");
    if (shmctl(shmid, IPC_RMID, 0) == -1)
      perror("Padre: rimozione segmento");
    printf("Padre: finito\n");
    exit(0);
    return 0;
    
    
}