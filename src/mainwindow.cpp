
#include <QtGui>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialog>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QFontComboBox>

#include "mainwindow.h"
#include "dialogitem.h"
#include "dialogjoint.h"
#include "dialogscene.h"

#include <fstream>

const int InsertTextButton = 10;

MainWindow::MainWindow()
{
	createActions();
	createToolBox();
	createMenus();

	scene = new DialogScene(itemMenu, this);
	scene->setSceneRect(QRectF(0, 0, 5000, 5000));
	connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)),
            this, SLOT(textInserted(QGraphicsTextItem*)));
	createToolbars();

	scene->setBackgroundBrush(QPixmap(":/res/bg.png"));

	QHBoxLayout *layout = new QHBoxLayout;
    view = new QGraphicsView(scene);
	view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	layout->addWidget(view);
	layout->addWidget(toolBox);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);

	setCentralWidget(widget);
	setWindowTitle(tr("Dialog Editor"));
	setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::deleteItem()
{
	foreach (QGraphicsItem *item, scene->selectedItems()) {
		if (item->type() == DialogJoint::Type) {
			scene->removeItem(item);
			DialogJoint *joint = qgraphicsitem_cast<DialogJoint *>(item);
			joint->startItem()->removeJoint(joint);
            joint->endItem()->removeJoint(joint);
		}
	}

	foreach (QGraphicsItem *item, scene->selectedItems()) {
		if (item->type() == DialogItem::Type) {
			qgraphicsitem_cast<DialogItem *>(item)->removeJoints();
		}
        scene->removeItem(item);
	}
}

void MainWindow::isolateItem()
{
	foreach (QGraphicsItem *item, scene->selectedItems()) {
		if (item->type() == DialogJoint::Type) {
			scene->removeItem(item);
			DialogJoint *joint = qgraphicsitem_cast<DialogJoint *>(item);
			joint->startItem()->removeJoint(joint);
			joint->endItem()->removeJoint(joint);
			delete item;
		}
	}

	foreach (QGraphicsItem *item, scene->selectedItems()) {
		if (item->type() == DialogItem::Type) {
			qgraphicsitem_cast<DialogItem *>(item)->removeJoints();
		}
	}
}


void MainWindow::pointerGroupClicked(int)
{
	scene->setMode(DialogScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::textInserted(QGraphicsTextItem *)
{
	pointerButton->setChecked(true);
	scene->setMode(DialogScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::currentFontChanged(const QFont &)
{
	handleFontChange();
}

void MainWindow::fontSizeChanged(const QString &)
{
	handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString &scale)
{
	double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	QMatrix oldMatrix = view->matrix();
	view->resetMatrix();
	view->translate(oldMatrix.dx(), oldMatrix.dy());
	view->scale(newScale, newScale);
}

void MainWindow::handleFontChange()
{
	QFont font = fontCombo->currentFont();
	font.setPointSize(fontSizeCombo->currentText().toInt());
	scene->setFont(font);
}

void MainWindow::itemSelected(DialogItem *textItem)
{
    QFont font = textItem->font();
	fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Dialog Editor"),
                       tr("<p><b>Dialog Graph Editor</b>, the QT-based dialog creation tool.</p>"));
}

void MainWindow::createToolBox()
{

    QWidget *itemWidget = new QWidget;
	QGridLayout *backgroundLayout = new QGridLayout;

	// Font combo
	fontCombo = new QFontComboBox();
	connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
			this, SLOT(currentFontChanged(QFont)));

	// Font size combo
	fontSizeCombo = new QComboBox;
	fontSizeCombo->setEditable(true);
	for (int i = 8; i < 30; i = i + 2)
		fontSizeCombo->addItem(QString().setNum(i));
	QIntValidator *validator = new QIntValidator(2, 64, this);
	fontSizeCombo->setValidator(validator);
	fontSizeCombo->setCurrentIndex(2);
	connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(fontSizeChanged(QString)));

	// Scale combo
	sceneScaleCombo = new QComboBox;
	QStringList scales;
	scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
	sceneScaleCombo->addItems(scales);
	sceneScaleCombo->setCurrentIndex(2);
	connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(sceneScaleChanged(QString)));


	// Font configuration frame:
	QGridLayout *fontLayout = new QGridLayout;
	fontLayout->addWidget(new QLabel(tr("Face:")), 0, 0, Qt::AlignLeft);
	fontLayout->addWidget(fontCombo, 1, 0, Qt::AlignLeft);
	fontLayout->addWidget(new QLabel(tr("Size:")), 2, 0, Qt::AlignLeft);
	fontLayout->addWidget(fontSizeCombo, 3, 0, Qt::AlignLeft);

	QGroupBox *fontGroup = new QGroupBox(tr("Font configuration:"));
	fontGroup->setFlat(true);
	fontGroup->setLayout(fontLayout);

	// Add
	backgroundLayout->addWidget(fontGroup);

	// Scene view frame:
	QGridLayout *viewLayout = new QGridLayout;
	viewLayout->addWidget(new QLabel(tr("Scale:")), 0, 0, Qt::AlignLeft);
	viewLayout->addWidget(sceneScaleCombo, 1, 0, Qt::AlignLeft);
	QGroupBox *viewGroup = new QGroupBox(tr("Scene view:"));
	viewGroup->setFlat(true);
	viewGroup->setLayout(viewLayout);

	// Add
	backgroundLayout->addWidget(viewGroup);


	backgroundLayout->setRowStretch(2, 10);
	backgroundLayout->setColumnStretch(2, 10);

	QWidget *backgroundWidget = new QWidget;
	backgroundWidget->setLayout(backgroundLayout);

	toolBox = new QToolBox;
	toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	toolBox->setMinimumWidth(212);
	toolBox->addItem(itemWidget, tr("Node Properties"));
	toolBox->addItem(backgroundWidget, tr("Global Properties"));
}

