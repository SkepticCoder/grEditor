
#ifndef DIALOGJOINT_H
#define DIALOGJOINT_H

#include <QtWidgets/QGraphicsLineItem>
#include "dfd.hpp"

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

class DialogItem;

class DialogJoint : public QGraphicsLineItem, public iserializable
{
public:
    enum { Type = UserType + 4 };

    DialogJoint(DialogItem *startItem, DialogItem *endItem, class DialogScene* scene,
      QGraphicsItem *parent = 0);
    DialogJoint(class DialogScene* scene,
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

    DialogItem* getStart() { return myStartItem; }
    DialogItem* getEnd() { return myEndItem; }
    QString label;
public slots:
    void updatePosition();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void serialize(std::ostream& out) const override;
    bool deserialize(std::istream& in) override;
private:
	QPointF getIntersection(QLineF line, QPolygonF poly, QPointF offset);

    DialogItem *myStartItem;
    DialogItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
    class DialogScene* scene;
};
//! [0]

#endif
