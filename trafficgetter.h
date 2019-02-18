#ifndef TRAFFICGETTER_H
#define TRAFFICGETTER_H

#include <QObject>

//此类用来统计网络流量
class TrafficGetter : public QObject
{
    Q_OBJECT
public:
    explicit TrafficGetter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TRAFFICGETTER_H
