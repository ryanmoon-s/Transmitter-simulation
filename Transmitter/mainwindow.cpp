#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*  设置状态栏   */
    QLabel *design = new QLabel("一舍5+4独家赞助，更多详情请点帮助-使用说明。");

    QFont font = design->font();
    font.setPointSize(12);
    font.setItalic(true);
    design->setFont(font);

    QColor color(50, 50, 50);
    QPalette pa;
    pa.setColor(QPalette::WindowText, color);
    design->setPalette(pa);

    ui->statusbar->addWidget(design);

    /*  按键   */
    ui->btnJump1->setEnabled(false);
    ui->btnSumm->setEnabled(false);
    ui->btnCalSource->setEnabled(false);
    ui->btnStartSouce->setEnabled(false);

    ui->btnJump2->setEnabled(false);

    /*  other   */
    create_chart();
    th = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOpenFile_clicked()
{
    /*
     * 打开文件，导入数据
     */

    ui->btnSumm->setEnabled(true);
#ifdef DEBUG
    QString filename = "G:/text.txt";
#else
    QString filename = QFileDialog::getOpenFileName(this, "选择一个文本文件", QDir::currentPath(), "文本文件(*.txt)");

#endif
     if(filename.isEmpty()){
        QMessageBox::warning(this, "警告", "取消选择文件");
    }else{
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this, "警告", "打开文件失败");
            return;
        }
        //输出到text browser
        QTextStream out(&file);
        fileSource = out.readAll();
        ui->textBrowser1->clear();
        ui->textBrowser1->append(fileSource);
        ui->textBrowser1->append("\n\n\n\n文件加载完成 ... ... ");
    }
}

void MainWindow::on_btnSumm_clicked()
{
    /*
     * 统计字符出现次数
     */

    ui->btnCalSource->setEnabled(true);
    ui->btnStartSouce->setEnabled(false);

    QString pri;

    /*    get result array    */
    Utils::statistical(fileSource, count_arr);

    /*    translate    */
    for(int i = 0; i < CODENUM; i++){
        if(count_arr[i] != 0){
            pri.append(QString::asprintf("有 %-3d 个 %c\n", count_arr[i], i));
        }
    }

    ui->textBrowser1->clear();
    ui->textBrowser1->append(pri);
    ui->textBrowser1->append("\n\n\n\n字符统计完成 ... ... ");
}

void MainWindow::on_btnCalSource_clicked()
{
    /*
     * 展示信源编码方式，什么编成什么。
     */

    ui->btnCalSource->setEnabled(false);
    ui->btnStartSouce->setEnabled(true);

    QString pri;
    ui->textBrowser1->clear();
    switch (ui->comboSource->currentIndex()) {
    case 0:
        Utils::source_shannon_code(count_arr, s_code);
        for(int i = 0; i < CODENUM; i++){
            if(s_code[i].value == -1)
                continue;
            pri = QString::asprintf("有 %-3d 个 %c ，编码为 %s\n", s_code[i].count, s_code[i].value, s_code[i].code);
            ui->textBrowser1->append(pri);
        }
        break;

    }

    ui->textBrowser1->append("\n\n\n\n计算编码完成 ... ... ");
}

void MainWindow::on_actionclear_all_triggered()
{
    /*
     * 功能-重置
     */

    /*  信源编码页面重置  */
    source_code.clear();
    ui->btnJump1->setEnabled(false);
    ui->btnSumm->setEnabled(false);
    ui->btnCalSource->setEnabled(true);
    ui->btnStartSouce->setEnabled(false);
    ui->textBrowser1->clear();
    for(int i = 0; i < CODENUM; i++){
        count_arr[i] = 0;
    }
    memset(s_code, 0, sizeof (s_code));
    for(int i = 0; i < CODENUM; i++){
        s_code->value = -1;
    }
    ui->stackedWidget->setCurrentIndex(0);

    /*  信道编码页面重置  */
    on_comboChannel_currentIndexChanged(0);
    ui->btnJump2->setEnabled(false);
}

void MainWindow::on_comboSource_currentIndexChanged(int index)  //combobox选择变化
{
    Q_UNUSED(index);
    ui->btnCalSource->setEnabled(false);
    ui->btnStartSouce->setEnabled(false);
    on_actionclear_all_triggered();
}

