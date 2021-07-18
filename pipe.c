/*! 
 *	@authors Marco Rondelli
 *	@authors Nicholas Bianchini
 *	@version 0.1
 *	@date 18/12/2020
 *	@copyright None
*/

/*! @file pipe.c
 * @brief Come funziona una Unix pipe.
 *
 * In questo file, creiamo due processi (padre e figlio) per mostrare il funzionamento
 * di una pipe: il figlio preleva da un file di testo una stringa e la stampa su stdout,
 * il padre prende la stringa e stampa un carattere alla volta seguito da a capo
 * e raggiunto un punto, termina l'esecuzione della pipe.
 * Da ricordare le funzioni per l'apertura e lettura da file: \n
 * op = open( "data.txt", O_RDONLY ) \n
 * rd = read( op, str, BUF_SIZE ) .
*/

#define _GNU_SOURCE ///< Inclusione funzioni GNU/Linux non standard e basso livello
#define BUF_SIZE 256 ///< Il buffer size della stringa che vogliamo stampare

#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main() {
  int pid, op, rd;	/// pid = process id \n
					/// op = contiene file descriptor \n
					/// rd = legge nel file aperto
  int fd[2];	/// fd[2] = contiene i 2 file descriptor (0:1)
  char str[ BUF_SIZE ], z;	/// str[ BUF_SIZE ] = la stringa letta di lunghezza fissa
							/// z = il carattere nel ciclo di stampa su shell
  op = open( "data.txt", O_RDONLY );
  rd = read( op, str, BUF_SIZE );
  pipe(fd);

  if ((pid = fork()) == 0) { /* figlio */
    /* chiusura lettura da pipe */
    close(fd[0]);
    /* redirezione stdout a pipe */
    dup2(fd[1], 1);
    printf("%s", str);
  }
  
  else if (pid > 0) { /* padre */
    /* chiusura scrittura su pipe */
    close(fd[1]);
    /* redirezione stdin a pipe */
    dup2(fd[0],0);
    scanf("%c", &z);
    while( z != '.' ){
      printf("%c \n", z);
      scanf("%c", &z);
    }
  }
}
