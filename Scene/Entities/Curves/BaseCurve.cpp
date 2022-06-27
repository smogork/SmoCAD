//
// Created by ksm on 4/22/22.
//

#include "BaseCurve.h"
#include "Scene/Utilities/Utilites.h"
#include "Renderer/Options.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Awares/CollectionAwareSystem.h"
#include "ThirdParty/Scene-Serializer/cpp/Serializer/Serializer/Scene/SerializerException.h"

BaseCurve::BaseCurve(unsigned int cid) : IEntity(cid)
{
    InitObject();
}

BaseCurve::BaseCurve(unsigned int cid, uint explicit_oid) : IEntity(cid, explicit_oid)
{
    InitObject();
}

void BaseCurve::InitObject()
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Collection = TransformCollection::CreateRegisteredComponent(GetObjectID()));

    InitializeDrawing();

    CurveColor = DefaultColor;
    PolylineColor.setBinding([this](){
        return this->m_curvePolyline.DrawingColor;
    });
    curvePolylineDrawing = Options::DrawBezierPolygon.addNotifier([this]()
                                                                  {
                                                                      this->m_curvePolyline.p_Drawing->Enabled = Options::DrawBezierPolygon;
                                                                  });
    curvePolylineColor = PolylineColor.addNotifier([this]()
                                                   {
                                                       this->m_curvePolyline.DrawingColor = PolylineColor;
                                                   });
}

void BaseCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData({});
    p_Drawing->SetIndexData({});
    p_Drawing->p_bufferLayout.Push<float>(4);//position
    if (auto sh = Renderer::GetShader(BEZIER_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&BaseCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&BaseCurve::UniformFunction);
}

void BaseCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawPatches(context->functions(), GetIndexCount());
}

void BaseCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(CurveColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    shader->GetRawProgram()->setPatchVertexCount(4);
}

void BaseCurve::CommonSerializeFunction(MG1::Bezier& b)
{
    b.SetId(GetObjectID());

    for (const auto& wel : p_Collection->GetPoints())
    {
        if (auto el = wel.lock())
            b.controlPoints.emplace_back(MG1::PointRef(el->GetAttachedObjectID()));
    }
}

void BaseCurve::CommonDeserializeFunction(const MG1::Bezier &b)
{
    SetObjectId(b.GetId());

    if (auto scene = SceneECS::Instance().lock())
    {
        for (const MG1::PointRef &ref: b.controlPoints)
        {
            if (auto el = scene->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(ref.GetId()).lock())
                p_Collection->AddPoint(el);
            else
                throw MG1::SerializerException("Unknown point during deserialization of Bezier curve");
        }
    }
}



