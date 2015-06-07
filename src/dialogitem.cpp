
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

void DialogItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
{

    QColor color = (outputs.count() <= 1) ? QColor(70, 70, 250, 75) : QColor(250, 70, 70, 55);
    color = (outputs.count() == 0) ? QColor(0, 0, 0, 25) : color;
    painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(_type == Type) {
        painter->drawRoundedRect(0, 0, boundingRect().width(), boundingRect().height(), 7, 7);
    }
    if(_type == Type1) {
        painter->drawEllipse(0, 0, boundingRect().width(), boundingRect().height());
    }
    if(_type == Type2) {
        painter->fillRect(-2, -2, boundingRect().width(), boundingRect().height(), QBrush(QColor(64, 64, 64, 255)));
        painter->fillRect(0, 0, boundingRect().width(), boundingRect().height(), QBrush(QColor(128, 128, 255, 255)));
    }
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
	path.addRect(t);
     	return path;
}

void DialogItem::removeJoint(DialogJoint *joint)
{
    int index = inputs.indexOf(joint);
    if (index != -1)
        inputs.removeAt(index);
    index = outputs.indexOf(joint);
    if (index != -1)
        outputs.removeAt(index);
}

void DialogItem::removeJoints()
{
    foreach (DialogJoint *joint, outputs) {
        joint->startItem()->removeJoint(joint);
        joint->endItem()->removeJoint(joint);
        scene()->removeItem(joint);
        delete joint;
    }
    foreach (DialogJoint *joint, inputs) {
        joint->startItem()->removeJoint(joint);
        joint->endItem()->removeJoint(joint);
        scene()->removeItem(joint);
        delete joint;
    }
}

void DialogItem::addJointIn(DialogJoint *joint)
{
    inputs.append(joint);
}

void DialogItem::addJointOut(DialogJoint *joint)
{
    outputs.append(joint);
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

void DialogItem::serialize(std::ostream& out) const {
    out << '{';
    out << std::hex << (quint32)0x216f626a << "-" << (quint32)id();
    out << "[" << std::dec << pos().rx() << ':' << pos().ry() << "]";
    std::string str = toPlainText().toStdString();
    out << '(' << str.size() << " " << str << ')';
    out << '}';
}

bool DialogItem::deserialize(std::istream& in) {
    char c1, c2, c3;
    quint32 magic, _id;
    std::istream::pos_type pos = in.tellg();
    in >> c1;
    if(c1 != '{') {
        in.seekg(pos);
        return false;
    }
    in >> std::hex >> magic >> c1 >> _id;
    if(c1 != '-') {
        in.seekg(pos);
        return false;
    }
    if(magic != (quint32)0x216f626a) {
        in.seekg(pos);
        return false;
    }
    QString text;
    qreal x, y;
    in >> c1 >> std::dec >> x >> c2 >> y >> c3;
    if((c1 != '[')||(c2 != ':')||(c3 != ']')) {
        in.seekg(pos);
        throw DFD<DialogItem, DialogJoint>::bad_node_deserialize(pos, this);
    }
    size_t size;
    in >> c1 >> size;
    in.ignore(1);
    if(c1 != '(') {
        in.seekg(pos);
        throw DFD<DialogItem, DialogJoint>::bad_node_deserialize(pos, this);
    }
    char * buffer = new char [size + 1];
    in.read(buffer, size);
    std::string str(buffer, size);
    in >> c1 >> c2;
    if((c1 != ')')||(c2 != '}')) {
        in.seekg(pos);
        throw DFD<DialogItem, DialogJoint>::bad_node_deserialize(pos, this);
    }
    this->setPos(x, y);
    this->setPlainText(str.c_str());
    this->setId(_id);
    return true;
}
