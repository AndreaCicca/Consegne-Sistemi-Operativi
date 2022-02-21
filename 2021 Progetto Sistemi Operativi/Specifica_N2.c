/*
Espandere il programma precedente usando una matrice di mxm processi (fork) per calcolare il medesimo
risultato, frutto della moltiplicazione di A e B. Utilizzare le pipe necessarie, opportunamente configurate,
per realizzare il meccanismo “a rotazione” presentato nelle slide 5 e 6. Anche in questo caso l’output,
gestito dal processo padre, dovrà includere i tempi di computazione
*/

/*
Autore: Andrea Ciccarello
Esercizio: Prototto tra matrici specifica numero 2
Versione 1.0
Data Consegna : 26/01/2022
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define DimensioneStandard 3

/********  ReadMatrice() *********/
/*!
    \brief  Questa funzione serve per leggere una matrice da file
    \param  n grandezza della matrice che bisogna leggere
    \param  Matrice array nxn che conterrà la matrice lette
    \return false = errore nell'apertura del file
    \return true = lettura avvenuta con successo
    \return false = lettura non avvenuta con successo

    Lo scopo di questa funzione e` quello di leggere una funzione data da un file
*/
bool ReadMatrice(int n, int Matrice[n][n], char *filename)
{
    FILE *File_Da_Aprire; //puntatore al File che deve essere aprito
    int i, j;
    File_Da_Aprire = fopen(filename, "r"); //Apertura del File tramite fopen
    
    if (File_Da_Aprire == NULL)
    {
        printf("Errore nell'apertura del file %s\n", filename);
        return false;
    }
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            fscanf(File_Da_Aprire, "%d", &Matrice[i][j]);
        }
    }
    fclose(File_Da_Aprire);
    return true;
}

/********  stampa() *********/
/*!
    \brief  Questa funzione serve per stampare una matrice nxn
    \param  n grandezza della matrice che bisogna stampare
    \param  Matrice array nxn che conterrà la matrice da stampare

    Lo scopo di questa funzione e` quello di stampare una matrice nxn
*/
void stampa(int n, int Matrice[n][n])
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("%d ", Matrice[i][j]);
        }
        printf("\n");
    }

    printf("\n \n");
}

/********  ProcessoSottostante() *********/
/*!
    \brief  Questa funzione serve ritornare il numero processo del processo sotto
    \param  numeroProcesso numero del processo corrente
    \param  riga numero di riga nella matrice in cui si trova il processo corrente
    \return numeroProcesso sotto

*/
int ProcessoSottostante(int numeroProcesso, int riga){

    if (riga == DimensioneStandard -1)
        {
            return numeroProcesso-DimensioneStandard * (DimensioneStandard - 1);
    }else{
            return numeroProcesso+DimensioneStandard;
    }
}

/********  ProcessoADestra() *********/
/*!
    \brief  Questa funzione serve ritornare il numero processo del processo a destra
    \param  numeroProcesso numero del processo corrente
    \param  riga numero di colonna nella matrice in cui si trova il processo corrente
    \return numeroProcesso a destra

*/
int ProcessoADestra(int numeroProcesso, int colonna){
    if (colonna == DimensioneStandard -1)
        {
            return numeroProcesso-DimensioneStandard+1;
    }else{
            return numeroProcesso+1;
    }
}

                
                


