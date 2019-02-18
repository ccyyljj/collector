#include "dialog_changefreinfo.h"
#include "ui_dialog_changefreinfo.h"

Dialog_changeFreInfo::Dialog_changeFreInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_changeFreInfo)
{
    ui->setupUi(this);
}

Dialog_changeFreInfo::~Dialog_changeFreInfo()
{
    delete ui;
}

void Dialog_changeFreInfo::on_pushButton_ok_clicked()
{
    FreInfo freInfo;
    freInfo.sampleRate = ui->lineEdit_simpleRate->text().toDouble();
    freInfo.cutOffFre = ui->lineEdit_cutoffFre->text().toUInt();
    freInfo.resolution = ui->lineEdit_resolution->text().toDouble();
    freInfo.fillterRank = ui->lineEdit_burgRank->text().toUInt();
    emit sig_changeFreInfo(freInfo);
    emit close();
}

void Dialog_changeFreInfo::on_pushButton_cancle_clicked()
{
    emit close();
}
