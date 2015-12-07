#ifndef __FOX__
#define __FOX__

/**
 * @brief Fox algorithm
 *
 * step -> sqrt(nbPE)
 * For each step, broadcast matrix A
 * Send matrix B to the previous row (same column) except for the first step
 * The destination and source are the same for each step because the matrix 
 * we receive replace the current one.
 *
 * Matrix A and B must not be modified, because there are not saved 
 * before the function call
 *
 * matLocC is already initialized with zeros
 *
 * @param matLocA   Matrix A
 * @param matLocB   Matrix B
 * @param matLocC   Matrix with results
 * @param nloc      Matrix is of size nloc * nloc
 */
void fox(int* matLocA, int* matLocB, int* matLocC, int nloc);

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
void multMatrix(int* matA, int nRowA, int nColA, int* matB, int nRowB, int nColB, int*& matC);

#endif
