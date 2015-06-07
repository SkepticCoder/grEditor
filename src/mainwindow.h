
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
//#include <QtWidgets/QDialog>
//#include <QtWidgets/QToolBox>
//#include <QtWidgets/QComboBox>
//#include <QtWidgets/QButtonGroup>
//#include <QtWidgets/QToolButton>
//#include <QtWidgets/QFontComboBox>
#include "dialogitem.h"

class DialogScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private slots:
	void deleteItem();
	void isolateItem();
	void pointerGroupClicked(int id);
	void textInserted(QGraphicsTextItem *item);
	void currentFontChanged(const QFont &font);
	void fontSizeChanged(const QString &size);
	void sceneScaleChanged(const QString &scale);
	void handleFontChange();
	void itemSelected(DialogItem *textItem);
	void about();
	void save();
	void load();
	void clear();
    void callCheckGraph();

private:
	void createToolBox();
	void createActions();
	void createMenus();
	void createToolbars();

	QString myFileName;

	DialogScene *scene;
	QGraphicsView *view;

	QAction *newFileAction;
	QAction *loadFileAction;
    QAction *checkGraph;
	QAction *saveFileAction;

	QAction *exitAction;
	QAction *addAction;
	QAction *deleteAction;
	QAction *isolateAction;

	QAction *aboutAction;

	QMenu *fileMenu;
	QMenu *itemMenu;
	QMenu *aboutMenu;

	QToolBar *editToolBar;
	QToolBar *pointerToolbar;

	QComboBox *sceneScaleCombo;
	QComboBox *fontSizeCombo;
	QFontComboBox *fontCombo;

	QToolBox *toolBox;
	QButtonGroup *pointerTypeGroup;

	QToolButton *nodeButton;
	QToolButton *pointerButton;
	QToolButton *linePointerButton;
	QToolButton *textButton;

	QAction *textAction;
	QAction *lineAction;
};

#endif
