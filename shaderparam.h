#ifndef SHADERPARAM_H
#define SHADERPARAM_H

#include <QString>


struct ShaderParameter{
    QString name;
    int     type;
    int     location;
    void*   value;
};


enum ParamType{
    Int,
    Uint,
    Float
};

#endif // SHADERPARAM_H
