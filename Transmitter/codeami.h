#ifndef CODEAMI_H
#define CODEAMI_H
#include "codebase.h"

class CodeAMI:public CodeBase
{
public:
    CodeAMI(QLineSeries *series);
    virtual void Coding(bool now) override;

private:
    bool AMI_ch_flag = true;   //AMI交替flag
};

#endif // CODEAMI_H
