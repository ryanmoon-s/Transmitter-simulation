#include "codednrz.h"

CodeDNRZ::CodeDNRZ(QLineSeries *series):CodeBase(series)
{

}

void CodeDNRZ::Coding(bool now)
{
    //即时编码 双极性非归零码
    if(DNRZ_first){
        DNRZ_first = false;
        add_point(UpX, l);
        if(now){
            //高，加个上升沿
            add_point(UpX, h);
            UpX += 1;
            add_point(UpX, h);
        }else{
            //低，正常走
            UpX += 1;
            add_point(UpX, l);
        }
    }else{
        if(now){
            if(last){
                //此高前高
                UpX += 1;
                add_point(UpX, h);
            }else{
                //此高前低，加一个上升沿
                add_point(UpX, h);
                UpX += 1;
                add_point(UpX, h);
            }
        }else{
            if(last){
                //此低前高，加一个下降沿
                add_point(UpX, l);
                UpX += 1;
                add_point(UpX, l);
            }else{
                //此低前低
                UpX += 1;
                add_point(UpX, l);
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
