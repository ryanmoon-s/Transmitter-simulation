#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "utils.h"
#include "QtCharts"
using namespace QtCharts;
#include "channelcodeth.h"
#include "dlghelp.h"

#define LINE_CODE_MAX 80

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /*  自编函数  */
    void huff_code(h_dat *root, int deepth, h_dat **h_code, int *index);  //哈夫曼遍历编码

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
    void push_rec_ch(char c);

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

    void on_btnChDecode_clicked();

    void on_btnSouDecode_clicked();

    void on_btnJump3_clicked();

    void on_btnBack1_clicked();

    void on_btnBack2_clicked();

private:
    Ui::MainWindow *ui;

    /*    信源编码   */
    QString fileSource;  //源文件
    int count_arr[CODENUM]; //统计数组
    s_dat s_code[CODENUM];  //香农编码数组，预留空间够大，能应对所有ASCII码内存在的元素
    f_dat f_code[CODENUM];  //费诺编码数组
    h_dat *head;  //哈夫曼头指针
    h_dat *h_code[CODENUM]; //哈夫曼指针数组
    int code_arr_count = 0;  //哈夫曼数组元素有效个数

    code_t _code_[CODENUM];  //每种信源编码在进行编码时，都将编码需要的二要素(值、码)，赋值到这个数组，信源解码时用这个数组接口
    int _code_num_ = 0;  //_code_数组的成员数量，方便查找，更快速
    int _code_max_ = 0;  //编码中最长一个的长度

    QString source_code;  //信源最终编码
    QString channel_code;   //信道最终编码
    QString channel_decode;    //信道解码结果
    QString source_decode;    //信源解码结果，即原文本
    ChannelCodeTh *th;  //信道编码负责线程
    bool huf_once = true;  //哈夫曼跳过根


    /*    信道编码    */
    void create_chart();
    QLineSeries *series;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

signals:
    void adjest_speed(int index);

};
#endif // MAINWINDOW_H
