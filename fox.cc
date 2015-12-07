/*
 * =====================================================================================
 *
 *       Filename:  fox.cc
 *
 *    Description:  Implementation of Fox Algorithm
 *
 *        Version:  1.0
 *        Created:  11/23/2015 01:28:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rudolf Hohn, Axel Fahy, Jessy Marin
 *   Organization:  HES-SO hepia section ITI
 *
 * =====================================================================================
 */

#include "fox.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <mpi.h>
#include <cstring>

using namespace std;

/**
 * @brief
 *
 * matLocC is already initialized with zeros
 *
 * @param matLocA
 * @param matLocB
 * @param matLocC
 * @param nloc
 */
void fox(int* matLocA, int* matLocB, int* matLocC, int nloc) {
    int nbPE, myPE;
    MPI_Comm_size(MPI_COMM_WORLD, &nbPE);
    MPI_Comm_rank(MPI_COMM_WORLD, &myPE);
    MPI_Comm commRow;
    MPI_Comm commCol;

    int q = sqrt(nbPE);

    cout << "%q = " << q << endl;

    // Find position of myPE in the matrix
    int row = myPE / q;    // Row
    int col = myPE % q;    // Column

    // Source and destionation for sending B (circular shift)
    int dest = (myPE + (q - 1) * q) % (q * q);
    int source = (myPE + q) % (q * q);
    //to = row + q - k
    //int source = (row + 1) % q;
    //int dest = (row + q - 1) % q;

    MPI_Comm_split(MPI_COMM_WORLD, row, myPE, &commRow); // TODO : GROUP OK (myPE / nloc)
    MPI_Comm_split(MPI_COMM_WORLD, col, myPE, &commCol);

    int rowPE;
    int colPE;
    MPI_Comm_rank(commRow, &rowPE);
    MPI_Comm_rank(commCol, &colPE);

    // TODO : Row and column OK
    cout << "%[" << myPE << "(" << rowPE << ")] Init i = " << row << " / j = " << col << endl;

    // Temporary matrix to get the matrix from broadcast
    int* matLocT = new int[nloc * nloc];

    for (int step = 0; step < q; step++) {
        // Calculate k : row + step, to avoid doing the addition each time
        // k is the sender of matrix A
        int k = (row + step) % q;

        // MATRIX T
        // Broadcast A(i, i+step) to process on row i (commRow) if (row + step) mod q == column
        if (col == k) {
            //cout << "%[" << myPE << "(" << rowPE << ")] Broadcast A from " << k << " (step=" << step << ") : "  << matLocA[0] << endl;
            //MPI_Barrier(MPI_COMM_WORLD);
            //MPI_Bcast(matLocA, nloc * nloc , MPI_INT, k, commRow); // TODO : matrix a est modif => ko
            // Version broadcast manuel
            for (int i = 0; i < q; i++) {
                int destT = row * q + i;
                if (destT != myPE) {
                    MPI_Send(matLocA, nloc * nloc, MPI_INT, destT , 0, MPI_COMM_WORLD);
                    cout << "%[" << myPE << "(" << rowPE << ")] Send A from " << myPE << " (step=" << step << ") to " << destT<< " : "  << matLocA[0] << endl;
                }
            }
            //cout << "%[" << myPE << "] After broadcast A : i = " << row << " / j = " << col << " / k =  " << k << " from " << sender << endl;
            //matLocT = matLocA;
        }
        else {
            //matLocT = new int[nloc * nloc];
            //MPI_Barrier(MPI_COMM_WORLD);
            int sourceT = row * q + k;
            MPI_Recv(matLocT, nloc * nloc, MPI_INT, sourceT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "%[" << myPE << "(" << rowPE << ")] Received A from " << sourceT << " (step=" << step << ") : " << matLocT[0] << endl;
            //MPI_Bcast(matLocT, nloc * nloc , MPI_INT, k, commRow);
            //cout << "%[" << myPE << "(" << rowPE << ")] Received A from " << k << " (step=" << step << ") : " << matLocT[0] << endl;
        }

        // MATRIX B
        // The matrixB is modified at every step and send to the previous row
        // So the source and the dest are each time the same
        if (step != 0) {
            // Send matrix B to dest
            cout << "%[" << myPE << "(" << colPE << ")] Send B to " << dest << " (step=" << step << ")" <<  " : " << matLocB[0] << endl;
            MPI_Send(matLocB, nloc * nloc, MPI_INT, dest , 0, MPI_COMM_WORLD);
            // Receive matrix B from source
            MPI_Recv(matLocB, nloc * nloc, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "%[" << myPE << "(" << colPE << ")] Received B from " << source << " (step=" << step << ")" <<  " : " << matLocB[0] <<  endl;
            //MPI_Sendrecv_replace(matLocB, nloc * nloc, MPI_INT,
            //    dest, 0, source, 0, commCol, MPI_STATUS_IGNORE);
        }

        // Multiplication of A(i, i+step) with B(i+step, j)
        if (col == k) {
            //multMatrix(matLocA, nloc, nloc, matLocB, nloc, nloc, matLocC);
            for (int i = 0; i < nloc; i++) {
                for (int j = 0; j < nloc; j++) {
                    for (int k = 0; k < nloc; k++) {
                        matLocC[i * nloc + j] += matLocA[i * nloc + k] * matLocB[k * nloc + j];
                    }
                }
            }
        }
        else {
            //multMatrix(matLocT, nloc, nloc, matLocB, nloc, nloc, matLocC);
            for (int i = 0; i < nloc; i++) {
                for (int j = 0; j < nloc; j++) {
                    for (int k = 0; k < nloc; k++) {
                        matLocC[i * nloc + j] += matLocT[i * nloc + k] * matLocB[k * nloc + j];
                    }
                }
            }
        }
        cout << "%[" << myPE << "(" << rowPE << ")] Addition done" << endl;
    }
    delete[] matLocT;
}

void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matRes) {
    for (int i = 0; i < nRowA; i++) {
        for (int j = 0; j < nColB; j++) {
            for (int k = 0; k < nRowB; k++) {
                matRes[i * nColB + j] += matA[i * nColA + k] * matB[k * nColB + j];
            }
        }
    }
}

