#ifndef CODESRZ_H
#define CODESRZ_H
#include "codebase.h"

class CodeSRZ: public CodeBase
{

public:
    CodeSRZ(QLineSeries *series);
    virtual void Coding(bool now) override;


};

#endif // CODESRZ_H
