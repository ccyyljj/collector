#include "mycustomplot.h"


MyCustomPlot::MyCustomPlot(QWidget *parent)
	: QCustomPlot(parent)
{
	//跟踪器
	m_xTracer = new MyTracer(this, MyTracer::XAxisTracer, this);//x轴
	m_datatracer = new MyTracer(this, MyTracer::DataTracer, this); //数据点
	m_lineTracer = new MyTracer(this, MyTracer::CrossLine, this);//直线

	connect(this, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(slo_myMouseMoveEvent(QMouseEvent*)));
}

MyCustomPlot::~MyCustomPlot()
{
}

void MyCustomPlot::slo_paintProgressCircle()
{
	graph(0)->data()->clear();
	replot();
}

void MyCustomPlot::slo_clearProgressCircle()
{
	
}

void MyCustomPlot::slo_myMouseMoveEvent(QMouseEvent* event)
{
	if (this->graph(0)->dataCount() > 0)
	{
		int x_pos = event->pos().x();
		int y_pos = event->pos().y();

		float x_val = this->xAxis->pixelToCoord(x_pos);
		float y_val = this->yAxis->pixelToCoord(y_pos);
		m_xTracer->updatePosition(x_val, y_val);

		auto iter = this->graph(0)->data()->findBegin(x_val);
		double value = iter->mainValue();

		m_datatracer->updatePosition(x_val, value);
		m_lineTracer->updatePosition(x_val, y_val);

		replot();//曲线重绘	
	}
}