void MainWindow::callCheckGraph() {
    QMessageBox msgBox(QMessageBox::Information, "Graph info", scene->dfd.dfs()?"acyclic":"not acyclic");
    msgBox.addButton("Ok" ,QMessageBox::AcceptRole);
    msgBox.exec();
}

void MainWindow::createActions()
{
	deleteAction = new QAction(QIcon(":/res/~delete.png"), tr("&Delete"), this);
	deleteAction->setShortcut(tr("Delete"));
	deleteAction->setStatusTip(tr("Delete item from diagram"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

	isolateAction = new QAction(QIcon(":/res/~isolate.png"), tr("&Isolate"), this);
	isolateAction->setShortcut(tr("CTRL+I"));
	isolateAction->setStatusTip(tr("Clear all joints from the item"));
	connect(isolateAction, SIGNAL(triggered()), this, SLOT(isolateItem()));

	newFileAction = new QAction(QIcon(":/res/~new.png"), tr("&New"), this);
	newFileAction->setShortcut(tr("CTRL+N"));
	newFileAction->setStatusTip(tr("New dialog"));
	connect(newFileAction, SIGNAL(triggered()), this, SLOT(clear()));

	loadFileAction = new QAction(QIcon(":/res/~open.png"), tr("&Load"), this);
	loadFileAction->setShortcut(tr("CTRL+O"));
	loadFileAction->setStatusTip(tr("Loads a dialog"));
	connect(loadFileAction, SIGNAL(triggered()), this, SLOT(load()));

    checkGraph = new QAction(QIcon(":/res/symbol_check.png"), tr("&CheckGraph"), this);
    checkGraph->setStatusTip(tr("Check graph"));
    connect(checkGraph, SIGNAL(triggered()), this, SLOT(callCheckGraph()));

	saveFileAction = new QAction(QIcon(":/res/~save.png"), tr("&Save"), this);
	saveFileAction->setShortcut(tr("CTRL+S"));
	saveFileAction->setStatusTip(tr("Saves a dialog"));
	connect(saveFileAction, SIGNAL(triggered()), this, SLOT(save()));

	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Quit Scenediagram example"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	aboutAction = new QAction(tr("A&bout"), this);
	aboutAction->setShortcut(tr("Ctrl+B"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newFileAction);
	fileMenu->addAction(loadFileAction);
    fileMenu->addAction(saveFileAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu->addAction(deleteAction);
	itemMenu->addSeparator();
	itemMenu->addAction(isolateAction);

	aboutMenu = menuBar()->addMenu(tr("&Help"));
	aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars()
{
	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(newFileAction);
	editToolBar->addAction(loadFileAction);
    editToolBar->addAction(saveFileAction);
    editToolBar->addAction(checkGraph);

	pointerButton = new QToolButton;
	pointerButton->setCheckable(true);
	pointerButton->setChecked(true);
	pointerButton->setIcon(QIcon(":/res/~move.png"));
	pointerButton->setStatusTip(tr("Select and move"));
	nodeButton = new QToolButton;
	nodeButton->setCheckable(true);
	nodeButton->setChecked(true);
	nodeButton->setIcon(QIcon(":/res/~insert.png"));
	nodeButton->setStatusTip(tr("Insert dialog node"));
	linePointerButton = new QToolButton;
	linePointerButton->setCheckable(true);
	linePointerButton->setIcon(QIcon(":/res/~joint.png"));
	linePointerButton->setStatusTip(tr("Link dialog nodes"));
    QToolButton *customButton = new QToolButton;
    customButton->setCheckable(true);
    customButton->setIcon(QIcon(":/res/User-icon.png"));
    customButton->setStatusTip(tr("Insert user dialog node"));
    QToolButton *databaseButton = new QToolButton;
    databaseButton->setCheckable(true);
    databaseButton->setIcon(QIcon(":/res/database.png"));
    databaseButton->setStatusTip(tr("Insert database dialog node"));
    QToolButton *userProccessButton = new QToolButton;
    userProccessButton->setCheckable(true);
    userProccessButton->setIcon(QIcon(":/res/user_process.png"));
    userProccessButton->setStatusTip(tr("Insert user dialog node"));

	pointerTypeGroup = new QButtonGroup(this);
	pointerTypeGroup->addButton(nodeButton, int(DialogScene::InsertText));
	pointerTypeGroup->addButton(pointerButton, int(DialogScene::MoveItem));
	pointerTypeGroup->addButton(linePointerButton, int(DialogScene::InsertLine));
    pointerTypeGroup->addButton(customButton, int(DialogScene::InsertCustom));
    pointerTypeGroup->addButton(databaseButton, int(DialogScene::DataBaseItem));
    pointerTypeGroup->addButton(userProccessButton, int(DialogScene::UserProccessItem));
	connect(pointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(pointerGroupClicked(int)));

	pointerToolbar = addToolBar(tr("Pointer type"));
	pointerToolbar->addWidget(pointerButton);
	pointerToolbar->addWidget(nodeButton);
	pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(customButton);
    pointerToolbar->addWidget(databaseButton);
    pointerToolbar->addWidget(userProccessButton);
}

void MainWindow::save()
{
    QFileDialog::Options options;
	QString selectedFilter = "*.dlg";
	QString fileName = QFileDialog::getSaveFileName(this,
					   tr("Save a dialog"),
					   myFileName,
					   tr("Dialog Files (*.dlg)"),
					   &selectedFilter,
					   options);

	if (!fileName.isEmpty()) {
		myFileName = fileName;

        std::ofstream file(myFileName.toStdString());
        file << (quint32)0x21646c67; // header
        file << ' ' << (quint32)100; // version
        scene->dfd.serialize(file);
		file.close();
	}
}

void MainWindow::load()
{
	QFileDialog::Options options;
	QString selectedFilter = "*.dlg";
	QString fileName = QFileDialog::getOpenFileName(this,
					   tr("Load a dialog"),
					   myFileName,
					   tr("Dialog Files (*.dlg)"),
					   &selectedFilter,
					   options);

	if (!fileName.isEmpty()) {
        quint32 header, version;
		myFileName = fileName;
        std::ifstream file(myFileName.toStdString());

        clear();

        file >> header;
        if (header != 0x21646c67) {
            QMessageBox msgBox(QMessageBox::Warning,
                                tr("Read error"),
                                tr("<p><b>Wrong header.</b></p><p>Not a valid file format, yo.</p>"), 0, this);
            msgBox.addButton(tr("Oh, dear..."), QMessageBox::AcceptRole);
            msgBox.exec();
            return;
        }

        file >> version;
        if (version > 100) {
            QMessageBox msgBox(QMessageBox::Warning,
                                tr("Read error"),
                                tr("<p>Wrong file version!</p><p>File version is > 1.00. I cannot read this.</p>"), 0, this);
            msgBox.addButton(tr("Oh, dear..."), QMessageBox::AcceptRole);
            msgBox.exec();
            return;
        } else if (version < 100) {
            QMessageBox msgBox(QMessageBox::Warning,
                                tr("Read error"),
                                tr("<p>Wrong file version!</p><p>File version is < 1.00. This should <b>never happen</b>, maybe the file is broken?</p>"), 0, this);
            msgBox.addButton(tr("Oh, dear..."), QMessageBox::AcceptRole);
            msgBox.exec();
            return;
        }

        auto createNode = [&](DFD<DialogItem, DialogJoint>::node* n) {
            DialogItem *textItem = new DialogItem();
            textItem->node = n;
            textItem->setFont(scene->font());
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DialogItem*)), this, SLOT(editorLostFocus(DialogItem*)));
            scene->addItem(textItem);
            return textItem;
        };

        auto createEdge = [&]() {
            DialogJoint *joint = new DialogJoint(scene);
            return joint;
        };

        scene->dfd.deserialize(file, createNode, createEdge);
        scene->setIdCounter(10000);
		file.close();
	}
}

void MainWindow::clear()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Are you really really sure?"),
								  tr("<p>There's maybe something unsaved.<br>Would you wish to <b>erase the scene?</b></p>"),
								  QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {

		foreach (QGraphicsItem *item, scene->items()) {
			if (item->type() == DialogJoint::Type) {
				scene->removeItem(item);
				DialogJoint *joint = qgraphicsitem_cast<DialogJoint *>(item);
				joint->startItem()->removeJoint(joint);
				joint->endItem()->removeJoint(joint);
				delete item;
			}
		}

		foreach (QGraphicsItem *item, scene->items()) {
			if (item->type() == DialogItem::Type) {
				qgraphicsitem_cast<DialogItem *>(item)->removeJoints();
			}
			scene->removeItem(item);
			delete item;
		}
	}

	scene->setIdCounter(0);

}
