#ifndef DLGHELP_H
#define DLGHELP_H

#include <QDialog>

namespace Ui {
class DlgHelp;
}

class DlgHelp : public QDialog
{
    Q_OBJECT

public:
    explicit DlgHelp(QWidget *parent = nullptr);
    ~DlgHelp();

private:
    Ui::DlgHelp *ui;
};

#endif // DLGHELP_H
