#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <vector>
#include "fraction.h"

//A type definition of a vector of fractions within a vector of fractions
//Used to represent a matrix
typedef std::vector < std::vector<fraction> > vector2d;

bool inverse (int matrixSize, vector2d currentMatrix, vector2d& inverseMatrix);
void squareColumnMultiplication(int matrixSize, vector2d matrix1,
                                std::vector<fraction> matrix2, std::vector<fraction>& result);

#endif // MATRIX_H_INCLUDED
