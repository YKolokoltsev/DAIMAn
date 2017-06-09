#include "progressdlg.h"
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include <QVBoxLayout>

ProgressDlg::ProgressDlg(QWidget *parent) :
    QDialog(parent)
{
    QRect scr = QApplication::desktop()->availableGeometry(0);
    scr.setRect(scr.width()/4,scr.height()/2-50,scr.width()/2,100);
    this->setGeometry(scr);
    this->setWindowTitle(QString("Progress of the current operation"));

    p_Operation = new QLabel(tr("Setting of current operation..."));
    p_Progress = new QProgressBar(this);
    p_Progress->setMinimum(0);
    p_Progress->setMaximum(100);

    QVBoxLayout* p_Layout = new QVBoxLayout();
    p_Layout->addWidget(p_Operation);
    p_Layout->addWidget(p_Progress);

    this->setLayout(p_Layout);
}

void ProgressDlg::setPercent(double percent)
{
    if(!p_Progress || percent < 0 || percent > 100) return;
    p_Progress->setValue(floor(percent));
}
