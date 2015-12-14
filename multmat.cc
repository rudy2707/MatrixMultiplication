#include <iostream>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <mpi.h>
#include <cstring>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matRes);
/*
Algorithmes de multiplication matricielle parallèle
*/
void fox(int* matLocA,int* matLocB,int* matLocC,int nloc);
void cannon(int* matLocA,int* matLocB,int* matLocC,int nloc);
void dns(int* matLocA,int* matLocB,int* matLocC,int nloc);
////////////////////////////////////////////////////////////////////////////////

/*
Les sous-matrices <mat> de dimension <nloc>x<nloc> sur les <nbPE> processeurs
du groupe de communication <comm> sont rassemblées sur le processeur 0
qui les affiche en une grande matrice de dimension (<p>*<nloc>) x (<p>*<nloc>)
*/
void printAll(int* mat,int nloc,MPI_Comm comm,string label) {
   int nbPE,globalPE;
   MPI_Comm_size(comm,&nbPE);
   MPI_Comm_rank(MPI_COMM_WORLD,&globalPE);
   int* recv_buf = new int[nbPE*nloc*nloc];
   MPI_Gather(mat,nloc*nloc,MPI_INT,recv_buf,nloc*nloc,MPI_INT,0,comm);
   if (globalPE == 0) {
      int p = sqrt(nbPE+0.1);
      cout << label;
      for (int global=0;global<(p*nloc)*(p*nloc);global++) {
         int global_i = global/(p*nloc); int global_j = global%(p*nloc);
         int pe_i = global_i/nloc; int pe_j = global_j/nloc;
         int local_i = global_i%nloc; int local_j = global_j%nloc;
         int pe = pe_i*p+pe_j; int local = local_i*nloc+local_j;
         cout << recv_buf[pe*nloc*nloc + local] << " ";
         if ((global+1)%(p*nloc) == 0) cout << endl;
      }
   }
   delete recv_buf;
}
/*
Allocation et initialisation aléatoire d'une matrice de dimension <size>x<size>
avec des valeurs entières entre <inf> et <sup>; cette matrice est retournée
*/
int* randomInit(int size,int inf,int sup) {
   int* mat = new int[size*size];
   for (int i=0;i<size*size;i++) mat[i] = inf+rand()%(sup-inf);
   return mat;
}


