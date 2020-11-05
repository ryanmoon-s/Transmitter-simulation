#ifndef CODEHDB3_H
#define CODEHDB3_H
#include <QString>
#include "codebase.h"

class CodeHDB3 :public CodeBase
{
public:
    CodeHDB3(QLineSeries *series, QString source_code);
    virtual void Coding(bool now) override;

    bool flush = false;  //最后刷新四个之内的零

private:
    int count = 0;  //hdb3中，下标
    int HDB3_zero_num = 0;   //0数量
    int HDB3_one_num = 0;   //两个v之间的1数量
    bool HDB3_ch_flag = true;   //HDB3交替flag
    bool HDB3_is_first_v = true;    //是否是第一个v
    int last_v_pos;   //上一个v的pos
    QString source_code;
    const char *str;  //整个编码串的char*形式

    void HDB3_v_jump(qreal range);   //v高电平跳转与前一个相同
    void HDB3_normal_jump(qreal range);   //正常高电平跳转，与前一个相反
};

#endif // CODEHDB3_H
