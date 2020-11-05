#ifndef CODEDRZ_H
#define CODEDRZ_H
#include "codebase.h"

class CodeDRZ :public CodeBase
{
public:
    CodeDRZ(QLineSeries *series);
    virtual void Coding(bool now) override;

};

#endif // CODEDRZ_H
