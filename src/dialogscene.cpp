
#include <QtGui>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QtWidgets/QMessageBox>

#include "dialogscene.h"
#include "dialogjoint.h"

DialogScene::DialogScene(QMenu *itemMenu, QObject *parent)
	: QGraphicsScene(parent)
{
	myItemMenu = itemMenu;
	myMode = MoveItem;
    line = 0;

	myFont = QFont( "tahoma" );
	myFont.setPointSize(10);
	myFont.setWeight(QFont::Normal);
	myFont.setItalic(false);
	myFont.setUnderline(false);

}

DialogItem *DialogScene::getItemById(quint32 id)
{
	foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == DialogItem::Type) {
			DialogItem *textItem = qgraphicsitem_cast<DialogItem *>(item);
			if (textItem->id() == id) return textItem;
		}
	}
	return NULL;
}

void DialogScene::setFont(const QFont &font)
{
	myFont = font;

    if (isItemChange(DialogItem::Type)) {
		QGraphicsTextItem *item =
            qgraphicsitem_cast<DialogItem *>(selectedItems().first());
		if (item)
			item->setFont(myFont);
	}
}

void DialogScene::setMode(Mode mode)
{
    myMode = mode;
}

void DialogScene::editorLostFocus(DialogItem *item)
{
	QTextCursor cursor = item->textCursor();
	cursor.clearSelection();
	item->setTextCursor(cursor);

	if (item->toPlainText().isEmpty()) {
		removeItem(item);
		item->deleteLater();
	}
}

void DialogScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DialogItem* textItem;
	if (mouseEvent->button() != Qt::LeftButton) return;

	switch (myMode) {

	case InsertLine:
		line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
											mouseEvent->scenePos()));
		line->setPen(QPen(QColor(50, 50, 200, 80), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
		addItem(line);
		break;

	case InsertText:
        textItem = new DialogItem();
        textItem->node = &dfd.createNode(textItem);
		textItem->setFont(myFont);
		textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
		textItem->setZValue(1000.0);
        connect(textItem, SIGNAL(lostFocus(DialogItem*)), this, SLOT(editorLostFocus(DialogItem*)));
        addItem(textItem);
		textItem->setPos(mouseEvent->scenePos());
		textItem->setId(myIdCounter++);

		emit textInserted(textItem);
        break;
    case InsertCustom:
        textItem = new DialogItem();
        textItem->node = &dfd.createNode(textItem);
        textItem->setFont(myFont);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        textItem->setPlainText("custom item");
        connect(textItem, SIGNAL(lostFocus(DialogItem*)), this, SLOT(editorLostFocus(DialogItem*)));
        addItem(textItem);
        textItem->setPos(mouseEvent->scenePos());
        textItem->setId(myIdCounter++);

        emit textInserted(textItem);
        break;
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void DialogScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (myMode == InsertLine && line != 0) {
		QLineF newLine(line->line().p1(), mouseEvent->scenePos());
		line->setLine(newLine);
	} else if (myMode == MoveItem) {
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
}

void DialogScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (line != 0 && myMode == InsertLine) {
		QList<QGraphicsItem *> startItems = items(line->line().p1());
		if (startItems.count() && startItems.first() == line)
			startItems.removeFirst();
		QList<QGraphicsItem *> endItems = items(line->line().p2());
		if (endItems.count() && endItems.first() == line)
			endItems.removeFirst();

		removeItem(line);
		delete line;

		if (startItems.count() > 0 && endItems.count() > 0 &&
                startItems.first()->type() == DialogItem::Type &&
                endItems.first()->type() == DialogItem::Type &&
				startItems.first() != endItems.first()) {
			DialogItem *startItem =
				qgraphicsitem_cast<DialogItem *>(startItems.first());
			DialogItem *endItem =
				qgraphicsitem_cast<DialogItem *>(endItems.first());
            DialogJoint *joint = new DialogJoint(startItem, endItem, this);
            startItem->addJointOut(joint);
            endItem->addJointIn(joint);
            dfd.linkNodes(*startItem->node, *endItem->node, joint);
            joint->label = QInputDialog::getText(nullptr, QString("Enter edge name"), QString("name:"));
			joint->setZValue(-1000.0);
			addItem(joint);
			joint->updatePosition();
		}
	}

	line = 0;
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool DialogScene::isItemChange(int type)
{
	foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
			return true;
	}
	return false;
}
