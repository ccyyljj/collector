#include "myTracer.h"

MyTracer::MyTracer(QCustomPlot *_plot, TracerType _type, QObject *parent) : plot(_plot),
type(_type), QObject(parent)
{
	if (plot)
	{

		tracer = new QCPItemTracer(plot);
		tracer->setStyle(QCPItemTracer::tsCircle);
		tracer->setPen(QPen(QColor(235, 100, 0)));
		tracer->setBrush(QColor(235, 100, 0));
		tracer->setSize(7);

		switch (type) {
		case XAxisTracer:
		{
			tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
			tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);

			label = new QCPItemText(plot);
			label->setLayer("overlay");
			label->setClipToAxisRect(false);
			label->setPadding(QMargins(5, 5, 5, 5));
			label->setBrush(QBrush(QColor(0, 0, 0, 200)));
			label->setPen(QPen(Qt::black));
			label->position->setParentAnchor(tracer->position);
			label->setFont(QFont("宋体", 10));
			label->setColor(Qt::white);
			label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
			label->position->setCoords(0, 0);

			label->setVisible(false);
			break;
		}
		case YAxisTracer:
		{
			tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
			tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

			arrow = new QCPItemLine(plot);
			QPen  arrowPen(Qt::black, 1);
			arrow->setPen(arrowPen);
			arrow->setLayer("overlay");
			arrow->setClipToAxisRect(false);
			arrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
			arrow->end->setParentAnchor(tracer->position);
			arrow->start->setParentAnchor(label->position);
			arrow->start->setCoords(-20, 0);//偏移量
			
			label = new QCPItemText(plot);
			label->setLayer("overlay");
			label->setClipToAxisRect(false);
			label->setPadding(QMargins(5, 5, 5, 5));
			label->setBrush(QBrush(QColor(0, 0, 0, 200)));
			label->setPen(QPen(Qt::black));
			label->position->setParentAnchor(tracer->position);
			label->setFont(QFont("宋体", 10));
			label->setColor(Qt::white);
			label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);
			arrow->setVisible(false);
			label->setVisible(false);
			break;
		}
		case DataTracer:
		{
			tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
			tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

			label = new QCPItemText(plot);
			label->setLayer("overlay");
			label->setClipToAxisRect(false);
			label->setPadding(QMargins(5, 5, 5, 5));
			label->setBrush(QBrush(QColor(0, 0, 0, 200)));
			label->setPen(QPen(Qt::black));
			label->position->setParentAnchor(tracer->position);
			label->setFont(QFont("宋体", 10));
			label->setColor(Qt::white);
			label->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label->position->setCoords(25, 0);

			arrow = new QCPItemLine(plot);
			QPen  arrowPen(Qt::black, 1);
			arrow->setPen(arrowPen);
			arrow->setLayer("overlay");
			arrow->setClipToAxisRect(false);
			arrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状
			arrow->end->setParentAnchor(tracer->position);
			arrow->start->setParentAnchor(arrow->end);
			arrow->start->setCoords(25, 0);

			arrow->setVisible(false);
			label->setVisible(false);
			break;		
		}
		case CrossLine:
		{
			line = new QCPItemStraightLine(plot);//直线
			line->setLayer("overlay");
			line->setClipToAxisRect(true);

			line->point1->setTypeX(QCPItemPosition::ptPlotCoords);
			line->point1->setTypeY(QCPItemPosition::ptPlotCoords);
			line->point2->setTypeX(QCPItemPosition::ptPlotCoords);
			line->point2->setTypeY(QCPItemPosition::ptPlotCoords);
			line->setVisible(false);
			break;
		}
		default:
			break;
		}
		tracer->setVisible(false);	
	}
}

MyTracer::~MyTracer()
{
	if (tracer)
		plot->removeItem(tracer);
	if (label)
		plot->removeItem(label);
}

void MyTracer::setPen(const QPen &pen)
{
	tracer->setPen(pen);
	arrow->setPen(pen);
}

void MyTracer::setBrush(const QBrush &brush)
{
	tracer->setBrush(brush);
}

void MyTracer::setLabelPen(const QPen &pen)
{
	label->setPen(pen);
}

void MyTracer::setText(const QString &text)
{
	label->setText(text);
}

void MyTracer::setVisible(bool visible)
{
	tracer->setVisible(visible);
	label->setVisible(visible);
	arrow->setVisible(visible);
	line->setVisible(visible);
}

void MyTracer::updatePosition(double xValue, double yValue)
{
	
	if (yValue > plot->yAxis->range().upper)
		yValue = plot->yAxis->range().upper;
	switch (type) {
	case XAxisTracer:
	{
		if (!visible)
		{
			tracer->setVisible(true);
			label->setVisible(true);
			visible = true;
		}
		tracer->position->setCoords(xValue, 1);
		setText(QString::number(xValue));
		break;
	}
	case YAxisTracer:
	{
		if (!visible)
		{
			tracer->setVisible(true);
			label->setVisible(true);
			arrow->setVisible(true);
			visible = true;
		}
		tracer->position->setCoords(0, yValue);
		label->position->setCoords(-60, 0);
		//arrow->start->setCoords(-20, 0);
		//arrow->end->setCoords(0, 0);
		setText(QString::number(yValue));
		break;
	}
	case DataTracer:
	{
		if (!visible)
		{
			tracer->setVisible(true);
			label->setVisible(true);
			arrow->setVisible(true);
			visible = true;
		}
		tracer->position->setCoords(xValue, yValue);
		//setText(toQString(MStrTools::Time2String(xValue)) + "\n "+ QString::number(yValue, 'f', 1));
		setText( QString::number(yValue));
		break;
	}
	case CrossLine:
	{
		if (!visible)
		{
			line->setVisible(true);
			visible = true;
		}
		line->point1->setCoords(xValue, yValue);
		line->point2->setCoords(xValue, yValue - 100.0);

		break;
	}

	default:
		break;
	}
}
