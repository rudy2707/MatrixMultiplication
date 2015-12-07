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

#include "dns.h"
#include "fox.h" // To multiply the matrix

void dns(int* matLocA, int* matLocB, int* matLocC, int nloc) { 
    // Init of MPI
    MPI_Init(&argc,&argv);
    int nbPE,myPE;
    MPI_Comm_size(MPI_COMM_WORLD,&nbPE);
    MPI_Comm_rank(MPI_COMM_WORLD,&myPE);
    
    // Making the communications channels
    MPI_Comm comm_a, comm_b, comm_c;
    MPI_Comm_split(MPI_COMM_WORLD, myPE%3+((myPE/9)*9), myPE, &comm_a);
    MPI_Comm_split(MPI_COMM_WORLD, myPE%9, myPE, &comm_b);
    MPI_Comm_split(MPI_COMM_WORLD, myPE/3, myPE, &comm_c);
    
    // Broadcasting the A and B matrix
    MPI_Bcast(matLocA, nloc*nloc, MPI_INT, 0, comm_a);
    MPI_Bcast(matLocB, nloc*nloc, MPI_INT, 0, comm_b);

    // Multiplication local of the matrix
    multMatrix(matLocA, nloc, nloc, matLocB, nloc, nloc, matLocC);

    // Reducing the results to the C matrix
    MPI_Reduce(matLocC, matLocC, nloc*nloc, MPI_INT, MPI_SUM, 0, comm_c);
}
