/*! 
 *	@authors Marco Rondelli
 *  @authors Nicholas Bianchini
 *	@version 0.1
 *	@date 18/12/2020
 *	@copyright None
*/

/*! @file esempiofork.c
 * @brief Comprensione processi figlio/padre e execl.
 *
 * In questo file trovate una funzione main() il quale scopo è: \n
 * 1) creare un processo figlio e studiarne il comportamento; \n
 * 2) eseguire il comando execl.
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#define PROVA 5 //!< Questa e' una macro globale

int temp; //!< Questa e' una Variabile globale

/************************  main() ***********************/
/*!
    \brief  Questa e' la funzione principale
    \param  argc Il numero di argomenti
    \param  argv Il vettore degli argomenti
    \return 0 = Esecuzione terminata con successo
    \return -1 = Esecuzione ha generato un errore

    Lo scopo di questa funzione e` quello di lanciare un processo figlio
    e attendere la sua terminazione. \n
	Inoltre, viene eseguito il comando di shell "ls" per mostrare i file in directory.
*/

int main(int argc, char *argv[])
{  
  int pid;  
  
  int retv; /// Si utilizza retv per ricevere il valore di ritorno del figlio

  switch(pid=fork()){
  case -1: printf("Errore in creazione figlio\n");
           exit(-1);
  case 0 : printf("Figlio sospende per 2 secondi...\n"); // figlio
           sleep(2); 
           printf("Figlio si risveglia\n");
           exit(3);
  default: printf("Padre esegue e attende figlio\n"); // padre
           wait(&retv); 
           printf("Padre riceve da figlio exit status %d\n",WEXITSTATUS(retv));
  }

printf("\n");
execl("/bin/ls", "ls", "-l", NULL); /// execl esegue comandi di shell
printf("\n");

 return 0;
}
