#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QDebug>
#include <fstream>
#include <QFile>
#include <string>
#include <QStringList>
#include "dialog_changefreinfo.h"
#include "mycustomplot.h"
#include "myTracer.h"
#include <QReadWriteLock>
#include <QMutex>

#pragma execution_character_set("utf-8");

//定义全局变量
QMutex baliseVecMutex;
ConfigParameter configParameter;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netWorkInfo = "Disconnected";
    connectState = 0; //未连接
    isShowOriginalSimpleData = false; //默认不显示原始采样数据

    ui->statusBar->showMessage("连接断开");

    //-----------设置balise_tableView相关-----------------------------
    ui->balise_tableWidget->setColumnCount(3);
    ui->balise_tableWidget->setRowCount(0);
    QStringList titleList;
    titleList<< "应答器编号" << "接收时间" << "丢包数";
    ui->balise_tableWidget->setHorizontalHeaderLabels(titleList);

    //使行自适应高度，假如行很多的话，行的高度不会一直减小，当达到一定值时会自动生成一个QScrollBar
    //ui->balise_tableWidget->verticalHeader()->//setResizeMode(QHeaderView::Strtch);

    //设置单击选择一行
    ui->balise_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置每行内容不可编辑
    ui->balise_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置只能选择一行，不能选择多行
    ui->balise_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
//    //选中一行：
//    ui->balise_tableWidget->setCurrentCell(row, QItemSelectionModel::Select);//(注意此处的列没有值)
    ui->balise_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->textEdit_msg->setReadOnly(true);

    qRegisterMetaType<Balise>("Balise");
	qRegisterMetaType<QVector<double>>("QVector<float>");
    qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<QVector<SampleDataGain>>("QVector<SampleDataGain>");
    qRegisterMetaType<FreInfo>("FreInfo");
	qRegisterMetaType<QVector<unsigned char>>("QVector<unsigned char>");
	qRegisterMetaType<QVector<int>>("QVector<int>");
	

    //------绘图相关--------------------------------
    initChart();
    configParameter.fre_high = 4.516*1e6;
    configParameter.fre_low = 3.951*1e6;
    configParameter.fre_Resolution = 0.001*1e6;
    configParameter.sampleRate = 63.221*1e6;

    //------初始状态栏--------------------------------

}

MainWindow::~MainWindow()
{

    delete ui;

}

