#include "codebase.h"

CodeBase::CodeBase(QLineSeries *series)
{
    this->series = series;

    //发送到主窗口
    mainwin = getMainWindow();
    connect(this, SIGNAL(push_rec_ch(char)), mainwin, SLOT(push_rec_ch(char)));
}

QMainWindow *CodeBase::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return mainWin;
    return nullptr;
}

void CodeBase::add_point(qreal UpX, qreal Y)
{
    /*
     * 包含添加点、通过坐标识别电平高低
     */

    /*
     * 识别电平
     * 判断上一个Y坐标与这一个Y坐标关系，以裁决是产生了一个高电平还是产生了一个低电平
     */
    if(last_Y == Y){
        if(Y == 0)
            emit push_rec_ch('n');
        else if(Y == 1)
            emit push_rec_ch('h');
        else if(Y == -1)
            emit push_rec_ch('l');
    }

    /*  添加点  */
    points = series->pointsVector();
    points.append(QPointF(UpX, Y));
    series->replace(points);

    /*  记录上一次的y坐标，供识别电平用  */
    last_Y = Y;
}

void CodeBase::top_jump(qreal range)
{
    qreal y_h = 1 + range, y_n = 0 + range;

    add_point(UpX, y_h);
    UpX += 0.5;
    add_point(UpX, y_h);
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}

void CodeBase::normal_jump(qreal range)
{
    qreal y_n = 0 + range;

    UpX += 0.5;
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}

void CodeBase::buttom_jump(qreal range)
{
    qreal y_n = 0 + range, y_l = -1 + range;

    add_point(UpX, y_l);
    UpX += 0.5;
    add_point(UpX, y_l);
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}
