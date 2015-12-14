/*
 * =====================================================================================
 *
 *       Filename:  cannon.cc
 *
 *    Description:  Implementation of cannon algorithm
 *
 *        Version:  1.0
 *        Created:  11/23/2015 01:33:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rudolf Hohn, Axel Fahy, Jessy Marin
 *   Organization:  HES-SO hepia section ITI
 *
 * =====================================================================================
 */

#include "cannon.h"
#include "fox.h"	//Pour fonction de multiplication de 2 matrice et additionne les resultas dans la matrice C

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <mpi.h>
#include <cstring>

using namespace std;

void cannon(int* matLocA, int* matLocB, int* matLocC, int nloc) {
	int nbPE, myPE;
    MPI_Comm_size(MPI_COMM_WORLD, &nbPE);
    MPI_Comm_rank(MPI_COMM_WORLD, &myPE);

	// Calcul racine de nbPE
    int q = sqrt(nbPE);

	// Position du precessus dans la matrice
    int lin = myPE / q;    // Ligne
    int col = myPE % q;    // Colonne

    // [Matrice T] Décalage vers la gauche, calcul du processus à qui envoyer et celui de qui reçevoir la matrice
	// Besoin de les calculer une seul fois vu qu'on envoie et reçois toujours du même processus
    int destT = ((myPE + (q-1)) % q)+(q*lin);
    int sourceT = ((myPE + 1) % q)+(q*lin);

	// [Matrice S] Décalage vers le haut, calcul du processus à qui envoyer et celui de qui reçevoir la matrice
    int destS = (myPE + (q - 1) * q) % (q * q);
    int sourceS = (myPE + q) % (q * q);

    // Copie des matrices en local
    int* T = new int[nloc * nloc];
	int* S = new int[nloc * nloc];
    for (int i = 0; i < q * q; i++) {
        T[i] = matLocA[i];
		S[i] = matLocB[i];
    }

    for (int k = 0; k < q; k++) {

    	//Calcul de T0 et S0
    	if(k == 0){
    		// [Matrice T] Décalage à gauche de la ligne i de i position
			// Dernière ligne ne bouge pas (+1 pour aligner lin au vrai numero de ligne
			// (si matrice à 3 lignes lin max = 2, car lin min = 0))
			if ((lin+1) != q){
				int destT0 = ((myPE + (q-(lin+1))) % q)+(q*lin);
				int sourceT0 = ((myPE + (lin+1)) % q)+(q*lin);
				// Envoie ça matrice de i position (numéro de ligne) vers la gauche
				MPI_Send(T, nloc * nloc, MPI_INT, destT0 , 0, MPI_COMM_WORLD);
				// Reçois ça matrice de ça source
				MPI_Recv(T, nloc * nloc, MPI_INT, sourceT0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			// [Matrice S] Décalage en haut de la colonne j de j position
			if((col+1) != q){	//Dernière colonne ne bouge pas
				int destS0 = (myPE + (q - (col+1)) * q) % (q * q);
				int sourceS0 = (myPE + (q*(col+1))) % (q * q);
				// Envoie ça matrice de j position (numéro de colonne) vers le haut
				MPI_Send(S, nloc * nloc, MPI_INT, destS0 , 0, MPI_COMM_WORLD);
				// Reçois ça matrice de ça source
				MPI_Recv(S, nloc * nloc, MPI_INT, sourceS0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
    	}else{
    		// [Matrice T] Décalage à gauche de une position
			// Envoie ça matrice à la destination
			MPI_Send(T, nloc * nloc, MPI_INT, destT , 0, MPI_COMM_WORLD);
			// Reçois ça matrice de ça source
			MPI_Recv(T, nloc * nloc, MPI_INT, sourceT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			// [Matrice S] Décalage vers le haut de une position
			// Envoie ça matrice à la destination
			MPI_Send(S, nloc * nloc, MPI_INT, destS , 0, MPI_COMM_WORLD);
			// Reçois ça matrice de ça source
			MPI_Recv(S, nloc * nloc, MPI_INT, sourceS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	}

		// Multiplication des matrice A et B dans C
        multMatrix(T, nloc, nloc, S, nloc, nloc, matLocC);
    }

	//Supression des matrices local
    delete[] T;
	delete[] S;
}
