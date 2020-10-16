#include "dlghelp.h"
#include "ui_dlghelp.h"

DlgHelp::DlgHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgHelp)
{
    ui->setupUi(this);
}

DlgHelp::~DlgHelp()
{
    delete ui;
}
