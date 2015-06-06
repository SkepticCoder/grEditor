
#include <QtGui>

#include "dialogjoint.h"
#include <math.h>

const qreal Pi = 3.14;

DialogJoint::DialogJoint(DialogItem *startItem, DialogItem *endItem,
             QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
	myStartItem = startItem;
	myEndItem = endItem;
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	myColor = QColor(50, 70, 200, 35);
	setPen(QPen(myColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF DialogJoint::boundingRect() const
{
	qreal extra = (pen().width() + 20) / 2.0;

	return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
									  line().p2().y() - line().p1().y()))
		   .normalized()
		   .adjusted(-extra, -extra, extra, extra);
}

QPainterPath DialogJoint::shape() const
{
	QPainterPath path = QGraphicsLineItem::shape();
	path.addPolygon(arrowHead);
	return path;
}

void DialogJoint::updatePosition()
{
	QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
	setLine(line);
}

void DialogJoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
				  QWidget *)
{
	if (myStartItem->collidesWithItem(myEndItem)) return;

	QPen myPen = pen();
	myPen.setColor(myColor);
	qreal arrowSize = 17;
	painter->setPen(myPen);
	painter->setBrush(myColor);
    painter->setRenderHint(QPainter::Antialiasing);

    QPointF start = myStartItem->boundingRect().center() + myStartItem->pos();
    QPointF end = myEndItem->boundingRect().center() + myEndItem->pos();
    QLineF centerLine(start, end);
    {
        QPainterPath path;
        path.moveTo(start);
        path.lineTo(end);
        QPointF point = path.pointAtPercent(0.5);
        qreal angle = path.angleAtPercent(0.5);
        QFontMetrics fm(painter->font());
        QString str = "edge";

        painter->save();
        if((angle > 90) && (angle < 270)) {
            painter->translate(point);
            painter->rotate(180 - angle);
            painter->drawText(QPoint(-fm.width(str) / 2, -myPen.width()), str);
        } else {
            painter->translate(point);
            painter->rotate(-angle);
            painter->drawText(QPoint(-fm.width(str) / 2, -myPen.width()), str);
        }

        painter->restore();
    }

	QPolygonF startPolygon = myStartItem->shape().toFillPolygon();
	QPolygonF endPolygon = myEndItem->shape().toFillPolygon();

	QPointF ip1 = getIntersection(centerLine, endPolygon, myEndItem->pos());
	QPointF ip2 = getIntersection(centerLine, startPolygon, myStartItem->pos());

    setLine(QLineF(ip1, ip2));

	double angle = ::acos(line().dx() / line().length());
	if (line().dy() >= 0)
		angle = (Pi * 2) - angle;

	QPointF jointP1 = line().p1() + QPointF(sin(angle + Pi / 2.3) * arrowSize,
											cos(angle + Pi / 2.3) * arrowSize);
	QPointF jointP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 2.3) * arrowSize,
											cos(angle + Pi - Pi / 2.3) * arrowSize);

	arrowHead.clear();
	arrowHead << line().p1() << jointP1 << jointP2;

	painter->drawLine(line());
	painter->drawPolygon(arrowHead);
	if (isSelected()) {
		painter->setPen(QPen(myColor, 1, Qt::DashLine));
		QLineF myLine = line();
		myLine.translate(0, 4.0);
		painter->drawLine(myLine);
		myLine.translate(0,-8.0);
		painter->drawLine(myLine);
	}

}

QPointF DialogJoint::getIntersection(QLineF line, QPolygonF poly, QPointF offset)
{
	QPointF p1 = poly.first() + offset;
	QPointF p2;
	QPointF intersectPoint;
	QLineF polyLine;
	for (int i = 1; i < poly.count(); ++i) {
		p2 = poly.at(i) + offset;
		polyLine = QLineF(p1, p2);
		QLineF::IntersectType intersectType =
				polyLine.intersect(line, &intersectPoint);
		if (intersectType == QLineF::BoundedIntersection) break;
		p1 = p2;
	}
	return intersectPoint;
}
