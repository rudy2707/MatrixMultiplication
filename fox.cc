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

    // Source and destionation for sending B (circular shift)
    // TODO : nloc or q ?
    int dest = (myPE + (q - 1) * q) % (q * q);  // TODO : OK si myPE est global
    int source = (myPE + q) % (q * q);             // TODO : OK si myPE est global
    //source = (i + 1) % q;
    //dest = (i + q - 1) % q;

    MPI_Comm_split(MPI_COMM_WORLD, myPE / nloc, myPE, &commRow); // TODO : GROUP OK (myPE / nloc)

    // TODO : Row and column OK
    cout << "%[" << myPE << "] Init i = " << i << " / j = " << j << endl;

    //int* matLocT = new int[nloc * nloc];
    for (int step = 0; step < q; step++) {
        // Calculate k : i + step, to avoid doing the addition each time
        int k = (i + step) % q; // TODO : KO ! on attend pas la valeur sur le bon processor
        cout << "%[" << myPE << "] Step = " << step << " / k = " << k << endl;
        // MATRIX T
        // Broadcast A(i, i+step) to process on row i (commRow) if (i + step) mod q == column
        // Copy of matrix A to avoid modification
        int* matLocT = new int[nloc * nloc];
        for (int z = 0; z < nloc * nloc; z++)
            matLocT[z] = matLocA[z];

        // Sender is (k + i * q) mod q
        int sender = (i + step) % q;

        if (j == k) {
            cout << "%[" << myPE << "] Broadcast A : i = " << i << " / j = " << j << " / k =  " << k << " from " << sender << " (step=" << step << ")" << endl;
            // WRONG value
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Bcast(matLocT, nloc * nloc , MPI_INT, sender, commRow); // TODO : matrix a est modif => ko
            cout << "%[" << myPE << "] After broadcast A : i = " << i << " / j = " << j << " / k =  " << k << " from " << sender << endl;
            //matLocT = matLocA;
        }
        else {
            //matLocT = new int[nloc * nloc];
            cout << "%[" << myPE << "] Wait for T from " << sender << endl;
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Bcast(matLocT, nloc * nloc , MPI_INT, sender, commRow);
            cout << "%[" << myPE << "] Receive T from " << sender << endl;
        }

        // MATRIX B
        // The matrixB is changed at every step and send to the previous row
        // So the source and the dest are each time the same
        if (step != 0) {
            //matLocS = new int[nloc * nloc];
            // Send matrix B to dest
            cout << "%[" << myPE << "] Send S to " << dest << endl;
            cout << "%[" << myPE << "] S i =  " << i << " / j = " << j << " / step = " << step << endl;
            MPI_Send(matLocB, nloc * nloc, MPI_INT, dest , 0, MPI_COMM_WORLD);
            // Receive matrix B from source
            cout << "%[" << myPE << "] Wait for S from " << source << endl;
            MPI_Recv(matLocB, nloc * nloc, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //MPI_Recv(matLocB, nloc * nloc, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "%[" << myPE << "] Received S from " << source << endl;
        }

        // Multiplication of A(i, i+step) with B(i+step, j)
        // Respectively, T and B
        //int* matTemp = multMatrix(matLocT, nloc, nloc, matLocB, nloc, nloc);
        //cout << "[" << myPE << "] Before mult" << endl;

        //int* matTemp = new int[nloc * nloc];
        for (int i = 0; i <= nloc; i++) {
            for (int j = 0; j < nloc; j++) {
                int sum = 0;
                for (int k = 0; k < nloc; k++)
                    sum = sum + matLocT[i * nloc + k] * matLocB[k * nloc + j];
                //matTemp[i * nloc + j] = sum;
                matLocC[i * nloc + j] += sum;
            }
        }

        //cout << "[" << myPE << "] After mult" << endl;
        cout << "%[" << myPE << "] Before add" << endl;
        // Add the result to C(i, j)
        //for (int l = 0; l < nloc * nloc; l++ )
        //    matLocC[l] += matTemp[l];
        //multMatrix(matLocT, nloc, nloc, matLocB, nloc, nloc, matLocC);
        cout << "%[" << myPE << "] After add" << endl;

        delete[] matLocT;
        //delete[] matTemp;
    }
        //delete[] matLocT;
}

void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matRes) {
    for (int i = 0; i <= nRowA; i++) {
        for (int j = 0; j < nColB; j++) {
            int sum = 0;
            for (int k = 0; k < nRowB; k++)
                sum = sum + matA[i * nColA + k] * matB[k * nColB + j];
            matRes[i * nColB + j] = sum;
        }
    }
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