void MainWindow::startAllTh()
{

    //读取增益码字对照表文件
    std::ifstream fin_gain("gainSheet.csv"); //打开文件流操作
    std::string line;
    QStringList strList;
    QString qline;
    if(fin_gain.is_open())
    {
        while (std::getline(fin_gain, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
        {
            qline = QString::fromStdString(line);
            strList = qline.split(',');
            gainSheetVec.push_back(strList.back().toDouble());
        }
        fin_gain.close();
    }
    else
    {
        if(QMessageBox::information(this, "", "读取增益码字对照表文件gainSheet.csv失败！是否继续？", QMessageBox::Ok, QMessageBox::No) == QMessageBox::No)
        {
            this->close();
        }
    }


    //---------应答器数据管理类相关-------------------------------------------------
    baliseManager = new BaliseManager(NULL);
    baliseManager->moveToThread(&baliseManagerTh);
    //TODO信号与曹连接

    connect(&baliseManagerTh, &QThread::finished, baliseManager, &QObject::deleteLater);
    connect(baliseManager, &BaliseManager::sig_drawSampleDataWave, this, &MainWindow::slo_drawSampleDataWave);
    connect(baliseManager, &BaliseManager::sig_drawFreDataWave, this, &MainWindow::slo_drawFreDataWave);
    connect(baliseManager, &BaliseManager::sig_drawErrorRateWave, this, &MainWindow::slo_drawErrorRateWave);
    connect(this, &MainWindow::sig_drawHistorySampleDataWave, baliseManager, &BaliseManager::slo_getHistorySampleData2Draw);
    connect(this, &MainWindow::sig_drawHistoryFreDataWave, baliseManager, &BaliseManager::slo_getHistoryFreData2Draw);
    connect(this, &MainWindow::sig_drawHistoryErrorRateWave, baliseManager, &BaliseManager::slo_getHistoryErrorRate2Draw);
    connect(this, &MainWindow::sig_drawPhaseWave, baliseManager, &BaliseManager::slo_getBalisePhaseData2Draw);
    connect(baliseManager, &BaliseManager::sig_drawPhaseWave, this, &MainWindow::slo_drawPhaseWave);
	connect(baliseManager, &BaliseManager::sig_hasaBalise, this, &MainWindow::slo_put_item2balise_tableWidget);
	connect(this, &MainWindow::sig_review, baliseManager, &BaliseManager::slo_review);
    baliseManagerTh.start();



    //----------从FPGA获取数据相关------------------------------------------------
    dataGetter = new GetFPGAData(NULL);
    dataGetter->moveToThread(&getDataTh);
    //TODO信号与槽连接

    connect(&getDataTh, &QThread::finished, dataGetter, &QObject::deleteLater);
    connect(this, &MainWindow::sig_startListen, dataGetter, &GetFPGAData::slo_init);
    connect(dataGetter, &GetFPGAData::sig_sucess, this, &MainWindow::slo_GetFPGAThreadInitSucess);
    connect(dataGetter, &GetFPGAData::sig_error, this, &MainWindow::slo_GetFPGAThreadInitError);
    connect(dataGetter, &GetFPGAData::sig_showConnectState, this, &MainWindow::slo_showConnectedState);
    connect(this, &MainWindow::sig_endPcapLoop, dataGetter, &GetFPGAData::slo_pcapEndloop, Qt::BlockingQueuedConnection);
    connect(this, &MainWindow::sig_sendData, dataGetter, &GetFPGAData::slo_sendData2FPGA);
    connect(dataGetter, &GetFPGAData::sig_hasCommandRespons, this, &MainWindow::slo_hasCommandRespond);
    
    connect(dataGetter, &GetFPGAData::sig_hasaBalise, baliseManager, &BaliseManager::slo_add2BaliseVec);

    getDataTh.start();
    emit sig_startListen();
    //--------------------------------------------------------------------------


	



    //----------解调部分相关------------------------------------------------------


    //----------测试绘图-----------------------------------------------------------

//    ui->widget_sampleDataAll->setSelectionRectMode(QCP::srmSelect);
//    QVector<double> x(10000), y0(10000);
//    for (int i=0; i<10000; ++i)
//    {
//      x[i] = i;
//      y0[i] = qCos(i/10.0); // exponentially decaying cosine
//    }
//    ui->widget_sampleDataAll->addGraph();
//    ui->widget_sampleDataAll->addGraph();
//    ui->widget_sampleDataAll->graph(0)->setPen(QPen(Qt::blue));
//    ui->widget_sampleDataAll->graph(1)->setPen(QPen(Qt::blue));
//    ui->widget_sampleDataAll->graph(0)->setBrush(QBrush(Qt::blue));
//    ui->widget_sampleDataAll->graph(0)->setChannelFillGraph(ui->widget_sampleDataAll->graph(1));

//    ui->widget_sampleDataAll->graph(0)->setData(x, y0);
//    ui->widget_sampleDataAll->graph(1)->setData(x, y0);

//    ui->widget_sampleDataAll->graph(0)->rescaleAxes();
//    ui->widget_sampleDataAll->graph(1)->rescaleAxes();
//    // add two new graphs and set their look:
//    ui->widget_sampleDataAll->addGraph();
//    ui->widget_sampleDataAll->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph

    // generate some points of data (y0 for first, y1 for second graph):

    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
//    ui->widget_sampleData->xAxis2->setVisible(true);
//    ui->widget_sampleData->xAxis2->setTickLabels(false);
//    ui->widget_sampleData->yAxis2->setVisible(true);
//    ui->widget_sampleData->yAxis2->setTickLabels(false);
//    // make left and bottom axes always transfer their ranges to right and top axes:
//    connect(ui->widget_sampleData->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget_sampleData->xAxis2, SLOT(setRange(QCPRange)));
//    connect(ui->widget_sampleData->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widget_sampleData->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
   // ui->widget_sampleDataAll->graph(0)->setData(x, y0);

    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    //ui->widget_sampleDataAll->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):

    // Note: we could have also just called ui->widget_sampleData->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    //ui->widget_sampleData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::initChart()
{

     ui->widget_sampleData->setOpenGl(true);
     qDebug() << ui->widget_sampleData->openGl();
     ui->widget_sampleData->addGraph();
     ui->widget_sampleData->graph(0)->setPen(QPen(Qt::blue));
     ui->widget_sampleData->graph(0)->rescaleAxes();
     ui->widget_sampleData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
     connect(this, &MainWindow::sig_paintProgressCircle, ui->widget_sampleData, &MyCustomPlot::slo_paintProgressCircle);




     //ui->widget_fre->setOpenGl(true);
     qDebug() << ui->widget_fre->openGl();
     ui->widget_fre->addGraph();
     ui->widget_fre->graph(0)->setPen(QPen(Qt::blue));
     ui->widget_fre->graph(0)->rescaleAxes();
     ui->widget_fre->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	 connect(this, &MainWindow::sig_paintProgressCircle, ui->widget_fre, &MyCustomPlot::slo_paintProgressCircle);
	



	 ui->widget_errorBit->addGraph();
	 ui->widget_errorBit->graph(0)->setPen(QPen(Qt::blue));
	 ui->widget_errorBit->graph(0)->rescaleAxes();
	 ui->widget_errorBit->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	 connect(this, &MainWindow::sig_paintProgressCircle, ui->widget_errorBit, &MyCustomPlot::slo_paintProgressCircle);
//    sampleDataSeries = new QLineSeries(sampleDataChart);
//    sampleDataSeries->setColor(Qt::blue);
//    //sampleDataSeries->setUseOpenGL(true);

//    sampleDataChart->addSeries(sampleDataSeries);
//    sampleDataChart->createDefaultAxes();
//    sampleDataChart->legend()->hide(); //隐藏图例

//    ui->widget_sampleData->setChart(sampleDataChart);
//    ui->widget_sampleData->setRenderHint(QPainter::Antialiasing);  //抗锯齿渲染


}



void MainWindow::slo_GetFPGAThreadInitSucess(QString initInfo)
{
    netWorkInfo = initInfo;
}

void MainWindow::slo_GetFPGAThreadInitError(QString initInfo)
{
    if(QMessageBox::critical(this, "错误", QString("%1 关闭程序?").arg(initInfo),
                             QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok)
    {
        emit close();
    }
}

void MainWindow::slo_showConnectedState(int connectedState)
{
    this->connectState = connectedState;
    if(connectedState == 0)
    {
        ui->statusBar->setStyleSheet("color:red");
        ui->statusBar->showMessage(QString("连接断开  监听网口：%1").arg(netWorkInfo));
    }
    else
    {
        ui->statusBar->setStyleSheet("color:green");
        ui->statusBar->showMessage(QString("已连接  监听网口：%1").arg(netWorkInfo));
    }
}

void MainWindow::slo_hasCommandRespond()
{
    isRespond = true;
}

void MainWindow::slo_checkRespond()
{
    if(isRespond)
    {
        QMessageBox::information(this, "", "修改成功！", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, "", "修改失败！", QMessageBox::Ok);
        ui->comboBox_gate->setCurrentIndex(3);
        ui->comboBox_length->setCurrentIndex(2);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    switch( QMessageBox::information( this, "CT Control View",
      "关闭程序?",
      "确定", "取消", 0, 1 ) )
     {
        case 0:
            event->accept();
            qDebug() << "mainThread" << QThread::currentThreadId();

            //结束程序前释放线程
            emit sig_endPcapLoop();
            getDataTh.quit();
            getDataTh.wait();

            baliseManagerTh.quit();
            baliseManagerTh.wait();
            break;
        case 1:
            event->ignore();
        default:
            event->ignore();
            break;
    }
}








//主界面点击确认修改之后的槽函数
void MainWindow::slo_on_pushButtonChangeOk_clicked()
{

}


//菜单栏导入bin文件选项的槽函数
void MainWindow::on_action_bin_triggered()
{

    QStringList fileNameList;
    fileNameList = QFileDialog::getOpenFileNames(
                    this, "导入.bin", "", "Files(*.bin)");
	if (fileNameList.size() == 1)
	{
		emit sig_review(fileNameList[0]);
	}
}

/******************************************************************************************************
 * 接收从getFPGAData线程发来的有应答器信号，显示应答器接收序号，时间，丢包信息等
 * ****************************************************************************************************/
void MainWindow::slo_put_item2balise_tableWidget(const Balise &balise)
{
    int row = ui->balise_tableWidget->rowCount();
    ui->balise_tableWidget->insertRow(row);
    if(row - 1 >0)
    {
        ui->balise_tableWidget->scrollToItem( ui->balise_tableWidget->item(row-1,0), QAbstractItemView::PositionAtBottom);
    }

    ui->balise_tableWidget->setItem(row, 0, new QTableWidgetItem ("正在处理"));
    ui->balise_tableWidget->setItem(row, 1, new QTableWidgetItem (balise.recvTime.toString("hh:mm:ss:zzz")));
    ui->balise_tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(balise.lostPackageRecordVec.size())));
    ui->balise_tableWidget->setCurrentCell(row, 0);
}



void MainWindow::slo_drawSampleDataWave(const int &recvNum, const QVector<float> &sampleDataVec, const QVector<SampleDataGain> &gainVec)
{
	qDebug() << sizeof(float)*sampleDataVec.size();
    if(ui->balise_tableWidget->currentRow() !=  recvNum) //只有当前选中应答器和将要绘制的应答器序号相同才绘制
    {
        return;
    }
    else
    {
		QVector<QCPGraphData> *mData;
		mData = ui->widget_sampleData->graph(0)->data()->coreData();
		mData->clear();

		int j = 0;
		double gain;
		QCPGraphData newPoint;
		isShowOriginalSimpleData = (isShowOriginalSimpleData && !gainVec.isEmpty()) ? true : false;
		mData->resize(sampleDataVec.size());
		for (int i = 0; i<sampleDataVec.size(); ++i)
		{
			newPoint.key = i;
			if (isShowOriginalSimpleData)
			{
				if(i == gainVec.at(j).position)
				{
				    gain = gainSheetVec[gainVec.at(j).gain];
				    if(j < gainVec.size() - 1) j++;
				}
				newPoint.value = sampleDataVec.at(i)/gain/2.523;
			}
			else
			{
				newPoint.value = sampleDataVec.at(i);
			}
			  
			(*mData)[i] = newPoint;
		}
        //QVector<double> sampleDataPointX;
        //for(int i = 0; i < sampleDataVec.size(); i++)
        //{
        //    sampleDataPointX.push_back(i);
        //}
        //if(isShowOriginalSimpleData && !gainVec.isEmpty())
        //{
        //   int j = 0;
        //   double gain;
        //   for(int i = 0; i < sampleDataVec.size(); i++)
        //   {
        //       if(i == gainVec[j].position)
        //       {
        //           gain = gainSheetVec[gainVec[j].gain];
        //           if(j < gainVec.size() - 1) j++;
        //       }
        //       sampleDataVec[i] = sampleDataVec[i]/gain/2.523;
        //   }
        //}
        //ui->widget_sampleData->graph(0)->setData(sampleDataPointX, sampleDataVec, true);
        ui->widget_sampleData->graph(0)->rescaleAxes();
        ui->widget_sampleData->replot();
    }
//        QFile fout("gain.txt");
//        fout.open(QIODevice::WriteOnly|QIODevice::Text);
//        QTextStream ts(&fout);

//        for(int i = 0; i < gainVec.size(); i++)
//        {
//            ts << QString::number(gainVec[i].position) << "  " << QString::number(gainVec[i].gain) << endl;
//        }
//        fout.close();
}

void MainWindow::slo_drawFreDataWave(const Balise &balise, const QVector<float> &freVec)
{

    if(ui->balise_tableWidget->currentRow() != balise.recvNum) //只有当前选中应答器和将要绘制的应答器序号相同才绘制
    {
        return;
    }

    //绘图
	QVector<QCPGraphData> *mData = ui->widget_fre->graph(0)->data()->coreData();
	mData->clear();
	mData->resize(freVec.size());

	QCPGraphData newPoint;
	int len = freVec.size();
    for(int i = 0; i < len; i++)
    {
		newPoint.key = i * (63.221/(len*6));
		newPoint.value = freVec[i];
		(*mData)[i] = newPoint;
    }
    ui->widget_fre->graph(0)->rescaleAxes();
    ui->widget_fre->replot();

    //填写频谱部分lineEdit
    ui->lineEdit_FL->setText(QString::number(balise.FL[2]));
    ui->lineEdit_FL_bias->setText(QString::number(abs(balise.FL[2] - 3.951)));
    ui->lineEdit_FC->setText(QString::number(balise.FC[2]));
    ui->lineEdit_FC_bias->setText(QString::number(abs(balise.FC[2] - 4.234)));
    ui->lineEdit_FH->setText(QString::number(balise.FH[2]));
    ui->lineEdit_FH_bias->setText(QString::number(abs(balise.FH[2] - 4.516)));
}


/*************************************************************************************************
 * 此函数用于绘制相位折线，会绘制到采样数据位置上，之前的采样数据会被清除
 *
 *
 * ***********************************************************************************************/
void MainWindow::slo_drawPhaseWave(const int &recvNum, const QVector<float> &phaseDataVec)
{
    if(ui->balise_tableWidget->currentRow() != recvNum) //只有当前选中应答器和将要绘制的应答器序号相同才绘制
    {
        return;
    }

	QVector<QCPGraphData> *mData = ui->widget_sampleData->graph(0)->data()->coreData();
	mData->clear();
	mData->resize(phaseDataVec.size());

	QCPGraphData newPoint;
	int len = phaseDataVec.size();
	for (int i = 0; i < len; i++)
	{
		newPoint.key = i;
		newPoint.value = phaseDataVec[i];
		(*mData)[i] = newPoint;
	}
    ui->widget_sampleData->graph(0)->rescaleAxes();
    ui->widget_sampleData->replot();
}

/************************************************************************************
 * 此槽函数用于绘制误码率曲线，填写应答器id，填写关于解调的lineEdit
 *
 * **********************************************************************************/
void MainWindow::slo_drawErrorRateWave(const Balise &balise)
{
    if(balise.baliseID == "")//未得到有效报文
    {
        ui->balise_tableWidget->setItem(balise.recvNum, 0, new QTableWidgetItem ("无效数据"));
    }
    else
    {
        ui->balise_tableWidget->setItem(balise.recvNum, 0, new QTableWidgetItem (balise.baliseID));
    }

    if(ui->balise_tableWidget->currentRow() != balise.recvNum) //只有当前选中应答器和将要绘制的应答器序号相同才绘制
    {
        return;
    }

    //填写应答器ID和关于解调的lineEdit
    if(balise.baliseID == "")//未得到有效报文
    {
        ui->balise_tableWidget->setItem(ui->balise_tableWidget->currentRow(), 0, new QTableWidgetItem ("无效数据"));

        ui->lineEdit_bps->setText("未知");
        ui->lineEdit_bps_bias->setText("未知");

        ui->lineEdit_amp->setText("未知");
        ui->lineEdit_power->setText("未知");

        ui->lineEdit_totalBitNum->setText(QString::number(balise.demodedByteVec.size()*8));
        ui->lineEdit_totalMsgNum->setText(QString::number(balise.demodedByteVec.size()*8/1023));

        ui->lineEdit_firstCorrectBitPos->setText("未知");
        ui->lineEdit_totalCorrectBitNum->setText(QString::number(0));
        ui->lineEdit_correctMSGNum->setText(QString::number(0));

        ui->textEdit_msg->clear();
        ui->textEdit_msg->setPlainText("1023bit报文: 无\n");
        ui->textEdit_msg->setPlainText("原始接收bit:\n");
        QString demodedBytesStr;
        for(int i = 0; i < balise.demodedByteVec.size(); i++)
        {
            demodedBytesStr += QString::number(balise.demodedByteVec[i], 2);
        }
         ui->textEdit_msg->setPlainText(demodedBytesStr);


    }
    else //得到有效报文
    {
        ui->lineEdit_bps->setText(QString::number(balise.averTranSpeed));
        ui->lineEdit_bps_bias->setText(QString::number(abs(balise.averTranSpeed - 564.48)));


        ui->lineEdit_amp->setText("未知");
        ui->lineEdit_power->setText("未知");

        ui->lineEdit_totalBitNum->setText(QString::number(balise.demodedByteVec.size()*8));
        ui->lineEdit_totalMsgNum->setText(QString::number(balise.demodedByteVec.size()*8/1023));

        ui->lineEdit_firstCorrectBitPos->setText(QString::number(balise.firstCorrectBitPos));
        ui->lineEdit_totalCorrectBitNum->setText(QString::number(balise.totalCorrectBitNum));
        ui->lineEdit_correctMSGNum->setText(QString::number(balise.correctMSGNum));

        ui->textEdit_msg->clear();
        ui->textEdit_msg->setPlainText("1023bit报文: 无\n");
        ui->textEdit_msg->setPlainText("原始接收bit:\n");
        QString demodedBytesStr;
        for(int i = 0; i < balise.demodedByteVec.size(); i++)
        {
            demodedBytesStr += QString::number(balise.demodedByteVec[i], 16);
        }
        ui->textEdit_msg->setPlainText(demodedBytesStr);
    }

    //绘图
	QVector<QCPGraphData> *mData = ui->widget_errorBit->graph(0)->data()->coreData();
	mData->clear();
	mData->resize(balise.errorRateVec.size()*32);

	QCPGraphData newPoint;
	int len = balise.errorRateVec.size() * 32;
	for (int i = 0; i < len; i++)
	{
		newPoint.key = i;
		newPoint.value = balise.errorRateVec[i / 32];
		(*mData)[i] = newPoint;
	}

    ui->widget_errorBit->graph(0)->rescaleAxes();
    ui->widget_errorBit->replot();
}

/************************************************************************************************************************
 * 此函数用来发送显示主界面信息的信号，主要包括发送绘制采样数据信号，发送绘制频谱数据信号，发送绘制解调数据信号 到 baliseManager线程，
 * baliseManager会调用相应的getData槽来获取数据，并交由主界面显示，注意：发送绘制频谱数据信号，发送绘制解调数据信号后可能尚未获得所需
 * 数据，baliseManager中getData槽可能并不会返回数据交由主界面显示。因此baliseManager在处理完这两类数据后，应主动向主界面汇报
 *
 * **********************************************************************************************************************/

void MainWindow::on_groupBox_13_clicked()
{

}







void MainWindow::on_pushButtonChangeOk_clicked()
{
    if(connectState == 0)
    {
        QMessageBox::warning(this, "警告", "连接断开，无法通信！", QMessageBox::Ok);
        return;
    }
    else
    {
        unsigned char length = ui->comboBox_length->currentIndex();
        unsigned char gate = ui->comboBox_gate->currentIndex();
        isRespond = false;
        emit sig_sendData(length, gate);
        QTimer::singleShot(200, this, SLOT(slo_checkRespond()));
    }
}

void MainWindow::on_action_config_fre_triggered()
{
    Dialog_changeFreInfo* changeFreInfoDialog = new Dialog_changeFreInfo(this);
    changeFreInfoDialog->show();
    connect(changeFreInfoDialog, &Dialog_changeFreInfo::sig_changeFreInfo, baliseManager, &BaliseManager::slo_changeFreInfo);
}

/***********************************************************************************
 * 菜单栏 - 显示 - 采样数据显示
 * 改变采样值的显示方式，显示放大后采样值
 *
 * ********************************************************************************/
void MainWindow::on_action_show_sampleDataWave_triggered()
{
    ui->action_show_sampleDataWave->setChecked(true);
    ui->action_show_original_sampleDataWave->setChecked(false);
    isShowOriginalSimpleData = false;
    emit sig_drawHistorySampleDataWave(ui->balise_tableWidget->currentRow());
}

/***********************************************************************************
 * 菜单栏 - 显示 - 采样数据显示
 * 改变采样值的显示方式，显示相位折线
 *
 * ********************************************************************************/
void MainWindow::on_action_show_phaseWave_triggered()
{
    emit sig_drawPhaseWave(ui->balise_tableWidget->currentRow());
}

/***********************************************************************************
 * 菜单栏 - 显示 - 采样数据显示
 * 改变采样值的显示方式，显示原始采样值
 *
 * ********************************************************************************/
void MainWindow::on_action_show_original_sampleDataWave_triggered()
{
    ui->action_show_sampleDataWave->setChecked(false);
    ui->action_show_original_sampleDataWave->setChecked(true);
    isShowOriginalSimpleData = true;
    emit sig_drawHistorySampleDataWave(ui->balise_tableWidget->currentRow());
}

void MainWindow::on_balise_tableWidget_cellClicked(int row, int column)
{
	emit sig_paintProgressCircle();
    emit sig_drawHistorySampleDataWave(ui->balise_tableWidget->currentRow());
    emit sig_drawHistoryFreDataWave(ui->balise_tableWidget->currentRow());
    emit sig_drawHistoryErrorRateWave(ui->balise_tableWidget->currentRow());
}
