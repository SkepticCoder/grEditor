#include "dataflowmodel.h"
#include <QMap>
#include "vertex.h"

DataFlowModel::DataFlowModel():
    data(new QMap<quint32, Vertex*>)
{

}

const Vertex* DataFlowModel::get(quint32 id) const
{
    return data->value(id, NULL);
}

void DataFlowModel::addVertex(Vertex &param)
{
    const Vertex *v = new Vertex(param);
    emit vertexAdded(v);
}

void DataFlowModel::updateVertex(Vertex &param)
{
    if(!data->contains(param.getId()))
        return;

    Vertex *v = new Vertex(param);
    data->insert(param.getId(), v);
    emit vertexUpdated(v);
}

void DataFlowModel::removeVertex(Vertex &param)
{
    const Vertex *v = new Vertex(param);
    emit vertexRemoved(v);
}

DataFlowModel::~DataFlowModel()
{

}

