//
// Created by ksm on 6/19/22.
//

#include "IntersectionAware.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Awares/IntersectionAwareSystem.h"
#include "Scene/Utilities/Utilites.h"

std::shared_ptr<IntersectionAware>
IntersectionAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<IntersectionAwareSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->p_UV = uv;
            return item;
        }
    }
    return nullptr;
}

void IntersectionAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<IntersectionAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

IntersectionAware::IntersectionAware(unsigned int oid): IComponent(oid, INTERSECTION_AWARE)
{
    IntersectionExists = false;
    FlipTrimming = false;
}

IntersectionAware::~IntersectionAware()
{
    UnregisterComponent();
}

QVector4D IntersectionAware::FindClosestPoints(std::shared_ptr<IntersectionAware> other, int density)
{
    QVector4D best_params;
    float best_dist = INFINITY;

    for (int v_step = 0; v_step < density; ++v_step)
    {
        float v = v_step * VMax / density + VMin;
        for (int u_step = 0; u_step < density; ++u_step)
        {
            float u = u_step * UMax / density + UMin;
            QVector3D p = SceneFunction({u, v});

            QVector2D other_params = other->FindClosestPoints(p, density);
            QVector3D other_p = other->SceneFunction(other_params);

            float dist = other_p.distanceToPoint(p);
            if (dist < best_dist)
            {
                best_dist = dist;
                best_params = {u, v, other_params.x(), other_params.y()};
            }
        }
    }

    return best_params;
}

QVector2D IntersectionAware::FindClosestPoints(QVector3D pos, int density)
{
    QVector2D best_params;
    float best_dist = INFINITY;

    for (int v_step = 0; v_step < density; ++v_step)
    {
        float v = v_step * VMax / density + VMin;
        for (int u_step = 0; u_step < density; ++u_step)
        {
            float u = u_step * UMax / density + UMin;

            QVector3D p = SceneFunction({u, v});
            float dist = pos.distanceToPoint(p);
            if (dist < best_dist)
            {
                best_dist = dist;
                best_params = {u, v};
            }
        }
    }

    return best_params;
}

bool IntersectionAware::ArgumentsInsideDomain(QVector2D args)
{
    args = WrapArgumentsAround(args);
    float u = args.x(), v = args.y();

    return u >= UMin and u <= UMax and v >= VMin and v <= VMax;
}

QVector2D IntersectionAware::WrapArgumentsAround(QVector2D args)
{
    float u = UWraps ? wrap(args.x(), UMin, UMax) : args.x();
    float v = VWraps ? wrap(args.y(), VMin, VMax) : args.y();

    return {u, v};
}

void IntersectionAware::SetTrimmingTexture(const QImage &img)
{
    IntersectionExists = true;

    TrimTexture = std::make_shared<QOpenGLTexture>(img);
    TrimTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void IntersectionAware::SetTrimmingUniforms(std::shared_ptr<ShaderWrapper> shader)
{
    if (TrimTexture and TrimTexture->isCreated())
    {
        TrimTexture->bind(0, QOpenGLTexture::ResetTextureUnit);
        shader->SetUniform("trimTexture", 0);
    }
    shader->SetUniform("u_FlipTrimming", FlipTrimming);
    shader->SetUniform("u_ActiveTrimming", IntersectionExists);
}
