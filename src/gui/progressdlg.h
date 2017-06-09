#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>

class ProgressDlg : public QDialog
{
Q_OBJECT
public:
    explicit ProgressDlg(QWidget *parent = 0);

public:
    void setPercent(double percent);

private:
    QLabel* p_Operation;
    QProgressBar* p_Progress;

};

#endif // PROGRESSDLG_H
