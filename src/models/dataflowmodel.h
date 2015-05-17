#ifndef DATAFLOWMODEL_H
#define DATAFLOWMODEL_H

#include <QtGlobal>
#include <QObject>
#include "modellistener.h"

QT_BEGIN_NAMESPACE
template <class Key, class T> class QMap;
QT_END_NAMESPACE

class Vertex;


class DataFlowModel : public ModelListener
{
    Q_OBJECT

public:
    DataFlowModel();

    ~DataFlowModel();


    const Vertex *get(quint32 id) const;

public slots:

    void addVertex(Vertex &param);

    void updateVertex(Vertex &param);

    void removeVertex(Vertex &param);

private:
    QMap<quint32, Vertex*> * const data;
};

#endif // DATAFLOWMODEL_H
