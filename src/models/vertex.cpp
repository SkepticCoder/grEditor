#include "vertex.h"

Vertex::Vertex(quint32 id):
    id(id)
{
}

Vertex::~Vertex()
{

}

quint32 Vertex::getId() const
{
    return id;
}

