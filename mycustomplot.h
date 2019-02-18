#pragma once

#include "qcustomplot.h"
#include "myTracer.h"

class MyCustomPlot : public QCustomPlot
{
	Q_OBJECT
public:
	MyCustomPlot(QWidget *parent);
	~MyCustomPlot();
public slots:
	void slo_paintProgressCircle();
	void slo_clearProgressCircle();
	void slo_myMouseMoveEvent(QMouseEvent* event);
private:
	MyTracer * m_xTracer;
	MyTracer * m_datatracer;
	MyTracer * m_lineTracer;
};
