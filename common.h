#ifndef COMMON_H
#define COMMON_H

#include <eigen3/Eigen/Dense>

#include<zjucad/matrix/matrix.h>
#include<zjucad/matrix/itr_matrix.h>
#include<zjucad/matrix/conversion.h>
#include<zjucad/matrix/io.h>

typedef double real_t;
typedef zjucad::matrix::matrix<real_t> matrixr_t; // real
typedef zjucad::matrix::matrix<int>    matrixi_t; // int
typedef zjucad::matrix::matrix<size_t> matrixs_t;
typedef zjucad::matrix::matrix<bool>   matrixb_t;

#endif // COMMON_H
