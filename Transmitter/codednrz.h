#ifndef CODEDNRZ_H
#define CODEDNRZ_H
//双极性非归零码
#include "codebase.h"

class CodeDNRZ : public CodeBase
{
public:
    CodeDNRZ(QLineSeries *series);
    virtual void Coding(bool now) override;

private:
    bool DNRZ_first = true;  //DNRZ码是否为第一个
};

#endif // CODEDNRZ_H
