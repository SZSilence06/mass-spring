#ifndef SYSTEM_H
#define SYSTEM_H

#include "common.h"

#include <vector>

using namespace std;
using namespace Eigen;

class Mass{
public:
    Vector2f x;
    Vector2f r;
    float m;
    Vector2f v;
    Vector2f a;
    vector<int> strings;
};

class String{
public:
    float k;
    float l;
    int mass1;
    int mass2;
};

class System{
private:
    int mode;

public:
    vector<Mass> mass;
    vector<String> str;

protected:
    static System* instance;
    MatrixXf M;
    MatrixXf A;
    MatrixXf MC;
    Vector2f G;
    System();

public:
    static System* getInstance();
    void load(char* filePath);
    void simulate();
    inline void setMode(int mode) { this->mode = mode; }
};

#endif // MASS_H
