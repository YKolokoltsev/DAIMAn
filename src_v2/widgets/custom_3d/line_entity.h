//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DLINEENTITY_H
#define DAIMAN_DLINEENTITY_H

#include <vector>

#include <Qt3DCore>
#include <Qt3DRender>

struct Line3DVertex{
    QVector3D xyz;
    QVector3D color;
};

class LineEntity : public Qt3DCore::QEntity{
public:
    typedef std::vector<Line3DVertex> t_vertex_vec;
    typedef std::vector<unsigned int> t_index_vec;
    typedef Qt3DRender::QGeometryRenderer::PrimitiveType t_primitive_type;

    LineEntity(Qt3DCore::QEntity* root_entity = nullptr);
    void setGeometry(t_vertex_vec&, t_index_vec&, t_primitive_type);

private:
    Qt3DRender::QGeometryRenderer *customGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry;
    Qt3DRender::QBuffer *vertexDataBuffer;
    Qt3DRender::QBuffer *indexDataBuffer;
    Qt3DRender::QMaterial *vertex_material;

    Qt3DRender::QAttribute *positionAttribute;
    Qt3DRender::QAttribute *colorAttribute;
    Qt3DRender::QAttribute *indexAttribute;
};


#endif //DAIMAN_DLINEENTITY_H
