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

/**
 * @brief Multiplication of two matrix
 *
 * The result is added into matRes
 *
 * Size of matrix Res is calculated in the function
 *
 * @param matA      Matrix A
 * @param nRowA     Number of row A
 * @param nColA     Number of col A
 * @param matB      Matrix B
 * @param nRowB     Number of row B
 * @param nColB     Number of col B
 * @param matRes    Matrix for the result
 */
void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matRes) {
    for (int i = 0; i < nRowA; i++) {
        for (int j = 0; j < nColB; j++) {
            for (int k = 0; k < nRowB; k++) {
                matRes[i * nColB + j] += matA[i * nColA + k] * matB[k * nColB + j];
            }
        }
    }
}

