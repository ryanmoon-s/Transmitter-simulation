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
            pri.append(QString::asprintf("有 %-3d 个 %c\n\n", count_arr[i], i));
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
    ui->comboSource->setCurrentIndex(0);

    /*  信道编码页面重置  */
    on_comboChannel_currentIndexChanged(0);
    ui->btnJump2->setEnabled(false);
    ui->textRecCh->clear();
    ui->comboChannel->setCurrentIndex(0);

    /*  解码页面重置  */
    ui->textChDe->clear();
    ui->textSouDe->clear();

    ui->stackedWidget->setCurrentIndex(0);
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
    ui->stackedWidget->setCurrentIndex(1);
    ui->labSourceCode->setText(ui->comboSource->currentText());
}

void MainWindow::on_btnJump2_clicked()
{
    /*
     * 进入信源解码
     */

    /*  读取信道编码结果  */
    channel_code = ui->textRecCh->document()->toPlainText();

    /*  设置相关文本，跳转  */
    ui->lab_Chde->setText(ui->comboChannel->currentText());
    ui->lab_SouDe->setText(ui->comboSource->currentText());
    ui->stackedWidget->setCurrentIndex(2);
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

    ui->btnJump2->setEnabled(true);
    QMessageBox::information(this, "提示", "信道编码已经结束");
}

void MainWindow::push_rec_ch(char c)
{
    /*
     * 打印信道解码过程识别到的电平
     * 直接append会在末尾换行，所以就只有一列
     * 此处取出文本内容，加上字符，写回去
     */

    QString str;
    str = ui->textRecCh->document()->toPlainText();
    str.append(c);
    ui->textRecCh->clear();
    ui->textRecCh->append(str);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    /*
     * 调速
     */

    emit adjest_speed(arg1);
}

