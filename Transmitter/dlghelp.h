#ifndef DLGHELP_H
#define DLGHELP_H

#include <QDialog>
#include <QtWidgets>

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

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // DLGHELP_H
