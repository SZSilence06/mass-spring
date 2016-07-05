#include "System.h"
#include "common.h"

#include <iostream>
#include <fstream>
using namespace std;

System* System::instance = nullptr;

System::System(){
    G[0] = 0;
    G[1] = -2;
    h = 5;
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
        if(i == 0 || i == 1 || i == 2){
            continue;
        }
        for(int j = 0; j < mass[i].strings.size(); j++){
            String tempStr = str[mass[i].strings[j]];
            int anotherMass;

            if(tempStr.mass1 != i){
                anotherMass = tempStr.mass1;
            }
            else{
                anotherMass = tempStr.mass2;
            }
            Vector2f temp = mass[i].x - mass[anotherMass].x + mass[i].r - mass[anotherMass].r;
            Vector2f temp2 = tempStr.k * (temp.norm() - tempStr.l) * temp.normalized();
            F[i*2] -= temp2[0];
            F[i*2+1] -= temp2[1];

            MatrixXf ttT = temp2 * temp2.transpose();
            K(i*2,i*2) = ttT(0,0);
            K(i*2,i*2+1) = ttT(0,1);
            K(i*2+1,i*2) = ttT(1,0);
            K(i*2+1,i*2+1) = ttT(1,1);
            K(i*2,anotherMass*2) = -ttT(0,0);
            K(i*2,anotherMass*2+1) = -ttT(0,1);
            K(i*2+1,anotherMass*2) = -ttT(1,0);
            K(i*2+1,anotherMass*2+1) = -ttT(1,1);
        }
        F[i*2] += G[0];
        F[i*2+1] += G[1];
    }

    VectorXf dV;

    if(mode == 1){
        dV = M.inverse() * F * h;
        X += V;
        V += dV;
    }
    else if(mode == 2){
        //VectorXf Fc = VectorXf::Zero(F.size());
        //Fc[0] = -F[0];
        dV = M.inverse() * F * h;
        V += dV;
        X += V;
    }
    else if(mode == 3){
        //dV = A.inverse() * KVX;

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
