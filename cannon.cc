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
    
    /*
    cout << "[Processus : " << myPE << "] racine : " << q << endl;
    cout << "[Processus : " << myPE << "] Position ligne : " << lin << endl;
    cout << "[Processus : " << myPE << "] Position colonne : " << col << endl;
    */
	
    // [Matrice A] Décalage vers la gauche, calcul du processus à qui envoyer et celui de qui reçevoir la matrice
	// Besoin de les calculer une seul fois vu qu'on envoie et reçois toujours du même processus
    int destA = ((myPE + (q-1)) % q)+(q*lin);
    int sourceA = ((myPE + 1) % q)+(q*lin);
    
    //cout << "[Processus : " << myPE << "] destA : " << destA << endl;
    //cout << "[Processus : " << myPE << "] sourceA : " << sourceA << endl;
	
	// [Matrice B] Décalage vers le haut, calcul du processus à qui envoyer et celui de qui reçevoir la matrice
    int destB = (myPE + (q - 1) * q) % (q * q);
    int sourceB = (myPE + q) % (q * q);
    
    //cout << "[Processus : " << myPE << "] destB : " << destB << endl;
    //cout << "[Processus : " << myPE << "] sourceB : " << sourceB << endl;

    // Copie des matrices en local
    int* My_matLocA = new int[nloc * nloc];
	int* My_matLocB = new int[nloc * nloc];
    for (int i = 0; i < q * q; i++) {
        My_matLocA[i] = matLocA[i];
		My_matLocB[i] = matLocB[i];
    }
    
    //Pour moi :
    // - My_matLocA = T
    // - My_matLocB = S
	
	// [Matrice A] Décalage à gauche de la ligne i de i position
	// Dernière ligne ne bouge pas (+1 pour aligner lin au vrai numero de ligne 
	// (si matrice à 3 lignes lin max = 2, car lin min = 0))
	if ((lin+1) != q){	
		int destdecalA = ((myPE + (q-(lin+1))) % q)+(q*lin);
		int sourcedecalA = ((myPE + (lin+1)) % q)+(q*lin);
		//cout << "[Processus : " << myPE << "] destdecalA : " << destdecalA << endl;
		//cout << "[Processus : " << myPE << "] sourcedecalA : " << sourcedecalA << endl;
		// Envoie ça matrice de i position (numéro de ligne) vers la gauche
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destdecalA , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourcedecalA, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	// [Matrice B] Décalage en haut de la colonne j de j position
	if((col+1) != q){	//Dernière colonne ne bouge pas
		int destdecalB = (myPE + (q - (col+1)) * q) % (q * q);
		int sourcedecalB = (myPE + (q*(col+1))) % (q * q);
		//cout << "[Processus : " << myPE << "] destdecalB : " << destdecalB << endl;
		//cout << "[Processus : " << myPE << "] sourcedecalB : " << sourcedecalB << endl;
		// Envoie ça matrice de j position (numéro de colonne) vers le haut
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destdecalB , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourcedecalB, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	// Multiplication des matrice A et B dans C
    //multMatrix(My_matLocA, nloc, nloc, My_matLocB, nloc, nloc, matLocC);

    for (int k = 1; k < q; k++) {
    	//cout << "[Processus : " << myPE << "] Test boucle"<< endl;
    	
		// Multiplication des matrice A et B dans C
        // multMatrix(My_matLocA, nloc, nloc, My_matLocB, nloc, nloc, matLocC);

        // [Matrice A alias T] Décalage à gauche de une position
		// Envoie ça matrice à la destination
		MPI_Send(My_matLocA, nloc * nloc, MPI_INT, destA , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocA, nloc * nloc, MPI_INT, sourceA, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
		// [Matrice B alisa S] Décalage vers le haut de une position
		// Envoie ça matrice à la destination
		MPI_Send(My_matLocB, nloc * nloc, MPI_INT, destB , 0, MPI_COMM_WORLD);
		// Reçois ça matrice de ça source
		MPI_Recv(My_matLocB, nloc * nloc, MPI_INT, sourceB, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// Multiplication des matrice A et B dans C
        multMatrix(My_matLocA, nloc, nloc, My_matLocB, nloc, nloc, matLocC);
    }
    
	//Supression des matrices local
    delete[] My_matLocA;
	delete[] My_matLocB;
}
