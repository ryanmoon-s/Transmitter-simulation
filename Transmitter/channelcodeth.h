#ifndef CHANNELCODETH_H
#define CHANNELCODETH_H
#include <QThread>
#include "QtCharts"
using namespace QtCharts;

struct HDB3_last_point{   //HDB3数据结构
    int index = -1;    //上一个V的位置
    int level = -1;    //高电平1，低电平0
};

class ChannelCodeTh : public QThread
{
    Q_OBJECT

protected:
    void run() override;

public:
    ChannelCodeTh();

    //参数传入
    void set_attribute(QLineSeries *series, QChart *chart, QValueAxis *axisX, QValueAxis *axisY, QString source_code, int way);

private:
    /*    变量声明    */

    QString source_code;  //信源编码结果
    int way;  //哪种停信道编码方式
    int index = 1;  //档位

    /*  信道编码相关  */
    void create_chart();
    QLineSeries *series;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QVector<QPointF> points;
    QByteArray rec_channel;   //信道编码识别出的电平序列，高为h，平为n，低为l

    /* 通用  */
    bool last;  //上一个电平的高低
    int count = 0;  //计数，当前是第几个脉冲
    qreal UpX = 0;    //x自增坐标
    qreal last_Y = 0;   //上一个Y坐标的位置，信道解码识别电平用

    /*  AMI  */
    bool AMI_ch_flag = true;   //AMI交替flag

    /*  HDB3  */
    int HDB3_zero_num = 0;   //0数量
    int HDB3_one_num = 0;   //两个v之间的1数量
    bool HDB3_ch_flag = true;   //HDB3交替flag
    bool HDB3_is_first_v = true;    //是否是第一个v
    HDB3_last_point last_point;   //上一个v

    /*  通用标志位  */
    bool SNRZ_first = true;  //SNRZ码是否为第一个
    bool DNRZ_first = true;  //DNRZ码是否为第一个
    bool CMI_first = true;   //CMI码是否为第一个
    bool CMI_ch_flag = true;  //CMI交替flag

    /*    函数声明    */

    /*    CMI的零有三步，单独写个函数    */
    void CMI_zero_jump();

    /*    归零用    */
    void top_jump(qreal range);  //上跳
    void normal_jump(qreal range);    //正常低电平
    void buttom_jump(qreal range);   //下跳

    void HDB3_v_jump(qreal range);   //v高电平跳转与前一个相同
    void HDB3_normal_jump(qreal range);   //正常高电平跳转，与前一个相反

    void code_machine(bool now);
    void add_point(qreal UpX, qreal Y);  //添加点

signals:
    void update_line_code(int code);  //更新码元显示
    void ch_code_finished();   //编码结束
    void push_rec_ch(char c);    //将转换出的电平传给主界面打印

private slots:
    void adjest_speed(int index);   //调速

};

#endif // CHANNELCODETH_H