int main (){

    srand(time(NULL));

    // Stampa iniziale che richiede la dimensione delle matrici e spiega il funzionamento del programma
    printf("\nQuesto programma serve ad effettuare il prodotto tra matrici attraverso la specifica 2 \nPer modificare la dimensione delle matrici devi modificare la costante di preprocessing 'DimensioneStandard'\n");
    printf("Ricordati che le matrici di test devono essere chiamate MatriceA e Matrice B\n");

    //creazione delle matrici A, B e C
    int MatriceA[DimensioneStandard][DimensioneStandard];
    int MatriceB[DimensioneStandard][DimensioneStandard];
    int MatriceC[DimensioneStandard][DimensioneStandard];


    //creazione delle pipe 
    int verticali[DimensioneStandard*DimensioneStandard][2];
    int orizzontali[DimensioneStandard*DimensioneStandard][2];
    int risultato_prodotto_MatriceC[DimensioneStandard*DimensioneStandard][2];


    int prodotto = 0; //prodotto tra due valori che poi dovrà essere passato tramite pipe al processo Padre

    int processoNum = 0; //numero per indentificare un preciso processp

    int sotto = 0, destra = 0; //valori che verranno usati per indicare quale siano le pile sotto e a destra del processo corrente

    int riga1 = 0, colonna1 = 0; // variabili di appoggio per il padre

    int indice = 0; // indice per stabilire quali valori a e b debbano essere assegnati ad ogni indice

    int indice_precedente = 0; //indice precedente rispetto all'indice corrente

    int b = 0,a = 0; //valori da assegnare a ciascun processo che poi verranno passati tramite pipe


    //inizializzazione a 0 della matrice MatriceC
    for (int i = 0; i < DimensioneStandard; i++)
    {
        for (int j = 0; j < DimensioneStandard; j++)
        {
            MatriceC[i][j] = 0;
        }
    }

    //lettura matrici da file
    if (ReadMatrice(DimensioneStandard, MatriceA, "MatriceA.txt") == false)
    {
        printf("Errore nella lettura del file MatriceA.txt\n");
        return 1;
    }
    if (ReadMatrice(DimensioneStandard, MatriceB, "MatriceB.txt") == false)
    {
        printf("Errore nella lettura del file MatriceB.txt\n");
        return 1;
    }

    //inizializzazione delle pipe
    for (int i = 0; i < DimensioneStandard*DimensioneStandard; i++)
    {
        if(pipe(verticali[i]) == -1 || pipe(orizzontali[i]) == -1 || pipe(risultato_prodotto_MatriceC[i]) == -1)
        {
            printf("Errore nella creazione delle pipe numero %d del ciclo\n", i);
            exit(EXIT_FAILURE);
        }
    }


    pid_t pid; // Valore pid usato per creare i processi tramite la fork

    clock_t t0 = clock(); // inizio misurazione del tempo di esecuzione

    //creazione dei processi
    for (int riga = 0; riga < DimensioneStandard; riga++)
    {
        for (int colonna = 0; colonna < DimensioneStandard; colonna++, processoNum++, indice++ ){
            
            // seghettatura
            if (indice >= DimensioneStandard) indice = 0;

            //creazione del processo figlio
            pid = fork();
            

            // errore creazione processi
            if (pid < 0)
            {
                printf("Errore nella creazione del processo %d\n", processoNum);
                exit(EXIT_FAILURE);
            }
            
            //processo figlio

            if (pid == 0){

                //asegnazione del processo
                a = MatriceA[riga][indice];
                b = MatriceB[indice][colonna];

                // come prima cosa ogni processo deve calcolare il prodotto di a*b
                prodotto = a*b;

                // per capire quale sia il processo vicino a livello verticale  
                sotto = ProcessoSottostante(processoNum, riga);

                // per capire quale sia il processo vicino a livello orizzontale
                destra = ProcessoADestra(processoNum, colonna);


                //chiusura delle pipe che corrispondono al processo figlio e dai vicini che però non sono utili
                close(orizzontali[processoNum][0]);
                close(verticali[processoNum][0]);
                close(orizzontali[destra][1]);
                close(verticali[sotto][1]);

                // chiusura delle pipe inutilizzate dal processo figlio

                for (int i=0; i < DimensioneStandard*DimensioneStandard; i++)
                {
                    if (i != processoNum && i!= sotto && i!= destra)
                    {

                        close(verticali[i][0]);
                        close(verticali[i][1]);
                        close(risultato_prodotto_MatriceC[i][0]);
                        close(risultato_prodotto_MatriceC[i][1]);
                        close(orizzontali[i][0]);
                        close(orizzontali[i][1]);
                        
                    }
                }
                
                // gestione della rotazione delle slide 5/6 e calcolo del prodotto che dovrà essere passato al precesso padre
                for (int j = 0; j < DimensioneStandard-1; j++)
                {   
                    //scrittura dei valori del processo figlio alla pipe orizzontale e verticale ai processi vicini
                    write(orizzontali[processoNum][1], &a, sizeof(int));
                    write(verticali[processoNum][1], &b, sizeof(int));

                    //lettura dei valori sulle pipe orizzontale e verticale dai processi vicini
                    read(orizzontali[destra][0], &a, sizeof(int));
                    read(verticali[sotto][0], &b, sizeof(int));
                    prodotto = prodotto + a*b;
                }

                //passaggio prodotto al processo padre

                write(risultato_prodotto_MatriceC[processoNum][1], &prodotto, sizeof(int));
                close(risultato_prodotto_MatriceC[processoNum][0]);
                close(risultato_prodotto_MatriceC[processoNum][1]);
                
                //chiusura pipe utilizzate dal processo figlio
                close(orizzontali[processoNum][1]);
                close(verticali[processoNum][1]);
                close(orizzontali[destra][0]);
                close(verticali[sotto][0]);

                //fine del processo figlio
                exit(0);
            }

            // per poter riepsettare le specufuche bisogna far partire il processo ad inizio di ogni riga in una posizione sotto rispetto a quella precedente

            indice = indice_precedente +1;
            indice_precedente = indice;
             
        }
    }
    
    clock_t t1 = clock();
    
    //Processo padre
    prodotto = 0;
    
    //lettura risultati
    for (int i = 0; i < DimensioneStandard*DimensioneStandard; i++)
    {

        close(risultato_prodotto_MatriceC[i][1]);

        read(risultato_prodotto_MatriceC[i][0], &prodotto, sizeof(int));

        close(risultato_prodotto_MatriceC[i][0]);

        riga1 = i/DimensioneStandard;
        colonna1 = i%DimensioneStandard;

        MatriceC[riga1][colonna1] = prodotto;

        
    }
    
    //aspetto la terminazione di tutti i processi con la whait


    for (int i = 0; i < DimensioneStandard*DimensioneStandard; i++)
    {
        wait(NULL);
    }

    clock_t t2 = clock();
    
     // termine misurazione del tempo 

    //stampa matrici
   printf("\nPrima matrice\n");
    stampa(DimensioneStandard, MatriceA);

    printf("Seconda matrice\n");
    stampa(DimensioneStandard, MatriceB);

    printf("Prodotto tra matrici\n");
    stampa(DimensioneStandard, MatriceC);

    printf("Tempo di esecuzione: %f secondi\n", (double)(t1 - t0) / CLOCKS_PER_SEC);

   return 0;
}