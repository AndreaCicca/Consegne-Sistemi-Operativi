// Scrivere un programma in linguaggio C che presi in input due file contenenti 
// le matrici A e B calcoli il risultato della moltiplicazione delle due matrici,
// tracciando i tempi di computazione

/*
Autore: Andrea Ciccarello
Esercizio: Prototto tra matrici specifica numero 1
Versione 1.0
Data Consegna : 26/02/2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define DimensioneStandard 3


/************************  prodotto_matrice_normale() ***********************/
/*!
    \brief  Questa serve per creare il prodotto tra 2 matrici
    \param  n grandezza delle matrici che vengono passate alla funzione
    \param  MatriceA seconda matrice che viene passata alla funzione
    \param  MatriceB prima matrice che viene passata alla funzione
    \return 0 = Esecuzione terminata con successo

    Lo scopo di questa funzione e` quello di calcolare il prodotto matriciale nxn
*/
int prodotto_matrice_normale (int n, int MatriceA[n][n], int MatriceB[n][n], int MatriceC[n][n])
{   
    int i, j, k;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            for (k = 0; k < n; k++)
            {
                MatriceC[i][j] += MatriceA[i][k] * MatriceB[k][j];
            }
        }
    }
    return 0;
}


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



int main()
{
    srand(time(NULL));

    // chiedere quanto debbano essere grandi le matrici
    printf("\nQuesto programma serve ad effettuare il prodotto tra matrici attraverso la specifica 1 \nPer modificare la dimensione delle matrici devi modificare la costante di preprocessing 'DimensioneStandard' ");
    printf("\nRicordati che le matrici di test devono essere chiamate MatriceA e Matrice B ");


    int MatriceA[DimensioneStandard][DimensioneStandard];
    int MatriceB[DimensioneStandard][DimensioneStandard];
    int MatriceC [DimensioneStandard][DimensioneStandard];

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



    clock_t t0 = clock();

    prodotto_matrice_normale(DimensioneStandard, MatriceA, MatriceB, MatriceC);

    clock_t t1 = clock();

    printf("\nPrima matrice\n");
    stampa(DimensioneStandard, MatriceA);

    printf("Seconda matrice\n");
    stampa(DimensioneStandard, MatriceB);\

    printf("Prodotto tra matrici\n");
    stampa(DimensioneStandard, MatriceC);


    printf("Tempo di esecuzione: %f secondi\n", (double)(t1 - t0) / CLOCKS_PER_SEC);
}