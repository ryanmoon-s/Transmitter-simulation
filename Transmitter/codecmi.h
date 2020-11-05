#ifndef CODECMI_H
#define CODECMI_H
#include "codebase.h"

class CodeCMI:public CodeBase
{
public:
    CodeCMI(QLineSeries *series);
    virtual void Coding(bool now) override;

private:
    bool CMI_first = true;   //CMI码是否为第一个
    bool CMI_ch_flag = true;  //CMI交替flag

    void CMI_zero_jump();
    void CMI_hi_jump(qreal range, qreal y);   //1所有函数，连续低或连续高
};

#endif // CODECMI_H
