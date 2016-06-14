#include "System.h"
#include "common.h"

#include <iostream>
using namespace std;

System* System::instance = nullptr;

System::System(){
    for(int i = 0; i < 5; i++){
        mass[i].m = 5;
        mass[i].v = 0;
        mass[i].originalX = mass[i].x = 2 * i;
    }

    mass[4].x += 2;

    for(int i = 0; i < 4; i++){
        str[i].k = 0.2;
        str[i].l = 2;
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
    VectorXf X(5,1);
    VectorXf V(5,1);

    for(int i = 0; i <X.size(); i++){
        X[i] = mass[i].x - mass[i].originalX;
    }

    for(int i = 0; i < V.size(); i++){
        V[i] = mass[i].v;
    }

    MatrixXf KVX = - K * (X + V);
    VectorXf F = - K * X;

    VectorXf FC = VectorXf::Zero(F.size() + 1);
    for(int i = 0; i < F.size(); i++){
        FC[i] = F[i];
    }
    VectorXf ALambda = MC.inverse() * FC;
    VectorXf dV = VectorXf::Zero(F.size());
    for(int i = 0; i < dV.size(); i++){
        dV[i] = ALambda[i];
    }

    if(mode == 1){
        //dV = M.inverse() * F;
        X += V;
        V += dV;
    }
    else if(mode == 2){
        VectorXf Fc = VectorXf::Zero(F.size());
        Fc[0] = -F[0];
        dV = M.inverse() * (F + Fc);
        V += dV;
        X += V;
    }
    else if(mode == 3){
        //dV = A.inverse() * KVX;
        V += dV;
        X += V;
    }

    for(int i = 0; i < X.size(); i++){
        mass[i].x = mass[i].originalX + X[i];
        mass[i].v = V[i];
    }
}
