#ifndef VERTEX_H
#define VERTEX_H

#include <QtGlobal>

QT_BEGIN_NAMESPACE
template<class Type>
class List;
QT_END_NAMESPACE

class Vertex
{
public:
    Vertex(quint32 id);
    ~Vertex();

    quint32 getId() const;

    const QString *getName() const {
        return name;
    }

    const List<Vertex>* getSource() const {
        return source;
    }

    const List<Vertex>* getTarget() const {
        return targets;
    }

private:
    quint32 id;

    QString *name;

    List<Vertex> *source, *targets;
};

#endif // VERTEX_H