void MainWindow::on_btnStartSouce_clicked()
{
    /*
     * 信源编码
     */

    ui->btnJump1->setEnabled(true);
    ui->btnCalSource->setEnabled(true);
    ui->btnStartSouce->setEnabled(false);

    ui->textBrowser1->clear();
    QByteArray bstr = fileSource.toLocal8Bit();
    const char *str = bstr;
    int i;

    switch (ui->comboSource->currentIndex()) {
    case 0:
        /*    制作字符串    */
        for(*str; *str != '\0'; str++){
            /*  找到编码  */
            for (i = 0; i < CODENUM; i++) {
                if(s_code[i].value == int(*str)){
                    break;
                }
            }
            source_code.append(QString::asprintf("%s", s_code[i].code));
        }
        ui->textBrowser1->append(source_code);
        break;
    }

    ui->textBrowser1->append("\n\n\n\n编码完成 ... ... ");
}

void MainWindow::on_btnJump1_clicked()  //进入信道编码
{
    ui->stackedWidget->setCurrentIndex((ui->stackedWidget->currentIndex() + 1) % 2);
    ui->labSourceCode->setText(ui->comboSource->currentText());
}

void MainWindow::on_btnJump2_clicked()   //进入信源解码
{
    ui->stackedWidget->setCurrentIndex((ui->stackedWidget->currentIndex() + 1) % 2);
}

void MainWindow::create_chart()
{
    /*
     * 创建chart，做必要初始化
     */

    /*  创建图表  */
    chart = new QChart();
    chart->setTitle("广告位招租");
    ui->chartView->setChart(chart);

    /*  创建坐标轴  */
    axisX = new QValueAxis();
    axisX->setLabelFormat("%d");
    axisX->setTickCount(23);
    axisX->setRange(0, 22);
    axisX->setTitleText("time");

    axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    axisY->setRange(-2, 2);
    axisY->setTickCount(5);
    axisY->setTitleText("level");

    /*  创折线序列  */
    series = new QLineSeries();
    chart->addSeries(series);
    /*  为序列设置坐标轴  */
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    /*  添加初始坐标原点  */
    series->append(0, 0);
    /*  标题  */
    series->setName(ui->comboChannel->currentText());
}


void MainWindow::update_line_code(int code)
{
    /*
     * 更新line_code的槽函数，展示码元
     */

    ui->line_code->setText(ui->line_code->text() + QString::number(code));
    if(ui->line_code->text().count() >= LINE_CODE_MAX)
        ui->line_code->clear();
}

void MainWindow::ch_code_finished()
{
    /*
     * 线程完成任务的槽函数，即信道编码结束
     * 提示用户，信道编码完成
     */

    QMessageBox::information(this, "提示", "信道编码已经结束");
}

void MainWindow::on_spinBox_valueChanged(int arg1)  //调速
{
    emit adjest_speed(arg1);
}

void MainWindow::on_comboChannel_currentIndexChanged(int index)
{
    /*
     * 信道编码方式切换
     */

    /*  th为空，还没启动信道编码线程，不用做以下清除操作  */
    if(!th)
        return;

    /*  清空码元展示、速度重置  */
    ui->line_code->clear();
    ui->spinBox->setValue(1);

    /*  终止当前信道编码线程  */
    th->terminate();
    th->wait();
    delete th;
    th = NULL;

    /*  清空chart  */
    QVector<QPointF> point;
    series->replace(point);

    /*  其它  */
    ui->btnChStart->setEnabled(true);
    Q_UNUSED(index);
}

void MainWindow::on_btnChStart_clicked()
{
    /*
     * 启动信道编码
     * 负责启动code线程
     */

    /*  按键(必须在new之前，不然如果快速按两下按键，第一个线程就游离了)  */
    ui->btnChStart->setEnabled(false);
    ui->btnJump2->setEnabled(true);

    th = new ChannelCodeTh();
    connect(th, SIGNAL(update_line_code(int)), this, SLOT(update_line_code(int)));
    connect(this, SIGNAL(adjest_speed(int)), th, SLOT(adjest_speed(int)));
    connect(th, SIGNAL(ch_code_finished()), this, SLOT(ch_code_finished()));
    th->set_attribute(series, chart, axisX, axisY, source_code, ui->comboChannel->currentIndex());
    th->start();
}

void MainWindow::on_actioninformation_triggered()
{
    /*
     * 进入使用说明
     */

    DlgHelp he;
    he.exec();
}