void MainWindow::on_comboChannel_currentIndexChanged(int index)
{
    /*
     * 信道编码方式切换
     */

    series->setName(ui->comboChannel->currentText());

    /*  th为空，还没启动信道编码线程，不用做以下清除操作  */
    if(!th)
        return;

    /*  清空码元展示、速度重置  */
    ui->line_code->clear();
    ui->textRecCh->clear();
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

    th = new ChannelCodeTh();
    connect(th, SIGNAL(update_line_code(int)), this, SLOT(update_line_code(int)));
    connect(th, SIGNAL(push_rec_ch(char)), this, SLOT(push_rec_ch(char)));
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

void MainWindow::on_btnChDecode_clicked()
{
    /*
     * 开始信道解码
     */

    channel_decode.clear();
    QByteArray bstr = channel_code.toLocal8Bit();
    const char *str = bstr;

    /*  解码  */
    char last_level = 'i';   //上一个电平状态，归零码判断低电平专用(连续两个低电平解释为1)

    char last_h_hdb3 = 'n';    //上一个脉冲是向上还是向下，HDB3判断V电平专用(连续两个相同属性的脉冲，第二个将被解释为0)
    int count = 0;   //记录当前下标是第几个,hdb3用到了
    int v_count_hdb3 = 0;   //两个V之间1的个数，hdb3用，判断两个V之间是否有奇数个1，若为，后一个V之前的脉冲解释为0
    bool is_first_v_hdb3 = true;   //是否为第一个v，hdb3用
    bool is_first_h_hdb3 = true;    //出现的第一个脉冲，若此脉冲上跳-为1，若它为下跳-为四连零的v

    int swit = ui->comboChannel->currentIndex();
    switch (swit) {
    case 0:
    case 1:
        /*
         * 单极性非归零码
         * 双极性非归零码 共用
         */
        for (*str; *str != '\0'; str++) {
            if(*str == 'h'){
                channel_decode.append('1');
                ui->textChDe->append(QString::asprintf("h 转换为 1"));
            }else{
                channel_decode.append('0');
                swit == 0?
                ui->textChDe->append(QString::asprintf("n 转换为 0")):
                ui->textChDe->append(QString::asprintf("l 转换为 0"));
            }
        }
        break;

    case 2:
    case 3:
        /*
         * 单极性归零码
         * 双极性归零码(AMI) 共用
         */
        for (*str; *str != '\0'; str++) {
            if(*str == 'h' || *str == 'l'){
                /*  h和l都应该被解释成1  */
                channel_decode.append('1');
                ui->textChDe->append(QString::asprintf("h 转换为 1"));
            }else{
                /*  连续两个低电平为，解释为0  */
                if(last_level == *str){
                    channel_decode.append('0');
                    swit == 2?
                    ui->textChDe->append(QString::asprintf("n 转换为 0")):
                    ui->textChDe->append(QString::asprintf("l 转换为 0"));
                    /*  这样判断会存在差分0，导致0数量增多，所以当解释成0后，将上一个电平置成'i'(与电平不同就行)  */
                    last_level = 'i';
                    continue;
                }
            }
            /*  更新上一个电平状态  */
            last_level = *str;
        }
        break;

    case 4:
        /*
         * HDB3
         * 连续两个相同脉冲，第二个必是四连零中的V脉冲，解释为0
         * 编码时，若两个V脉冲之间偶数个1，则会将四连零中的第一个变成B脉冲 == 那么在解码时，两个V之间将会是奇数个脉冲
         * 那么，解码时，如果有奇数个脉冲，则V之前的脉冲必是B脉冲，解释为0
         *
         * 特别的是：当一来就四连零时，V脉冲没有参考，规定：编码时将第一个1设为上翻，一开始就四连零的V设为下翻，以示区别判断
         */
        for (*str; *str != '\0'; str++) {
            if(*str == 'n'){
                if(last_level == 'n'){
                    /*  连续两个零电平解释成0  */
                    channel_decode.append('0');
                    ui->textChDe->append(QString::asprintf("n 转换为 0"));
                    /*  这样判断会存在差分0，导致0数量增多，所以当解释成0后，将上一个电平置成'i'(与电平不同就行)  */
                    last_level = 'i';
                    /*  contine前必做的更新操作  */
                    count++;
                    continue;
                }
            }else{   //h or l
                if(is_first_h_hdb3){
                    /*  接收到的第一个脉冲，若此脉冲上跳--为1，若它为下跳--为四连零的v  */
                    if(*str == 'h'){
                        /*  上跳为1  */
                        channel_decode.append('1');
                        ui->textChDe->append(QString::asprintf("h 转换为 1"));
                    }else if(*str == 'l'){
                        /*  下跳为四连零的v，解释为0  */
                        channel_decode.append('0');
                        ui->textChDe->append(QString::asprintf("l 转换为 0"));
                        /*  其它的V操作，更新上一个V下标位置、两个v之间的1数量置0  */
                        v_count_hdb3 = 0;
                    }
                    is_first_h_hdb3 = false;
                }else{
                    /*  后续脉冲，与前一个脉冲比较  */
                    if(last_h_hdb3 == *str){
                        /*  连续两个相同属性脉冲，第二个必是V脉冲，将被解释成0  */
                        channel_decode.append('0');
                        ui->textChDe->append(QString::asprintf("V脉冲：%c 转换为 0", *str));
                        if(is_first_v_hdb3){
                            /*  如果是第一个V，将不进行如下判断  */
                            is_first_v_hdb3 = false;   //不再是第一个v
                        }else{
                            /*  判断与上一个V之间1的个数，奇数则将此V前一个脉冲修正解释为0  */
                            if((v_count_hdb3 % 2) != 0){
                                /*  修正B脉冲：1解释回0，向前三个0(6个取样周期)  */
                                channel_decode.replace(count - 6, 1, '0');
                                ui->textChDe->append(QString::asprintf("修正决定：上一个V脉冲向前数第三个从 1 修正为 0 vsum = %d", v_count_hdb3));
                            }
                        }
                        /*  更新上一个V下标位置、两个v之间的1数量置0  */
                        v_count_hdb3 = 0;
                    }else{
                        /*  两个相反脉冲，解释成1  */
                        channel_decode.append('1');
                        ui->textChDe->append(QString::asprintf("h 转换为 1"));
                        if(!is_first_v_hdb3){
                            /*  第一个V到达以后所有时间里，计V数量  */
                            v_count_hdb3++;
                        }
                    }
                }
                /*  更新上一个脉冲状态  */
                last_h_hdb3 = *str;  //h or l
            }
            /*  更新上一个电平状态  */
            last_level = *str;
            count++;
        }
        break;

    case 5:
        /*
         * CMI
         * 前平后高解释成0
         * 连续两个相同的电平解释成1
         * 其它不解释
         */
        for (*str; *str != '\0'; str++) {
            if(*str == 'h'){
                if(last_level  == 'n'){
                    /*  前平后高，解释成0  */
                    channel_decode.append('0');
                    ui->textChDe->append(QString::asprintf("n h 转换为 0"));
                }else if(last_level == 'h'){
                    /*  前高后高，相同电平，解释成1  */
                    channel_decode.append('1');
                    ui->textChDe->append(QString::asprintf("h h 转换为 1"));
                    /*  这样判断会存在差分1，导致1数量增多，所以当解释成1后，将上一个电平置成'i'(与电平不同就行)  */
                    last_level = 'i';
                    continue;
                }
            }else{
                if(last_level  == 'n'){
                    /*  前低后低，相同电平，解释成1  */
                    channel_decode.append('1');
                    ui->textChDe->append(QString::asprintf("n n 转换为 1"));
                    /*  这样判断会存在差分1，导致1数量增多，所以当解释成1后，将上一个电平置成'i'(与电平不同就行)  */
                    last_level = 'i';
                    continue;
                }
            }
            /*  更新上一个电平状态  */
            last_level = *str;
        }
        break;
    }

    /*  打印  */
    ui->textChDe->append("\n\n解码结果：");
    ui->textChDe->append(channel_decode);
    ui->textChDe->append("\n解码完成 ... ... ");

    ui->textSouDe->append(source_code);


    if(channel_decode == source_code)
        qDebug() << "equal";
    else
        qDebug() << "not equal";
}

void MainWindow::on_btnSouDecode_clicked()
{
    /*
     * 开始信源解码
     */
}

void MainWindow::on_btnJump3_clicked()
{
    /*
     * 最后一个页面重置，调用action重置
     */

    on_actionclear_all_triggered();
}

void MainWindow::on_btnBack1_clicked()
{
    /*
     * 页面回退至0
     */

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btnBack2_clicked()
{
    /*
     * 页面回退至1
     */

    ui->stackedWidget->setCurrentIndex(1);
}
