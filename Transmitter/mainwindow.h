#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "utils.h"
#include "QtCharts"
using namespace QtCharts;
#include "channelcodeth.h"
#include "dlghelp.h"

#define LINE_CODE_MAX 100

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*    信道编码相关    */
    void CMI_zero_jump(QLineSeries *series);  //CMI的零有三步，单独写个函数

    //归零用
    void top_jump(QLineSeries *series, qreal range, qreal *UpX);  //上跳
    void normal_jump(QLineSeries *series, qreal range, qreal *UpX);    //正常低电平
    void buttom_jump(QLineSeries *series, qreal range, qreal *UpX);   //下跳

    void HDB3_v_jump(qreal range, qreal *UpX);   //v高电平跳转与前一个相同
    void HDB3_normal_jump(qreal range, qreal *UpX);   //正常高电平跳转，与前一个相反

private slots:

    /*    自定义槽    */
    void update_line_code(int code);
    void ch_code_finished();

    /*    系统槽    */
    void on_btnOpenFile_clicked();

    void on_btnSumm_clicked();

    void on_btnCalSource_clicked();

    void on_actionclear_all_triggered();

    void on_comboSource_currentIndexChanged(int index);

    void on_btnStartSouce_clicked();

    void on_btnJump1_clicked();

    void on_btnJump2_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_comboChannel_currentIndexChanged(int index);

    void on_btnChStart_clicked();

    void on_actioninformation_triggered();

private:
    Ui::MainWindow *ui;

    /*    信源编码 相关    */
    QString fileSource;  //源文件
    int count_arr[CODENUM]; //统计数组
    s_dat s_code[CODENUM];  //香农编码数组，预留空间够大，能应对所有ASCII码内存在的元素
    QString source_code;  //信源最终编码
    ChannelCodeTh *th;  //信道编码负责线程

    /*    信道编码 charts 相关    */
    void create_chart();
    QLineSeries *series;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

signals:
    void adjest_speed(int index);

};
#endif // MAINWINDOW_H
