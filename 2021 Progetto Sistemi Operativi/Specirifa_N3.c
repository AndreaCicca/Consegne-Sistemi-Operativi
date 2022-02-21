// Espandere la specifica con pthread 

/*
Autore: Andrea Ciccarello
Esercizio: Prototto tra matrici specifica numero 3
Versione 1.0
Data Consegna : 26/02/2022
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define DimensioneStandard 3

pthread_mutex_t mutex; // mutex per la sincronizzazione

// dichiarazione delle 3 matrici A X B = C

    int MatriceA[DimensioneStandard][DimensioneStandard];
    int MatriceB[DimensioneStandard][DimensioneStandard];
    int MatriceC[DimensioneStandard][DimensioneStandard];



/************************  ReadMatrice() ***********************/
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

/************************  stampa() ***********************/
/*!
    \brief  Questa funzione serve per stampare una matrice nxn
    \param  n grandezza della matrice che bisogna stampare
    \param  Matrice array nxn che conterrà la matrice da stampare
    \return void

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

/************************  funzione_prodotto_tra_matrici() ***********************/
/*!
    \brief  Questa funzione che viene esiguita da ogni thread di pthread per calcolare il prodotto tra due matrici
    \param  parametro contiene l'indice del thread 
    \return void 

    Lo scopo di questa funzione e` quello di calcolare il prodotto tra due matrici utilizzando nxn pthread
*/
void* funzione_prodotto_tra_matrici (void *parametro){
    
    int * indice_del_thread = (int *) parametro; //Indice di riferimento del thread

    int prodotto_indice = 0; // valore finale che poi dovrà essere inserito in MatriceC

    for (int i = 0; i < DimensioneStandard; i++) // ciclo per caloclare prodotto indice 
    {
        prodotto_indice = prodotto_indice + MatriceA[(* indice_del_thread)/DimensioneStandard][i] * MatriceB[i][(* indice_del_thread)%DimensioneStandard];
    }

    //scrivere il prodotto_indice al'interno della matrice C

    pthread_mutex_lock (&mutex); // lock della mutex per la sincronizzazione

    MatriceC[(* indice_del_thread) / DimensioneStandard][(* indice_del_thread) % DimensioneStandard] = prodotto_indice;
    
    pthread_mutex_unlock (&mutex); // unlock della mutex per la sincronizzazione

    return NULL;
}


int main()
{
    srand(time(NULL));

    printf("\nQuesto programma serve ad effettuare il prodotto tra matrici attraverso la specifica 3 \nPer modificare la dimensione delle matrici devi modificare la costante di preprocessing 'DimensioneStandard' \nRicordati che le matrici di test devono essere chiamate MatriceA e Matrice B" );

    //inizializzo MatriceC a 0
    for (int i = 0; i < DimensioneStandard; i++)
    {
        for (int j = 0; j < DimensioneStandard; j++)
        {
            MatriceC[i][j] = 0;
        }
    }
    

    ReadMatrice(DimensioneStandard, MatriceA, "MatriceA.txt");
    ReadMatrice(DimensioneStandard, MatriceB, "MatriceB.txt");

    pthread_mutex_init(&mutex, NULL);

    pthread_t prodotto_matrice_thread[DimensioneStandard*DimensioneStandard]; // array di pthread

    int indice_thread[DimensioneStandard*DimensioneStandard]; // indice del thread da passare alla funzione

    clock_t t0 = clock(); // tempo iniziale


    for (int i = 0; i < DimensioneStandard * DimensioneStandard; i++)
    {
        indice_thread[i] = i; // indice da passare alla funzione

        if (pthread_create(&prodotto_matrice_thread[i],NULL,funzione_prodotto_tra_matrici,(void *)  &indice_thread[i]) < 0)
        {
            printf("Errore nella creazione del thread\n");
            exit(EXIT_FAILURE);
        }

    }

    for (int i = 0; i < DimensioneStandard; i++)
    {
       pthread_join(prodotto_matrice_thread[i], NULL);
    }

    clock_t t1 = clock(); // tempo finale


    printf("\nPrima matrice\n");
    stampa(DimensioneStandard, MatriceA);

    printf("Seconda matrice\n");
    stampa(DimensioneStandard, MatriceB);

    printf("Prodotto tra matrici\n");
    stampa(DimensioneStandard, MatriceC);


    printf("Tempo di esecuzione: %f secondi\n", (double)(t1 - t0) / CLOCKS_PER_SEC);
}