int main(int argc,char** argv) {
   MPI_Init(&argc,&argv);
   int nbPE,myPE;
   MPI_Comm_size(MPI_COMM_WORLD,&nbPE);
   MPI_Comm_rank(MPI_COMM_WORLD,&myPE);
   MPI_Comm comm_i_cte = MPI_COMM_WORLD,
            comm_j_cte = MPI_COMM_WORLD,
            comm_k_cte = MPI_COMM_WORLD;

   int algo = atoi(argv[1]); // 1 = fox, 2 = cannon, 3 = dns
   srand(atoi(argv[2])+myPE);
   int nloc = atoi(argv[3]);

   // sous-matrices de dimension <nloc> x <nloc>
   // des matrices A, B et C de dimension (<p>*<nloc>) x (<p>*<nloc>)
   int* matLocA = randomInit(nloc,-10,10);
   int* matLocB = randomInit(nloc,-10,10);
   int* matLocC = randomInit(nloc, 0, 1); //new int[nloc*nloc];

   switch(algo) {
   case 1: fox(matLocA,matLocB,matLocC,nloc); break;
   case 2: cannon(matLocA,matLocB,matLocC,nloc); break;
   case 3: // p^3 processeurs P_{ijk} avec i,j,k = 0,...,p-1
      int p = pow(nbPE+0.1,1.0/3.0);
      int j = (myPE/p)%p;  // A_{ik} sur les P_{i0k}
      MPI_Comm_split(MPI_COMM_WORLD,j,myPE,&comm_j_cte);
      int i = myPE/(p*p); // B_{kj} sur les P_{0jk}
      MPI_Comm_split(MPI_COMM_WORLD,i,(myPE%p)*p+myPE/p,&comm_i_cte);
      int k = myPE%p;      // C_{ij} sur les P_{ij0}
      MPI_Comm_split(MPI_COMM_WORLD,k,myPE,&comm_k_cte);
      dns(matLocA,matLocB,matLocC,nloc);
   }
   printAll(matLocA,nloc,comm_j_cte,"%matrice complete A\n");
   printAll(matLocB,nloc,comm_i_cte,"%matrice complete B\n");
   printAll(matLocC,nloc,comm_k_cte,"%matrice complete C\n");

   MPI_Finalize();
   delete matLocA, matLocB, matLocC;
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void fox(int* matLocA, int* matLocB, int* matLocC, int nloc) {
    int nbPE, myPE;
    MPI_Comm_size(MPI_COMM_WORLD, &nbPE);
    MPI_Comm_rank(MPI_COMM_WORLD, &myPE);
    MPI_Comm commRow;

    int q = sqrt(nbPE);

    // Find position of myPE in the matrix
    int row = myPE / q;    // Row
    int col = myPE % q;    // Column

    // Source and destionation for sending B (circular shift)
    // It's always the same dest and source because we replace the S matrix with
    // the one we just received
    int dest = (myPE + (q - 1) * q) % (q * q);
    int source = (myPE + q) % (q * q);

    MPI_Comm_split(MPI_COMM_WORLD, row, myPE, &commRow);

    // Warning : MatrixB is not saved in multmat, so we must not modify it !
    int* matLocS = new int[nloc * nloc];
    for (int i = 0; i < q * q; i++) {
        matLocS[i] = matLocB[i];
    }

    for (int step = 0; step < q; step++) {
        // Temporary matrix to get the matrix from broadcast
        int* matLocT = new int[nloc * nloc];
        for (int i = 0; i < q * q; i++) {
            matLocT[i] = matLocA[i];
        }
        // Calculate k : row + step, to avoid doing the addition each time
        // k is the sender of matrix T
        int k = (row + step) % q;

        // MATRIX T
        // Broadcast A(i, i+step) = S to process on row (commRow) if (row + step) mod q == column
        MPI_Bcast(matLocT, nloc * nloc , MPI_INT, k, commRow);

        // MATRIX B
        // The matrixB is modified at every step and send to the previous row
        // So the source and the dest are each time the same
        if (step > 0) {
            // Send matrix B to dest
            MPI_Send(matLocS, nloc * nloc, MPI_INT, dest , 0, MPI_COMM_WORLD);
            // Receive matrix B from source
            MPI_Recv(matLocS, nloc * nloc, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Multiplication of A(i, i+step) = T with B(i+step, j) = S
        multMatrix(matLocT, nloc, nloc, matLocS, nloc, nloc, matLocC);
        delete[] matLocT;
    }
    delete[] matLocS;
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void dns(int* matLocA, int* matLocB, int* matLocC, int nloc) {
    // Init of MPI
    int nbPE,myPE;
    MPI_Comm_size(MPI_COMM_WORLD,&nbPE);
    MPI_Comm_rank(MPI_COMM_WORLD,&myPE);

    // Variables for the size of the matrix
    int p = pow(nbPE + 0.1, 1.0 / 3.0);

    // Making the communications channels
    MPI_Comm comm_a, comm_b, comm_c;
    MPI_Comm_split(MPI_COMM_WORLD, myPE%p+((myPE/(p*p))*(p*p)), myPE, &comm_a);
    MPI_Comm_split(MPI_COMM_WORLD, myPE%(p*p), myPE, &comm_b);
    MPI_Comm_split(MPI_COMM_WORLD, myPE/p, myPE, &comm_c);

    // Broadcasting the A and B matrix
    MPI_Bcast(matLocA, nloc*nloc, MPI_INT, 0, comm_a);
    MPI_Bcast(matLocB, nloc*nloc, MPI_INT, 0, comm_b);

    // Multiplication local of the matrix
    multMatrix(matLocA, nloc, nloc, matLocB, nloc, nloc, matLocC);

    // Reducing the results to the C matrix
    int* result = new int[nloc * nloc];
    MPI_Reduce(matLocC, result, nloc*nloc, MPI_INT, MPI_SUM, 0, comm_c);

    // Copy the result in matLocC
    for (int i = 0; i < nloc*nloc; i++) matLocC[i] = result[i];
}

////////////////////////////////////////////////////////////////////////////////
void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matRes) {
    for (int i = 0; i < nRowA; i++) {
        for (int j = 0; j < nColB; j++) {
            for (int k = 0; k < nRowB; k++) {
                matRes[i * nColB + j] += matA[i * nColA + k] * matB[k * nColB + j];
            }
        }
    }
}

/*
Exemple pour l'algorithme DNS avec p=3 (p^3 = 27 processeurs, pe = 0,1,...,p^3-1)

                              j
                             ^
                            /
                           /----->
                           |    k
                         i |           j
                           v          ^
                                     /
                                    /
                  /|               /--------------------------/|
                 / | ...          /        /        /        / |
                /  |             /  B02   /  B12   /  B22   /  |
               /   |            /        /        /        /   |
              /|C02|           /--------------------------/|   |
             / |  /|          /        /        /        / |  /|
            /  | / | ...     /  B01   /  B11   /  B21   /  | / |
           /   |/  |        /        /        /        /   |/  |
          /|C01|C12|       /--------------------------/|   |   |
         / |  /|  /|      /        /        /        / |  /|  /|
        /  | / | / | ... /  B00   /  B10   /  B20   /  | / | / |
       /   |/  |/  |    /        /        /        /   |/  |/  |
      /C00 |C11|C22/   /----------------------------> k|   |   /
      |   /|  /|  /    |        |        |        |   /|  /|  /
      |  / | / | / ... |  A00   |  A01   |  A02   |  / | / | /
      | /  |/  |/      |        |        |        | /  |/  |/
      |/C10|C21/       |--------|--------|--------|/   |   /
      |   /|  /        |        |        |        |   /|  /
      |  / | /  ...    |  A10   |  A11   |  A12   |  / | /
      | /  |/          |        |        |        | /  |/
      |/C20|           |--------|--------|--------|/   |
      |   /            |        |        |        |   /
      |  /  ...        |  A20   |  A21   |  A22   |  /
      | /              |        |        |        | /
      |/               |--------|--------|--------|/
                       |
                       |
                       v

                       i

Initialement, Aik est sur le processeur P_{i0k} et Bkj est sur P_{0jk}
Broadcast de Aik dans la direction j par P_{i0k} aux p processeurs P_{ijk}
Broadcast de Bkj dans la direction i par P_{0jk} aux p processeurs P_{ijk}
Multiplication Aik*Bkj par le processeur P_{ijk}
Réduction dans la direction k en rapatriant et sommant les Aik*Bkj:
Cij = Sum_{k=0}^{p-1} Aik*Bkj sur P_{ij0}

=======================================================
i = pe/9

i=0
     6--7--8
    /| /| /|
   3--4--5
  /| /| /|
 0--1--2
 |  |  |

i=1
      |   |   |
     15--16--17
    |/  |/ | /|
   12--13--14 |
  |/  |/  |/| /
  9--10--11 |/
  |   |   | /
  |___|___|/

i=2
      |   |   |
     24--25--26
    |/  |/ | /
   21--22--23
  |/  |/  |/
 18--19--20

=======================================================
j = (pe/3)%3

j=0

   /   /   /
  0---1---2
  |/  |/  |/
  9--10--11
  |/  |/  |/
 18--19--20

j=1

   /   /   /
  3---4---5
 /|/  |/  |/
 12--13--14
 /|/  |/  |/
 21--22--23
 /   /   /

j=2

  6---7---8
 /|  /|  /|
 15--16--17
 /|  /|  /|
 24--25--26
 /   /   /

=======================================================
k = pe%3

k=0
         6---
        /|
       / |
      3- 15--
     /| /|
    / |/ |
   0- 12 24--
   | /| /
   |/ |/
   9- 21--
   | /
   |/
   18--

k=1
         ---7---
           /|
          / |
      ---4- 16--
        /| /|
       / |/ |
   ---1- 13 25--
      | /| /
      |/ |/
   --10 -22--
      | /
      |/
   --19--

k=2
            ---8
              /|
             / |
         ---5  17
           /| /|
          / |/ |
      ---2  14 26
         | /| /
         |/ |/
      --11 -23
         | /
         |/
      --20

===========================================================
Exemple avec p = 3

    	0	1	2	3	4	5	6	7	8
---------------------------------
i=0:	0	1	2	3	4	5	6	7	8	    i = pe/(p*p)
i=1:	9	10	11	12	13	14	15	16	17
i=2:	18	19	20	21	22	23	24	25	26
      j*p+k = pe%(p*p)

    	0	1	2	3	4	5	6	7	8
---------------------------------
j=0:	0	1	2	9	10	11	18	19	20	    j = (pe/p)%p ou j = (pe%(p*p))/p
j=1:	3	4	5	12	13	14	21	22	23
j=2:	6	7	8	15	16	17	24	25	26
      i*p+k = (pe/(p*p))*p+pe%p

    	0	1	2	3	4	5	6	7	8
---------------------------------
k=0:	0	3	6	9	12	15	18	21	24	    k = pe%p
k=1:	1	4	7	10	13	16	19	22	25
k=2:	2	5	8	11	14	17	20	23	26
      i*p+j = pe/p

*/
