//
// Created by morrigan on 19/07/17.
//

#include <Qt3DExtras>

#include "line_entity.h"

LineEntity::LineEntity(Qt3DCore::QEntity * root_entity) :
        Qt3DCore::QEntity(root_entity),
        customGeometryRenderer(new Qt3DRender::QGeometryRenderer),
        positionAttribute(new Qt3DRender::QAttribute()),
        colorAttribute(new Qt3DRender::QAttribute()),
        indexAttribute(new Qt3DRender::QAttribute())
{

    customGeometry = new Qt3DRender::QGeometry(customGeometryRenderer);
    vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
    indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

    //point material, todo: not to this?
    vertex_material = new Qt3DExtras::QPerVertexColorMaterial(this);

    //Attributes
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(6 * sizeof(float));
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(colorAttribute);
    customGeometry->addAttribute(indexAttribute);
    this->addComponent(vertex_material);

    customGeometryRenderer->setInstanceCount(1);
    customGeometryRenderer->setGeometry(customGeometry);
    this->addComponent(customGeometryRenderer);
}

void LineEntity::setGeometry(t_vertex_vec& vertices, t_index_vec& indices, t_primitive_type primitive_type){
    //vertices
    QByteArray vertexBufferData;
    vertexBufferData.resize(vertices.size() * (3 + 3) * sizeof(float)); //xyz + rgb

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int i = 0;
    for(const auto &v : vertices) {
        rawVertexArray[i++] = v.xyz.x();
        rawVertexArray[i++] = v.xyz.y();
        rawVertexArray[i++] = v.xyz.z();

        rawVertexArray[i++] = v.color.x();
        rawVertexArray[i++] = v.color.y();
        rawVertexArray[i++] = v.color.z();
    }
    vertexDataBuffer->setData(vertexBufferData);
    positionAttribute->setCount(vertices.size());
    colorAttribute->setCount(vertices.size());

    //indices
    QByteArray indexBufferData;
    indexBufferData.resize(indices.size() * sizeof(unsigned int));

    unsigned int* rawIndexArray = reinterpret_cast<unsigned int *>(indexBufferData.data());
    i = 0;
    for(const auto & idx : indices)
        rawIndexArray[i++] = idx;
    indexDataBuffer->setData(indexBufferData);
    indexAttribute->setCount(indices.size());

    //setup renderer
    customGeometryRenderer->setPrimitiveType(primitive_type);
    //todo: can depend on primitive type?
    customGeometryRenderer->setVertexCount(indices.size());
}