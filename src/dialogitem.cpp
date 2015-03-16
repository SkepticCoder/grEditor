
#include <QtGui>

#include "dialogitem.h"
#include "dialogjoint.h"
#include "dialogscene.h"

DialogItem::DialogItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
	setDefaultTextColor(QColor(10, 10, 50, 200));
}

void DialogItem::paint(QPainter *painter,
				const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
{

	QColor color = (joints.count() <= 1) ? QColor(70, 70, 250, 75) : QColor(250, 70, 70, 55);
	color = (joints.count() == 0) ? QColor(0, 0, 0, 25) : color;
    painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

//	painter->fillRect(0, 0, boundingRect().width(), boundingRect().height(),
//						QBrush(QRadialGradient(10, 10, 20)));
	painter->drawRoundedRect(0, 0, boundingRect().width(), boundingRect().height(), 7, 7);
	QGraphicsTextItem::paint(painter, option, widget);
}

QRectF DialogItem::boundingRect() const
{
	const QRectF t = QGraphicsTextItem::boundingRect();
	return t;
}

QPainterPath DialogItem::shape() const
{
	const QRectF t = QGraphicsTextItem::boundingRect();
	QRectF t2;
	QPainterPath path;
//	path.addEllipse(t2);
	path.addRect(t);
     	return path;
}

void DialogItem::removeJoint(DialogJoint *joint)
{
    int index = joints.indexOf(joint);

    if (index != -1)
        joints.removeAt(index);
}

void DialogItem::removeJoints()
{
    foreach (DialogJoint *joint, joints) {
        joint->startItem()->removeJoint(joint);
        joint->endItem()->removeJoint(joint);
        scene()->removeItem(joint);
        delete joint;
    }
}

void DialogItem::addJoint(DialogJoint *joint)
{
    joints.append(joint);
}

QVariant DialogItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}

void DialogItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void DialogItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}
