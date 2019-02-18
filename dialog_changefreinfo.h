#ifndef DIALOG_CHANGEFREINFO_H
#define DIALOG_CHANGEFREINFO_H

#include <QDialog>
#include "balisestructdefine.h"

namespace Ui {
class Dialog_changeFreInfo;
}

class Dialog_changeFreInfo : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_changeFreInfo(QWidget *parent = 0);
    ~Dialog_changeFreInfo();

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancle_clicked();

signals:
    void sig_changeFreInfo(FreInfo freInfo);

private:
    Ui::Dialog_changeFreInfo *ui;
};

#endif // DIALOG_CHANGEFREINFO_H
