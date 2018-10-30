#include "matrix.h"

using std::vector;

void swapRows (int matrixSize, int row1, int row2,
               vector2d& currentMatrix, vector2d& inverseMatrix);

void addMultipleOfRow (int matrixSize, int rowToChange, int rowAddFrom, fraction multiple,
                       vector2d& currentMatrix, vector2d& inverseMatrix);

void multiply (int matrixSize, int row, fraction multiplication,
               vector2d& currentMatrix, vector2d& inverseMatrix);


//This file contains a function that computes the inverse of an N by N matrix
//It contains the additional function for multiplying two a square matrix with a column matrix of the same column size

void squareColumnMultiplication(int matrixSize, vector2d matrix1,
                                vector<fraction> matrix2, vector<fraction>& resultMatrix){



    /**
        The function assumes all 3 vectors are of appropriate sizes and that they've all been initialised

        When multiplying two matrices, each element of each row of the first matrix,
        is multiplied by each element of the corresponding column of the second matrix
    **/


    for (int i = 0; i< matrixSize; i++){

        fraction result(0,1);

        for (int j = 0; j < matrixSize; j++){

            result += matrix1[i][j]*matrix2[j];
        }

        resultMatrix[i] = result;
    }

}

//Maybe change this so that if it returns false, it returns the original untouched vector of inverseMatrix
bool inverse (int matrixSize, vector2d currentMatrix, vector2d& inverseMatrix){

    //Function places the inverse of currentMatrix into inverseMatrix
    //It will return false if the N by N matrix does not have an inverse
    //If will also return False if the vectors provided are not of size matrixSize by matrixSize
    //Note that if the function returns false while it's already started operations, inverseMatrix will not be reverted back into its original form

    int i,j;

    //A fraction object with numerator and denominator of 1
    fraction one (1,1);
    //Fraction object with numerator of 0. It doesn't matter what the denominator is, as long as it's also not 0
    fraction zero (0,1);

    if (matrixSize <= 1)
        return false;

    if (currentMatrix.size() != matrixSize || inverseMatrix.size() != matrixSize)
        return false;

    //Ensures the inner vectors (or -abstractly- each row in the matrix) is the size of matrixSize
    //Additionally sets up the identity matrix at the same time
    for (i = 0; i<matrixSize;i++){
        if (currentMatrix[i].size() != matrixSize || inverseMatrix[i].size() != matrixSize)
            return false;

        //Identity matrix has 1s along the diagonals
        inverseMatrix[i][i] = one;

        //Set every other position in the current row as a 0
        for (j = 0; j < matrixSize;j++){
            if (i == j)
                continue;

            inverseMatrix[i][j] = zero;
        }

    }

    //Loop through every row
    for (i = 0; i < matrixSize; i++){


        //If diagonal has a current value of 0, we need to swap with another row
        if (currentMatrix[i][i] == 0){
            bool foundSwap = false;
            //If i>0 we don't want to swap current row with any previous rows, as this will disrupt the diagonal of 1s of the identity matrix
            for (j = i+1; j<matrixSize; j++){

                //Swap rows, so current diagonal has a non-zero number
                if (currentMatrix[j][i] != 0){
                    swapRows (matrixSize, i, j, currentMatrix, inverseMatrix);
                    foundSwap = true;
                    break;
                }
            }

            //Couldn't get a non-zero number in current diagonal, therefore matrix can't be inverted
            if (!foundSwap)
                return false;
        }

        fraction diagonal_val = currentMatrix[i][i];
        //Make current diagonal equal to 1
        if (diagonal_val != 1)
            multiply (matrixSize,i,one/diagonal_val,currentMatrix,inverseMatrix);

        //Now, the every other value in the column the diagonal is in must be made into a zero
        //This is done by subtracting each value by itself*current row
        diagonal_val = currentMatrix[i][i];
        for (j = 0; j < matrixSize; j++){
            if (j == i)
                continue;
            //Already a zero so no need to change it
            if (currentMatrix[j][i] == 0)
                continue;

            addMultipleOfRow (matrixSize,j,i,currentMatrix[j][i]*-1,currentMatrix,inverseMatrix);

        }


    }

    return true;

}

void swapRows (int matrixSize, int row1, int row2,
               vector2d& currentMatrix, vector2d& inverseMatrix){

    for (int i = 0; i<matrixSize; i++){

        //Need to store one current values for corresponding rows as one will be lost
        fraction temp = currentMatrix[row1][i];
        fraction temp2 = inverseMatrix[row1][i];

        currentMatrix[row1][i] = currentMatrix[row2][i];
        currentMatrix[row2][i] = temp;

        inverseMatrix[row1][i] = inverseMatrix[row2][i];
        inverseMatrix[row2][i] = temp2;
    }

}

void addMultipleOfRow (int matrixSize, int rowToChange, int rowAddFrom, fraction multiple,
                       vector2d& currentMatrix, vector2d& inverseMatrix){

    for (int i = 0; i<matrixSize;i++){

        currentMatrix[rowToChange][i] += (currentMatrix[rowAddFrom][i]*multiple);
        inverseMatrix[rowToChange][i] += (inverseMatrix[rowAddFrom][i]*multiple);
    }
}

void multiply (int matrixSize, int row, fraction multiplication,
               vector2d& currentMatrix, vector2d& inverseMatrix){

    for (int i = 0; i<matrixSize;i++){

        currentMatrix[row][i] *= multiplication;
        inverseMatrix[row][i] *= multiplication;

    }
}

