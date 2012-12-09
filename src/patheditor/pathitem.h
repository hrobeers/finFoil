#ifndef PATHITEM_H
#define PATHITEM_H

#include <QGraphicsItem>
#include <QList>

namespace patheditor
{
    class PathItem : public QGraphicsItem
    {
    public:
        explicit PathItem(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

        QSharedPointer<QPointF> startPoint;
        QSharedPointer<QPointF> endPoint;

        virtual int numberOfControlPoints() = 0;
        virtual QList<QSharedPointer<QPointF> > controlPoints() = 0;
    };
}

#endif // PATHITEM_H
