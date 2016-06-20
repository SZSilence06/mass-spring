#ifndef SYSTEM_H
#define SYSTEM_H

#include "common.h"

using namespace Eigen;

class Mass{
public:
    Vector2f x;
    Vector2f r;
    float m;
    Vector2f v;
    Vector2f a;
};

class String{
public:
    float k;
};

class System{
private:
    int mode;

public:
    Mass mass[5];
    String str[4];

protected:
    static System* instance;
    MatrixXf M;
    MatrixXf K;
    MatrixXf A;
    MatrixXf MC;
    System();

public:
    static System* getInstance();
    void simulate();
    inline void setMode(int mode) { this->mode = mode; }
};

#endif // MASS_H
