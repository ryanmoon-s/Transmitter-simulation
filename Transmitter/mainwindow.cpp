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
    ui->btnSouDecode->setEnabled(false);
    ui->btnJump2->setEnabled(false);

    /*  other   */
    create_chart();
    th = NULL;

    /*  test for ijstack   */
//    ijStack duck;
//    duck.push(1,1);
//    duck.push(2,2);
//    duck.push(3,3);
//    duck.push(4,4);
//    duck.push(3,3);
//    duck.push(6,4);

//    for(int i = 0; i < ELEM_NUM; i++){
//        elem e = duck.pop();
//        qDebug() << "i" << e.i << " j" << e.j;
//    }

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
            if(s_code[i].code.value == -1)
                continue;
            pri = QString::asprintf("有 %-3d 个 %c ，编码为 %s\n", s_code[i].count, s_code[i].code.value, s_code[i].code.codes);
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
    ui->btnCalSource->setEnabled(false);
    ui->btnStartSouce->setEnabled(false);
    ui->btnSouDecode->setEnabled(false);
    ui->textBrowser1->clear();
    memset(s_code, 0, sizeof (s_code));
    memset(_code_, 0, sizeof (_code_));
    for(int i = 0; i < CODENUM; i++){
        s_code[i].code.value = -1;
        s_code[i].probability = -1;
        s_code[i].s_probability = 0;
        _code_[i].value = -1;
        count_arr[i] = 0;
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

void MainWindow::on_comboSource_currentIndexChanged(int index)
{
    /*
     * 信源编码方式选择
     */

    Q_UNUSED(index);
    ui->btnCalSource->setEnabled(false);
    ui->btnStartSouce->setEnabled(false);
    ui->btnJump1->setEnabled(false);
    ui->btnSumm->setEnabled(false);

    /*  清空操作  */
    ui->textBrowser1->clear();
    memset(s_code, 0, sizeof (s_code));
    memset(_code_, 0, sizeof (_code_));
    for(int i = 0; i < CODENUM; i++){
        s_code[i].code.value = -1;
        s_code[i].probability = -1;
        s_code[i].s_probability = 0;
        _code_[i].value = -1;
        count_arr[i] = 0;
    }
}

void MainWindow::on_btnStartSouce_clicked()
{
    /*
     * 1 信源编码
     * 2 将码元三要素(值、码、父串下标)抽象出来填入_code_，方便信源解码统一调用
     */

    ui->btnJump1->setEnabled(true);
    ui->btnCalSource->setEnabled(true);
    ui->btnStartSouce->setEnabled(false);

    ui->textBrowser1->clear();
    source_code.clear();
    QByteArray bstr = fileSource.toLocal8Bit();
    const char *str = bstr;
    int i;

    switch (ui->comboSource->currentIndex()) {
    case 0:
        /*    制作字符串    */
        for(*str; *str != '\0'; str++){
            /*  找到编码  */
            for (i = 0; i < CODENUM; i++) {
                if(s_code[i].code.value == int(*str)){
                    break;
                }
            }
            source_code.append(QString::asprintf("%s", s_code[i].code.codes));
        }
        ui->textBrowser1->append(source_code);
        break;
    }

    ui->textBrowser1->append("\n\n\n\n编码完成 ... ... ");

    /*  装载_code_，反序，switch case  */
    _code_num_ = 0;
    _code_max_ = 0;
    for(i = CODENUM - 1; i >= 0; i--){
        //qDebug() <<  s_code[i].code.value;
        if(s_code[i].code.value != -1){
            _code_[_code_num_].value = s_code[i].code.value;
            for(int j = 0; j < 16; j++){
                _code_[_code_num_].codes[j] = s_code[i].code.codes[j];
            }
            if(Utils::arr_length(_code_[_code_num_].codes) > _code_max_){
                /*  求编码的最大长度  */
                _code_max_ = Utils::arr_length(_code_[_code_num_].codes);
            }
            /*  _code_num_为装的码个数  */
            _code_num_++;
        }
    }
}

void MainWindow::on_btnJump1_clicked()  //进入信道编码
{
    /*  进入之前清空操作  */
    ui->textRecCh->clear();
    ui->labSourceCode->setText(ui->comboSource->currentText());
    ui->comboChannel->setCurrentIndex(0);
    ui->spinBox->setValue(0);

    ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::on_btnJump2_clicked()
{
    /*
     * 进入解码
     */
    /*  按键  */
    ui->btnSouDecode->setEnabled(false);
    ui->btnChDecode->setEnabled(true);

    /*  读取信道编码结果  */
    channel_code = ui->textRecCh->document()->toPlainText();

    /*  设置相关文本，，清空文本区域，跳转  */
    ui->lab_Chde->setText(ui->comboChannel->currentText());
    ui->lab_SouDe->setText(ui->comboSource->currentText());
    ui->textChDe->clear();
    ui->textSouDe->clear();

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
    ui->textRecCh->clear();
    ui->line_code->clear();
    QVector<QPointF> point;
    series->replace(point);

    /*  启动线程  */
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
         * 注意QString的比较、替换，都是针对字符串，要用"" 而非''
         */
        for (*str; *str != '\0'; str++) {
            if(*str == 'n'){
                if(last_level == 'n'){
                    /*  连续两个零电平解释成0  */
                    channel_decode.append("0");
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
                        channel_decode.append("1");
                        ui->textChDe->append(QString::asprintf("h 转换为 1"));
                    }else if(*str == 'l'){
                        /*  下跳为四连零的v，解释为0  */
                        channel_decode.append("0");
                        ui->textChDe->append(QString::asprintf("l 转换为 0"));
                        is_first_v_hdb3 = false;   //不再是第一个v
                    }
                    is_first_h_hdb3 = false;
                }else{
                    /*  后续脉冲，与前一个脉冲比较  */
                    if(last_h_hdb3 == *str){
                        /*  连续两个相同属性脉冲，第二个必是V脉冲，将被解释成0  */
                        channel_decode.append("0");
                        ui->textChDe->append(QString::asprintf("V脉冲：%c 转换为 0", *str));
                        if(is_first_v_hdb3){
                            /*  如果是第一个V，将不进行如下判断  */
                            is_first_v_hdb3 = false;   //不再是第一个v
                        }else{
                            /*  判断V向前数三个是否是1，如果是1，则修正回0  */
                            if(channel_decode.at(channel_decode.count() - 1 - 3) == "1"){
                                /*  修正B脉冲：1解释回0，向前三个0(6个取样周期)  */
                                channel_decode.replace(channel_decode.count() - 1 - 3, 1, "0");
                                ui->textChDe->append(QString::asprintf("修正决定：上一个V脉冲向前数第三个从 1 修正为 0"));
                            }
                        }
                    }else{
                        /*  两个相反脉冲，解释成1  */
                        channel_decode.append("1");
                        ui->textChDe->append(QString::asprintf("h 转换为 1"));
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
    /*  按键  */
    ui->btnSouDecode->setEnabled(true);
    ui->btnChDecode->setEnabled(false);

    if(channel_decode == source_code)
        qDebug() << "equal" << endl;
    else
        qDebug() << "not equal" << endl;
}

void MainWindow::on_btnSouDecode_clicked()
{
    /*
     * 开始信源解码
     */
    ui->btnSouDecode->setEnabled(false);

    /*
     * 方案一：
     * 将所有的码制作成一个s字符串，并求得码开始的下标i
     * 在需要解的码，从1到n进行取样，求其在s中的位置，与下标i比较，如果相同则匹配成功，读取对应的value
     * 匹配成功，翻译到目标串；匹配失败，n继续+
     * 匹配成功后，又从n + 1到m进行取样，查找
     * 母串：00100
     * 子串：00001，第一个0就在母串中找到了位置，读取下标0的value
     * 又来一个0，还是匹配到下标0；又来一个0，还是匹配到下标0；又来一个0，还是匹配到下标0
     * 严重错误，方案失败
     */

    /*
     * 方案二：
     * 在需要解的码，从1到n进行取样，与已知编码数组中的码进行匹配
     * 匹配成功，翻译到目标串；匹配失败，n继续+
     * 匹配成功后，又从n + 1到m进行取样，查找
     * 0100  01000
     * 如果来的是01000，结果却匹配成了0100
     * 长码匹配成短，方案失败
     */

    /*
     * 方案三：
     * 规避回退算法：(能避免方案二的长码匹配成短码的问题)
     * 保存编码的最大长度_code_max
     * 写一个类维护一个存储ij数值结构体的栈，栈只存少量元素，push时，满了就自动去掉最底元素。pop返回ij结构体，空时返回ij等于-1的结构体
     *
     * 循环中：
     * 每匹配到就将ij入栈，在解码串中添加value
     * 如果匹配串长度达到_code_max还没有成功，就出栈，解码串回删一个
     * 判断：若栈不空，则主流程ij重新赋值成出栈的元素。再次匹配，直至栈空
     * 若栈空(ij=-1)，说明已经是最初状态，在这个状态继续j+1，匹配。
     *
     * 想的是错的码，会在接下的_code_max再次出错，回退现场
     * 理想：000100 01100 1100 1110 1001...
     *        T      h    e   space
     * 实际：000100 0110 0110 011101001
     *        T      a    a
     * 实际：000100 0110 01100 1110 1001...
     *        T      a    h   space
     * 而错的地方，刚好与正确的码接上，不会再错，又正常运行了下去。错误将不再纠正
     * 对于程序来说没有错，正常运行，而逻辑不对。
     *
     * 由于编码时，是以概率升序排列，而应该以降序排列，造成了非唯一编码，才出现第二种、第三种方案的失败，第二种方案就可以成功了。
     * 不会存在一个编码是另一个编码的子串情况。
     * 第三种方案的栈并未用上。
     */

    source_decode.clear();
    int c_count = channel_decode.count();

    bool find = false; //是否在_code_中匹配到
    int i, j;
    int source;  //拷贝源下标
    QString string;  //匹配子串
    ijStack stack;  //ij栈
    elem e;  //接收stack弹出元素

    for(i = 0; i < c_count; i++){
        for(j = i; j < i + _code_max_; j++){   //j为从i到_code_max_的偏移
            qDebug() << "结果串：" <<source_decode;
            qDebug() << "=========================";
            qDebug() << "i：" << i << " j：" <<j;

            /*  制作比较串  */
            string.clear();
            for(source = i; source <= j; source++){
                /*  从i到j(包括j)这一段拷贝到目标  */
                string.append(channel_decode.at(source));
            }
            qDebug() << "string：" << string;

            /*  在码元对数组中进行比较  */
            for(int fix = 0; fix < _code_num_; fix++){
                if(_code_[fix].codes == string){
                    /*  入栈  */
                    stack.push(i, j);
                    qDebug() << "push" << "i：" << i << " j：" << j;
                    /*  i 跳过匹配段长度，要考虑到执行完后i会自动加1 */

                    i += string.count() - 1;
                    /*  解码串添加匹配成果  */
                    source_decode.append(QString::asprintf("%c", _code_[fix].value));
                    /*  标志位设置(用来跳出j循环，i跳过匹配串长度)  */
                    find = true;
                    break;
                }
            }

            /*  上面匹配成功  */
            if(find){
                find = false;
                qDebug() << "jump to：" << "i：" << i + 1 << " j：" << i + 1;
                break;  //跳出j循环，i跳过匹配串长度
            }

            /*  防止j越界  */
            /*  一次最大长度匹配失败  */
            if(j == c_count || j == i + _code_max_ - 1){
                /*  解码串回删一个  */
                source_decode.remove(source_decode.count() - 1, 1);
                /*  出栈  */
                e = stack.pop();
                if(e.i != -1){
                    /*  栈未空，还原现场  */
                    i = e.i;
                    j = e.j;
                    qDebug() << "backto：" << "i：" << i << " j：" << j;
                }else{
                    /*  栈空，跳过一个i循环。i正常+1  */
                    qDebug() << "stack empty" ;
                    break;
                }
            }

        }/*  j循环  */
    }/*  i循环  */

    ui->textSouDe->append(source_decode);
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
    ui->btnSumm->setEnabled(false);
    ui->btnCalSource->setEnabled(false);

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btnBack2_clicked()
{
    /*
     * 页面回退至1
     */
    ui->btnChStart->setEnabled(true);

    ui->stackedWidget->setCurrentIndex(1);
}
