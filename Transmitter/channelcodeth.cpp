#include "channelcodeth.h"

ChannelCodeTh::ChannelCodeTh()
{

}

void ChannelCodeTh::set_attribute(QLineSeries *series, QString source_code, int way)
{
    /*
     * 传入操作的对象，顺便添加初始坐标原点
     */

    this->series = series;
    this->source_code = source_code;
    this->way = way;

    /*  添加初始坐标原点  */
    points = series->pointsVector();
    points.append(QPointF(0, 0));
    series->replace(points);
}


void ChannelCodeTh::run()
{
    /*
     * 解析经过信源编码的01二进制流，通过Bool型模拟电平，传给编码器进行编码模拟
     * 要求能控制时速
     */

    QByteArray bstr = source_code.toLocal8Bit();
    const char *str = bstr;
    bool now;
    CodeHDB3 hd3(series, source_code);   //可能不会用上,hdb3才能用上
    switch(way){
    case 0:
        base = new CodeSNRZ(series);
        break;
    case 1:
        base = new CodeDNRZ(series);
        break;
    case 2:
        base = new CodeSRZ(series);
        break;
    case 3:
        base = new CodeDRZ(series);
        break;
    case 4:
        base = new CodeAMI(series);
        break;
    case 5:
        base = &hd3;
        break;
    case 6:
        base = new CodeCMI(series);
        break;
    }

    /*  编码控制  */
    for(*str; *str != '\0'; str++){
        if(*str == '1'){
            /*  发信号通知主界面更新code码元  */
            emit update_line_code(1);
            /*  发给编码器进行信道编码  */
            now = true;
        }else{
            emit update_line_code(0);
            if(*(str + 1) == '\0'){
                /*  这是最后一个0，刷新出积累的0，hdb3专用  */
                now = false;
                hd3.flush = true;
            }else{
                now = false;
            }
        }
        /*编码*/
        base->Coding(now);  //多态调用
        /*  时速控制，十档延时  */
        msleep(20*(11-index));
    }

    /*  编码结束  */
    emit ch_code_finished();
    quit();
}


void ChannelCodeTh::adjest_speed(int index)  //调速
{
    this->index = index;
}

