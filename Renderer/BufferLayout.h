//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_BUFFERLAYOUT_H
#define SMOCAD_BUFFERLAYOUT_H

#include <vector>
#include <QtGui>
#include "Scene/Utilities/TypeMap.h"

struct VertexBufferElement
{
    unsigned int type;
    int count;
    bool normalized;

    static int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:
                return sizeof(float);
            case GL_UNSIGNED_INT:
                return sizeof(unsigned int);
            case GL_UNSIGNED_BYTE:
                return sizeof(unsigned char);
            default:
                return 0;
        }
    }
};

class BufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    int m_Stride;
    TypeMap<unsigned int> glTypes;
public:
    BufferLayout() : m_Stride(0)
    {
        //[TODO] to moze byc static teoretycznie
        glTypes.put<float>(GL_FLOAT);
        glTypes.put<unsigned int>(GL_UNSIGNED_INT);
        glTypes.put<unsigned char>(GL_UNSIGNED_BYTE);
    }

    template<typename T>
    void Push(int count, bool normalized = false)
    {
        auto it = glTypes.find<T>();
        assert(it != glTypes.end());
        unsigned int glType = it->second;
        m_Elements.push_back({glType, count, normalized});
        m_Stride += count * VertexBufferElement::GetSizeOfType(glType);
    }

    [[nodiscard]] inline const std::vector<VertexBufferElement> &GetElements() const { return m_Elements; }

    [[nodiscard]] inline int GetStride() const { return m_Stride; }
};


#endif //SMOCAD_BUFFERLAYOUT_H
