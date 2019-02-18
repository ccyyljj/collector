#include "chartview.h"
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>
#include <QLineSeries>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    isClicking(false),
    xOld(0), yOld(0)
{
    //setRubberBand(QChartView::RectangleRubberBand);

}

ChartView::ChartView(QWidget *parent):
    QChartView(parent),
    isClicking(false),
    xOld(0), yOld(0)
{
    //setRubberBand(QChartView::RectangleRubberBand);

}

void ChartView::zoomIn(QPointF centerPoint)
{
    QValueAxis* xAxis = (QValueAxis*)chart()->axisX();
    QValueAxis* yAxis = (QValueAxis*)chart()->axisY();
    if(centerPoint.x() < xAxis->min()) //在y轴左侧触发滚轮,缩小y轴
    {
        double scale = (centerPoint.y() - yAxis->min())/(yAxis->max() - yAxis->min());
        yAxis->setMin(yAxis->min() + (yAxis->max() - yAxis->min())/10);
        qreal newMax = (qreal)(centerPoint.y() - yAxis->min())/scale +  yAxis->min();
        yAxis->setMax(newMax);
        return;
    }

    double scale = (centerPoint.x()  - xAxis->min()) / (xAxis->max() - xAxis->min());
    xAxis->setMin(xAxis->min() + (xAxis->max() - xAxis->min())/10);
    qreal newMax = (qreal)(centerPoint.x() - xAxis->min()) / scale + xAxis->min();
    xAxis->setMax(newMax);

}

void ChartView::zoomOut(QPointF centerPoint)
{
    QValueAxis* xAxis = (QValueAxis*)chart()->axisX();
    QValueAxis* yAxis = (QValueAxis*)chart()->axisY();
    if(centerPoint.x() < xAxis->min()) //在y轴左侧触发滚轮,放大y轴
    {
        double scale = (centerPoint.y() - yAxis->min())/(yAxis->max() - yAxis->min());
        yAxis->setMin(yAxis->min() - (yAxis->max() - yAxis->min())/10);
        double newMax = (centerPoint.y() - yAxis->min())/scale +  yAxis->min();
        yAxis->setMax(newMax);
        return;
    }
    double scale = (centerPoint.x()  - xAxis->min()) / (xAxis->max() - xAxis->min());
    xAxis->setMin(xAxis->min() - (xAxis->max() - xAxis->min())/10);
    qreal newMax = (qint64)(centerPoint.x() - xAxis->min()) / scale + xAxis->min();
    xAxis->setMax(newMax);
}



void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        isClicking = true;
    } else if (event->button() & Qt::RightButton) {//右键恢复缩放之前的x轴y轴比例,并恢复实时绘图模式
//        QDateTimeAxis* xAxis = (QDateTimeAxis*)chart()->axisX();
//        QValueAxis* yAxis = (QValueAxis*)chart()->axisY();
//        QDateTime xMin;
//        QList<QAbstractSeries *> list = chart()->series();
//        QLineSeries* series = (QLineSeries*)list.front();
//        if(series->pointsVector().isEmpty())
//        {
//            return;
//        }
//        xMin.setMSecsSinceEpoch(series->pointsVector().last().x());
//        xAxis->setMin(xMin);
//        QDateTime xMax(xMin.addSecs(10));
//        xAxis->setMax(xMax);

//        yAxis->setMin(-100.0);
//        yAxis->setMax(500.0);
    }

    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    int x, y;

    if (isClicking) {
        if (xOld == 0 && yOld == 0) {

        } else {
            x = event->x() - xOld;
            y = event->y() - yOld;
            chart()->scroll(-x, 0); //只能进行横向移动
        }

        xOld = event->x();
        yOld = event->y();
    }

    QChartView::mouseMoveEvent(event);

}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (isClicking) {
        xOld = yOld = 0;
        isClicking = false;
    }
    QChartView::mouseReleaseEvent(event);
}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QChartView::mouseDoubleClickEvent(event);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    //QList<QAbstractSeries *> list = chart()->series();

    QPointF centerPoint = chart()->mapToValue(event->posF());
    if (event->delta() > 0) {
        zoomIn(centerPoint);
    } else {
        zoomOut(centerPoint);
    }
    QChartView::wheelEvent(event);
}


