#ifndef CODESNRZ_H
#define CODESNRZ_H
//单极性非归零码
#include "codebase.h"

class CodeSNRZ :public CodeBase
{
public:
    CodeSNRZ(QLineSeries *series);
    virtual void Coding(bool now) override;


private:
    bool SNRZ_first = true;  //SNRZ码是否为第一个

};

#endif // CODESNRZ_H
