
#ifndef DIALOGITEM_H
#define DIALOGITEM_H

#include <QtWidgets/QGraphicsTextItem>
#include <QPen>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class DialogJoint;

class DialogItem : public QGraphicsTextItem
{
	Q_OBJECT

public:
	enum { Type = UserType + 3 };

    DialogItem(QGraphicsItem *parent = 0);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QRectF boundingRect() const;
	QPainterPath shape() const;

	void addJoint(DialogJoint *joint);
	void removeJoint(DialogJoint *joint);
	void removeJoints();

	int type() const { return Type; }

   // myId accessors
   void setId(const quint32 &id) { myId = id; }
	quint32 id() const { return myId; }

signals:
	void lostFocus(DialogItem *item);
	void selectedChange(QGraphicsItem *item);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void focusOutEvent(QFocusEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
	quint32 myId;
	QPolygonF boundingBox;
	QList<DialogJoint *> joints;
};

#endif
