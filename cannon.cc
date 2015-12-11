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
#include "fox.h"	//Pour fonction de multiplication de 2 matrice, renvoie la matrice de resultas

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
	
	cout << "Gniaaa";
	
	
	
	// Calcul racine de nbPE
    int q = sqrt(nbPE);
    
    cout << "q=" << q << endl;
	
	// Position du precessus dans la matrice
    int lin = myPE / q;    // Ligne
    int col = myPE % q;    // Colonne
	
    // [Matrice A] Calcul du processus à qui envoyer et celui de qui reçevoir la matrice
	// Besoin de les calculer une seul fois vu qu'on envoie et reçois toujours du même processus
    int destA = ((myPE + (q-1)) % q)+(q*lin);
    int sourceA = ((myPE + 1) % q)+(q*lin);
	
	// [Matrice B] Calcul du processus à qui envoyer et celui de qui reçevoir la matrice
    int destB = (myPE + (q - 1) * q) % (q * q);
    int sourceB = (myPE + q) % (q * q);

    // Copie des matrices en local
    int* My_matLocA = new int[nloc * nloc];
	int* My_matLocB = new int[nloc * nloc];
    for (int i = 0; i < q * q; i++) {
        My_matLocA[i] = matLocA[i];
		My_matLocB[i] = matLocB[i];
    }
	
	// [Matrice A] Décalage à gauche de la ligne i de i position
	if (lin != 0){
		int destdecalA = ((myPE + (q-lin)) % q)+(q*lin);
		int sourcedecalA = ((myPE + lin) % q)+(q*lin);
		// Envoie ça matrice de i position (numéro de ligne) vers la gauche
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destdecalA , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourcedecalA, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	// [Matrice B] Décalage à gauche de la colonne j de j position
	if(col != 0){
		int destdecalB = (myPE + (q - col) * q) % (q * q);
		int sourcedecalB = (myPE + q) % (q * q);
		// Envoie ça matrice de j position (numéro de colonne) vers le haut
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destdecalB , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourcedecalB, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

    for (int k = 0; k < q; k++) {
		// Multiplication des matrice A et B dans C
        multMatrix(My_matLocA, nloc, nloc, My_matLocB, nloc, nloc, matLocC);

        // [Matrice A] Décalage à gauche de une position
		// Envoie ça matrice à la destination
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destA , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourceA, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
		// [Matrice B] Décalage vers le haut de une position
		// Envoie ça matrice à la destination
		MPI_Send(My_matLocB, nloc * nloc, MPI_INT, destB , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocB, nloc * nloc, MPI_INT, sourceB, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
	//Supression des matrices local
    delete[] My_matLocA;
	delete[] My_matLocB;
}
