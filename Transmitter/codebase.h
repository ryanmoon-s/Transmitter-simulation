#ifndef CODEBASE_H
#define CODEBASE_H
//Code的基类，这里使用多态，策略模式
#include "QtCharts"

class CodeBase :public QObject
{
    Q_OBJECT

private:
    QMainWindow* getMainWindow();

public:
    CodeBase(QLineSeries *series);
    virtual ~CodeBase(){}
    virtual void Coding(bool now) = 0;

protected:
    QMainWindow *mainwin;
    QLineSeries *series;
    QVector<QPointF> points;
    qreal UpX = 0;    //x自增坐标
    qreal last_Y = 0;   //上一个Y坐标的位置，信道解码识别电平用
    bool last;  //上一个电平的高低
    qreal range = 0;
    qreal h = 1 + range, n = 0 + range, l = -1 + range;

    void add_point(qreal UpX, qreal Y);  //添加点
    void top_jump(qreal range);  //上跳
    void normal_jump(qreal range);    //正常低电平
    void buttom_jump(qreal range);   //下跳

signals:
    void push_rec_ch(char c);

};

#endif // CODEBASE_H
