#ifndef CHANNELCODETH_H
#define CHANNELCODETH_H
#include <QThread>
#include "QtCharts"
#include "codebase.h"
#include "codeami.h"
#include "codecmi.h"
#include "codednrz.h"
#include "codedrz.h"
#include "codehdb3.h"
#include "codesnrz.h"
#include "codesrz.h"

using namespace QtCharts;

class ChannelCodeTh : public QThread
{
    Q_OBJECT

protected:
    void run() override;

public:
    ChannelCodeTh();

    //参数传入
    void set_attribute(QLineSeries *series, QString source_code, int way);

private:
    QString source_code;  //信源编码结果
    int way;  //哪种停信道编码方式
    int index = 1;  //档位

    /*  信道编码相关  */
    QLineSeries *series;
    QVector<QPointF> points;

    /*    Code基类    */
    CodeBase *base;

signals:
    void update_line_code(int code);  //更新码元显示
    void ch_code_finished();   //编码结束

private slots:
    void adjest_speed(int index);   //调速

};

#endif // CHANNELCODETH_H
