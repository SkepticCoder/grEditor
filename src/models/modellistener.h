#ifndef MODELLISTENER_H
#define MODELLISTENER_H

#include <QObject>
#include <QtGlobal>

class Vertex;

class ModelListener: public QObject
{
    Q_OBJECT

public:

    ModelListener();

    ~ModelListener();

signals:

    void vertexAdded(const Vertex *param);

    void vertexUpdated(const Vertex *param);

    void vertexRemoved(const Vertex *param);

};

#endif // MODELLISTENER_H
