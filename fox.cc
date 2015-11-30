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

    int q = sqrt(nbPE);

    // Find position of myPE in the matrix
    int i = myPE / nloc;    // Row
    int j = myPE % nloc;    // Column

    // Source and destionation for sending B
    int dest = (myPE + (nloc - 1) * nloc) % (nloc * nloc);
    int source = (myPE + nloc) % (nloc * nloc);

    MPI_Comm_split(MPI_COMM_WORLD, myPE / nloc, myPE, &commRow); // TODO : GROUP OK (myPE / nloc)

    // TODO : Row and column OK
    cout << "[" << myPE << "] i = " << i << " / j = " << j << endl;

    for (int step = 0; step < q; step++) {
        // Calculate k : i + step, to avoid doing the addition each time
        int k = (i + step) % q;
        cout << "[" << myPE << "] k = " << k << endl;
        // MATRIX T
        // Broadcast A(i, i+step) to process on row i (commRow) if (i + step) mod q == column
        // Sender is k
        // Copy of matrix A to avoid modification
        int* matLocT = new int[nloc * nloc];
        for (int z = 0; z < nloc * nloc; z++)
            matLocT[z] = matLocA[z];
        if (j == k) {
            cout << "[" << myPE << "] Broadcast A : i = " << i << " / j = " << j << " / k =  " << k << endl;
            MPI_Bcast(matLocT, nloc * nloc , MPI_INT, k, commRow); // TODO : matrix a est modif => ko
            cout << "[" << myPE << "] After broadcast A : i = " << i << " / j = " << j << " / k =  " << k << endl;
        }
        else {
            //matLocT = new int[nloc * nloc];
            cout << "[" << myPE << "] Wait for T " << endl;
            MPI_Bcast(matLocT, nloc * nloc , MPI_INT, k, commRow);
            cout << "[" << myPE << "] Receive T " << endl;
        }

        // MATRIX B
        // The matrixB is changed at every step and send to the previous row
        // So the source and the dest are each time the same
        if (step != 0) {
            //matLocS = new int[nloc * nloc];
            // Send matrix B to dest
            cout << "[" << myPE << "] Send S to " << dest << endl;
            cout << "[" << myPE << "] i =  " << i << " / j = " << j << " / step = " << step << endl;
            MPI_Send(matLocB, nloc * nloc, MPI_INT, dest , 0, MPI_COMM_WORLD);
            // Receive matrix B from source)
            cout << "[" << myPE << "] Wait for S from " << source << endl;
            MPI_Recv(matLocB, nloc * nloc, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "[" << myPE << "] Received S from " << source << endl;
        }

        // Multiplication of A(i, i+step) with B(i+step, j)
        // Respectively, T and B
        int* matTemp = multMatrix(matLocT, nloc, nloc, matLocB, nloc, nloc);
        // Add the result to C(i, j)
        for (int l = 0; l < nloc * nloc; l++ )
            matLocC[l] += matTemp[l];

        //delete[] matLocT;
        //delete[] matTemp;
    }
}

int* multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB) {
    int* res = new int[nRowA * nColB];
    for (int i = 0; i <= nRowA; i++) {
        for (int j = 0; j < nColB; j++) {
            int sum = 0;
            for (int k = 0; k < nRowB; k++)
                sum = sum + matA[i * nColA + k] * matB[k * nColB + j];
            res[i * nColB + j] = sum;
        }

    }
    //for (int i = 0; i < nA * nloc; k++) {
    //    int i = (k * nloc) % (nloc - 1);
    //    res[k] = matLocA[k] * matLocB[i];
    //    cout << matLocA[k] << " * " << matLocB[i] << " = " << res[k] << endl;
    //}
    return res;
}


//void SampleUtils::multiplyMatrices(float* matA, int rA, int cA, float* matB,
//        int rB, int cB, float* matC, int rC, int cC) {
//    for (int i = 0; i <= rA; i++) {
//        for (int j = 0; j <= cB; j++) {
//            float sum = 0.0;
//            for (int k = 0; k <= rB; k++)
//                sum = sum + matA[i * cA + k] * matB[k * cB + j];
//            matC[i * cC + j] = sum;
//        }
//
//    }






