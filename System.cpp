#include "System.h"
#include "common.h"

#include <iostream>
#include <fstream>
using namespace std;

System* System::instance = nullptr;

System::System(){
    h = 1;
}

System* System::getInstance(){
    if(instance == nullptr){
        instance = new System();
    }
    return instance;
}

void System::load(char *filePath){
    ifstream fin;
    fin.open(filePath);

    char buffer[100];
    int i = 0;
    while(fin.getline(buffer,sizeof(buffer))){
        stringstream ss;
        ss << buffer;

        char symbol;
        ss >> symbol;
        if(symbol == 'v'){
            Mass tempMass;
            tempMass.m = 10;
            tempMass.v = Vector2f::Zero();
            tempMass.x = Vector2f::Zero();
            ss >> tempMass.r[0];
            ss >> tempMass.r[1];

            mass.push_back(tempMass);
        }
        else if(symbol == 'f'){
            int a,b,c;
            ss >> a;
            ss >> b;
            ss >> c;

            String tempStr[3];
            tempStr[0].mass1 = a - 1;
            tempStr[0].mass2 = b - 1;
            tempStr[1].mass1 = a - 1;
            tempStr[1].mass2 = c - 1;
            tempStr[2].mass1 = b - 1;
            tempStr[2].mass2 = c - 1;

            for(int i = 0; i < 3; i++){
                bool existed = false;
                for(int j = 0; j < str.size(); j++){
                    if((str[j].mass1 == tempStr[i].mass2 && str[j].mass2 == tempStr[i].mass1) ||
                       (str[j].mass1 == tempStr[i].mass1 && str[j].mass2 == tempStr[i].mass2)){
                        existed = true;
                        break;
                    }
                }
                if(!existed){
                    tempStr[i].k = 1;
                    tempStr[i].l = (mass[tempStr[i].mass1].r - mass[tempStr[i].mass2].r).norm();
                    str.push_back(tempStr[i]);
                    mass[tempStr[i].mass1].strings.push_back(str.size()-1);
                    mass[tempStr[i].mass2].strings.push_back(str.size()-1);
                }
            }
        }
    }
    fin.close();

    M = MatrixXf::Zero(mass.size()*2,mass.size()*2);
    G = VectorXf::Zero(mass.size()*2);
    for(int i = 0; i <mass.size(); i++){
        G[i*2] = 0;
        G[i*2+1] = -2;
    }

    for(int i = 0; i < mass.size(); i++){
        M(i*2, i*2) = mass[i].m;
        M(i*2+1, i*2+1) = mass[i].m;
    }

}

void System::simulate(){
    VectorXf X = VectorXf::Zero(mass.size()*2);
    VectorXf V = VectorXf::Zero(mass.size()*2);

    for(int i = 0; i <mass.size(); i++){
        X[i*2] = mass[i].x[0];
        X[i*2+1] = mass[i].x[1];
    }

    for(int i = 0; i < mass.size(); i++){
        V[i*2] = mass[i].v[0];
        V[i*2+1] = mass[i].v[1];
    }

    VectorXf F = VectorXf::Zero(mass.size()*2);
    MatrixXf K = MatrixXf::Zero(mass.size()*2,mass.size()*2);
    for(int i = 0; i < mass.size(); i++){
        for(int j = 0; j < mass[i].strings.size(); j++){
            String tempStr = str[mass[i].strings[j]];
            int anotherMass;

            if(tempStr.mass1 != i){
                anotherMass = tempStr.mass1;
            }
            else{
                anotherMass = tempStr.mass2;
            }
            Vector2f temp = mass[anotherMass].x - mass[i].x + mass[anotherMass].r - mass[i].r;
            Vector2f temp2 = tempStr.k * (temp.norm() - tempStr.l) * temp.normalized();
            F[i*2] += temp2[0];
            F[i*2+1] += temp2[1];

            Vector2f temp3 = temp.normalized();
            Matrix2f ttT = temp3 * temp3.transpose();
            float norm = temp.norm();
            float norm3 = norm * norm * norm;
            Matrix2f dnormT;
            dnormT(0,0) = temp[1] * temp[1] / norm3;
            dnormT(0,1) = -temp[0] * temp[1] / norm3;
            dnormT(1,0) = -temp[1] * temp[0] / norm3;
            dnormT(1,1) = temp[0] * temp[0] / norm3;
            ttT += (norm - tempStr.l) * dnormT;

            K(i*2,i*2) += ttT(0,0) * tempStr.k;
            K(i*2,i*2+1) += ttT(0,1) * tempStr.k;
            K(i*2+1,i*2) += ttT(1,0) * tempStr.k;
            K(i*2+1,i*2+1) += ttT(1,1) * tempStr.k;
            K(i*2,anotherMass*2) -= ttT(0,0) * tempStr.k;
            K(i*2,anotherMass*2+1) -= ttT(0,1) * tempStr.k;
            K(i*2+1,anotherMass*2) -= ttT(1,0) * tempStr.k;
            K(i*2+1,anotherMass*2+1) -= ttT(1,1) * tempStr.k;
        }
    }

    VectorXf dV;

    if(mode == 1){
        dV = M.inverse() * h * (-K * X + G);
        for(int i=0;i<3;i++){
            dV[i*2] = dV[i*2+1] = 0;
        }
        X += V;
        V += dV;
    }
    else if(mode == 2){
        dV = M.inverse() * h * (F + G);
        for(int i=0;i<3;i++){
            dV[i*2] = dV[i*2+1] = 0;
        }
        V += dV;
        X += V;
    }
    else if(mode == 3){
        dV = (M + h * h * K).inverse() * h * (F - K * h * V + G);
        for(int i=0;i<3;i++){
            dV[i*2] = dV[i*2+1] = 0;
        }
        V += dV;
        X += V;
    }

    for(int i = 0; i < mass.size(); i++){
        mass[i].x[0] = X[i*2];
        mass[i].x[1] = X[i*2+1];
        mass[i].v[0] = V[i*2];
        mass[i].v[1] = V[i*2+1];
    }
}
