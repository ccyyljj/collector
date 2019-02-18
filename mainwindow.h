#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QCloseEvent>
#include <QTimer>
#include "getfpgadata.h"
#include "balisemanager.h"
#include "GeneratedFiles/ui_mainwindow.h"


namespace Ui {
class MainWindow;
}

class GetFPGAData;
class BaliseManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void startAllTh();
	void closeEvent(QCloseEvent* event);
	void initChart();
    GetFPGAData* dataGetter;
private:



signals:

    void sig_hasBalise(int recvNum);
    void sig_getTheBalise(int num);  //向baliseManager索要指定位置的应答器信息

    void sig_test();
    //-----------GetFPGAData线程相关槽-----------------------------
    void sig_startListen();
    void sig_endPcapLoop();
    void sig_sendData(unsigned char length, unsigned char gate);

    void sig_drawHistorySampleDataWave(int recvNum);
    void sig_drawHistoryFreDataWave(int recvNum);
    void sig_drawHistoryErrorRateWave(int recvNum);
    void sig_drawPhaseWave(int recvNum);
	void sig_review(QString binFileName);

	void sig_paintProgressCircle();
	void sig_clearProgressCircle();

public slots:

    //-----------GetFPGAData线程相关槽-----------------------------
    void slo_GetFPGAThreadInitSucess(QString initInfo);
    void slo_GetFPGAThreadInitError(QString initInfo);
    void slo_showConnectedState(int connectedState);
    void slo_hasCommandRespond();
    void slo_checkRespond();



private slots:
    void slo_on_pushButtonChangeOk_clicked();
    void on_action_bin_triggered();
    void slo_put_item2balise_tableWidget(const Balise &balise);

    //-----------绘图相关槽----------------------------------------
    void slo_drawSampleDataWave(const int &recvNum, const QVector<float>& sampleDataVec, const QVector<SampleDataGain>& gainVec);
    void slo_drawFreDataWave(const Balise &balise, const QVector<float> &spectrum);
    void slo_drawPhaseWave(const int &recvNum, const QVector<float> &phaseDataVec);
    void slo_drawErrorRateWave(const Balise &balise);
    void on_groupBox_13_clicked();



    void on_pushButtonChangeOk_clicked();
    void on_action_show_sampleDataWave_triggered();

    void on_action_config_fre_triggered();

    void on_action_show_phaseWave_triggered();

    void on_action_show_original_sampleDataWave_triggered();

    void on_balise_tableWidget_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;

    QThread getDataTh;
    QThread baliseManagerTh;
    BaliseManager *baliseManager;

    QString netWorkInfo = ""; //当前监听的以太网网卡名称
    int connectState = 0; //当前连接状态
    bool isRespond = false;   //是否收到FPGA回复的标志
    //界面绘图相关
    QVector<double> gainSheetVec; //用来存储增益码字对照表

    //显示菜单相关
    bool isShowOriginalSimpleData; //是否显示原始采样值
    Balise nowBalise;

};

#endif // MAINWINDOW_H
