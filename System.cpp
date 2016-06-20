#include "System.h"
#include "common.h"

#include <iostream>
using namespace std;

System* System::instance = nullptr;

System::System(){
    for(int i = 0; i < 5; i++){
        mass[i].m = 5;
        mass[i].v = Vector2f::Zero();
        mass[i].x = Vector2f::Zero();
        mass[i].r[0] = 100 * i;
        mass[i].r[1] = 100 * i;
        //mass[i].r[0] = 100 * i;
        //mass[i].r[1] = 250;
    }

    mass[4].x[0] = 50;
    mass[4].x[1] = 10;

    for(int i = 0; i < 4; i++){
        str[i].k = 0.2;
    }

    M = MatrixXf::Zero(5,5);
    K = MatrixXf::Zero(5,5);

    for(int i = 0; i < M.cols(); i++){
        M(i, i) = mass[i].m;
    }

    for(int i = 0; i < K.cols(); i++){
        if(i - 1 >= 0){
            K(i, i - 1) = -str[i-1].k;
            K(i,i) += str[i-1].k;
        }
        if(i + 1 <K.cols()){
            K(i, i + 1) = -str[i].k;
            K(i,i) += str[i].k;
        }
    }

    A = M + K;

    MC = MatrixXf::Zero(6,6);
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            MC(i,j) = M(i,j);
        }
    }
    MC(5,0) = 1;
    MC(5,1) = -1;
    MC(0,5) = 1;
    MC(1,5) = -1;
}

System* System::getInstance(){
    if(instance == nullptr){
        instance = new System();
    }
    return instance;
}

void System::simulate(){
    MatrixXf X(5,2);
    MatrixXf V(5,2);

    for(int i = 0; i <X.rows(); i++){
        X.row(i) = mass[i].x;
    }

    for(int i = 0; i < V.rows(); i++){
        V.row(i) = mass[i].v;
    }

    MatrixXf KVX = - K * (X + V);
    MatrixXf F = MatrixXf::Zero(5,2);
    for(int i = 0; i < F.rows(); i++){
        if(i > 0){
            Vector2f temp = mass[i].x - mass[i-1].x + mass[i].r - mass[i-1].r;
            F.row(i) -= str[i-1].k * (temp.norm() - (mass[i].r - mass[i-1].r).norm())
                    * temp.normalized();
        }
        if(i < F.rows() - 1){
            VectorXf temp = mass[i+1].x - mass[i].x + mass[i+1].r - mass[i].r;
            F.row(i) += str[i].k * (temp.norm() - (mass[i+1].r - mass[i].r).norm())
                    * temp.normalized();
        }
    }

    MatrixXf dV = M.inverse() * F;

    /*VectorXf FC = VectorXf::Zero(F.size() + 1);
    for(int i = 0; i < F.size(); i++){
        FC[i] = F[i];
    }
    VectorXf ALambda = MC.inverse() * FC;
    VectorXf dV = VectorXf::Zero(F.size());
    for(int i = 0; i < dV.size(); i++){
        dV[i] = ALambda[i];
    }*/

    if(mode == 1){
        //dV = M.inverse() * F;
        X += V;
        V += dV;
    }
    else if(mode == 2){
        //VectorXf Fc = VectorXf::Zero(F.size());
        //Fc[0] = -F[0];
        //dV = M.inverse() * (F + Fc);
        V += dV;
        X += V;
    }
    else if(mode == 3){
        //dV = A.inverse() * KVX;
        V += dV;
        X += V;
    }

    for(int i = 0; i < X.rows(); i++){
        mass[i].x = X.row(i);
        mass[i].v = V.row(i);
    }
}
