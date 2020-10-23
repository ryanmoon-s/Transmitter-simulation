#include "dlghelp.h"
#include "ui_dlghelp.h"

DlgHelp::DlgHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgHelp)
{
    ui->setupUi(this);

    /*  QSS引入  */
    QDir qssdir = QApplication::applicationDirPath();
    QString qsspath = qssdir.path() + "/QtStyleSheet.qss";
    QFile qssFile(qsspath);
    qssFile.open(QFile::ReadOnly);
    QTextStream stream(&qssFile);
    QString stylesheet = stream.readAll();
    this->setStyleSheet(stylesheet);
    qssFile.close();
}

DlgHelp::~DlgHelp()
{
    delete ui;
}

void DlgHelp::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    QPen pen(Qt::NoPen);
    QRect rect(0, 0, width(), height());
    painter.setRenderHint(QPainter::Antialiasing);  //抗锯齿
    painter.setPen(pen);

    /*  背景图  */
    QPixmap map(":/new/prefix1/src/bk.jpg");
    painter.drawPixmap(rect, map);

    /*  蒙板  */
    QColor color(100, 100, 100, 150);
    brush.setColor(color);
    painter.setBrush(brush);

    painter.drawRect(rect);
}
