#include "codecmi.h"

CodeCMI::CodeCMI(QLineSeries *series):CodeBase(series)
{

}

void CodeCMI::Coding(bool now)
{
    //即时编码 CMI
    if(CMI_first){
        CMI_first = false;
        if(now){
            //加个上升沿
            add_point(UpX, h);
            /*  设第一个1为高，1是连续两个时钟周期为相同电平，要分两次加点，突出最小时钟周期，方便电平识别  */
            CMI_hi_jump(range, h);
            CMI_ch_flag = false;
        }else{
            CMI_zero_jump();
        }
    }else{
        if(now){
            //此1，分高平两种情况
            if(CMI_ch_flag){
                //高
                if(last){
                    /*  此高，前1，加上升沿，要分两次加点，突出最小时钟周期，方便电平识别  */
                    add_point(UpX, h);
                    CMI_hi_jump(range, h);
                    CMI_ch_flag = false;
                }else{
                    //此高，前0，直接过
                    CMI_hi_jump(range, h);
                    CMI_ch_flag = false;
                }
            }else{
                //低
                if(last){
                    //此低，前1，加下降沿
                    add_point(UpX, n);
                    CMI_hi_jump(range, n);
                    CMI_ch_flag = true;
                }else{
                    //此低，前0，加下降沿
                    add_point(UpX, n);
                    CMI_hi_jump(range, n);
                    CMI_ch_flag = true;
                }
            }
        }else{
            //此0，分前1前0两种情况
            if(last){
                //前1，此0，分两种情况
                if(CMI_ch_flag){
                    //上一个为平1，直接跳
                    CMI_zero_jump();
                }else{
                    //上一个为高1，加下降沿
                    add_point(UpX, n);
                    CMI_zero_jump();
                }
            }else{
                //前0，此0，加下降沿
                add_point(UpX, n);
                CMI_zero_jump();
            }
        }
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

void CodeCMI::CMI_zero_jump()
{
    UpX += 0.5;
    add_point(UpX, n);
    add_point(UpX, h);
    UpX += 0.5;
    add_point(UpX, h);
}

void CodeCMI::CMI_hi_jump(qreal range, qreal y)
{
    /*  分两次加点，突出最小时钟周期，方便电平识别  */

    UpX += 0.5;
    add_point(UpX, y + range);
    UpX += 0.5;
    add_point(UpX, y + range);
}
