#include "codesrz.h"

CodeSRZ::CodeSRZ(QLineSeries *series):CodeBase(series)
{

}

void CodeSRZ::Coding(bool now)
{
    //即时编码 单极性归零码
    if(now){
        //高电平
        top_jump(range);
    }else{
        //低电平
        normal_jump(range);
    }
}
