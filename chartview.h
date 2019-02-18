#ifndef __CHARTVIEW_H__
#define __CHARTVIEW_H__

#include <QChartView>
#include <QRubberBand>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent = 0);
    ChartView(QWidget *parent);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    bool isClicking;

    int xOld;
    int yOld;


    void zoomIn(QPointF centerPoint);
    void zoomOut(QPointF centerPoint);
signals:
    void leftMarkClick(QPointF point);
    void rightMarkClick(QPointF point);

};

#endif /* __CHARTVIEW_H__ */
