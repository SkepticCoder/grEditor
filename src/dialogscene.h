
#ifndef DIALOGSCENE_H
#define DIALOGSCENE_H

#include <QtWidgets/QGraphicsScene>
#include "dialogitem.h"
#include "dialogjoint.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

class DialogScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem, InsertCustom };
    enum SaveResult { Ok, BadHeader, BadVersion, BadContent };

    DialogScene(QMenu *itemMenu, QObject *parent = 0);

    QFont font() const { return myFont; }
    void setFont(const QFont &font);

    // myIdCounter accessors
    void setIdCounter(const quint32 &idCounter) { myIdCounter = idCounter; }
    quint32 idCounter() const { return myIdCounter; }

	DialogItem *getItemById(quint32 id);
    DFD<DialogItem, DialogJoint> dfd;
public slots:
    void setMode(Mode mode);
    void editorLostFocus(DialogItem *item);

signals:
    void textInserted(QGraphicsTextItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    bool isItemChange(int type);

	quint32 myIdCounter;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
};

#endif
