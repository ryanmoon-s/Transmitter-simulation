#include "codeami.h"

CodeAMI::CodeAMI(QLineSeries *series):CodeBase(series)
{

}

void CodeAMI::Coding(bool now)
{
    //即时编码 AMI
    if(now){
        //高电平
        if(AMI_ch_flag){  //上翻
            top_jump(range);
            AMI_ch_flag = false;
        }else{  //下翻
            buttom_jump(range);
            AMI_ch_flag = true;
        }
    }else{
        //低电平
        normal_jump(range);
    }


    last = now; //上一个电平赋值为当前电平
    /*  超出x边界  */
    if(UpX >= 23){
        /*  清空图表  */
        /* 牺牲一格，避免换到初始坐标后没有前导，出现空格的情况
         * 到了0坐标后，下一个从1开始，那么0-1之间没有连线
         * 只受视觉影响，不影响上面实际点的添加
         */
        UpX = -1;
        QVector<QPointF> point;
        point.clear();
        series->replace(point);
    }
}

