/*
 * =====================================================================================
 *
 *       Filename:  dns.cc
 *
 *    Description:  Implementation of DNS algorithm
 *
 *        Version:  1.0
 *        Created:  11/23/2015 01:35:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rudolf Hohn (), rudolf.hohn@etu.hesge.ch
 *   Organization:  HES-SO hepia section ITI
 *
 * =====================================================================================
 */

#include <mpi.h>
#include "dns.h"
#include <cmath>
#include "fox.h" // To multiply the matrix

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
