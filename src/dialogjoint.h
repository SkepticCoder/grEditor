
#ifndef DIALOGJOINT_H
#define DIALOGJOINT_H

#include <QtWidgets/QGraphicsLineItem>

#include "dialogitem.h"

QT_BEGIN_NAMESPACE
//class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

//! [0]
class DialogJoint : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    DialogJoint(DialogItem *startItem, DialogItem *endItem,
      QGraphicsItem *parent = 0);

    int type() const
        { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)
        { myColor = color; }
    DialogItem *startItem() const
        { return myStartItem; }
    DialogItem *endItem() const
        { return myEndItem; }


public slots:
    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

private:
	QPointF getIntersection(QLineF line, QPolygonF poly, QPointF offset);

    DialogItem *myStartItem;
    DialogItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
};
//! [0]

#endif
