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

IntersectionAware::IntersectionAware(unsigned int oid) : IComponent(oid, INTERSECTION_AWARE)
{
    IntersectionExists = false;
    FlipTrimming = false;
}

IntersectionAware::~IntersectionAware()
{
    UnregisterComponent();
}

QVector4D IntersectionAware::FindClosestPoints(std::shared_ptr<IntersectionAware> other, int samples)
{
    QVector4D best_params;
    float best_dist = INFINITY;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> Udist(UMin, UMax);
    std::uniform_real_distribution<float> Vdist(VMin, VMax);

    std::vector<QVector2D> oneArgs(samples), twoArgs(samples);

    for (int i = 0; i < samples; ++i)
    {
        float u1 = Udist(generator);
        float v1 = Vdist(generator);
        float u2 = Udist(generator);
        float v2 = Vdist(generator);

        oneArgs[i] = {u1, v1};
        twoArgs[i] = {u2, v2};
    }

    for (int j = 0; j < twoArgs.size(); ++j)
        for (int i = 0; i < oneArgs.size(); ++i)
        {
            QVector3D p = SceneFunction(oneArgs[i]);
            QVector3D other_p = other->SceneFunction(twoArgs[j]);

            float dist = other_p.distanceToPoint(p);
            if (dist < best_dist)
            {
                best_dist = dist;
                best_params = {oneArgs[i].x(), oneArgs[i].y(), twoArgs[j].x(), twoArgs[j].y()};
            }
        }

    return best_params;
}

QVector2D IntersectionAware::FindClosestPoints(QVector3D pos, int samples)
{
    QVector2D best_params;
    float best_dist = INFINITY;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> Udist(UMin, UMax);
    std::uniform_real_distribution<float> Vdist(VMin, VMax);

    for (int i = 0; i < samples; ++i)
    {
        float u = Udist(generator);
        float v = Vdist(generator);

        QVector3D p = SceneFunction({u, v});

        float dist = pos.distanceToPoint(p);
        if (dist < best_dist)
        {
            best_dist = dist;
            best_params = {u, v};
        }
    }
    return best_params;
}

QVector4D IntersectionAware::FindClosestPointsSelf(int samples)
{
    QVector4D best_params;
    float best_dist = INFINITY;
    const QVector2D minArgDist = {(UMax - UMin) / 20, (VMax - VMin) / 20};

    std::default_random_engine generator;
    std::uniform_real_distribution<float> Udist(UMin, UMax);
    std::uniform_real_distribution<float> Vdist(VMin, VMax);

    std::vector<QVector2D> oneArgs(samples), twoArgs(samples);

    for (int i = 0; i < samples; ++i)
    {
        float u1 = Udist(generator);
        float v1 = Vdist(generator);
        float u2 = Udist(generator);
        float v2 = Vdist(generator);

        oneArgs[i] = {u1, v1};
        twoArgs[i] = {u2, v2};
    }

    for (int j = 0; j < twoArgs.size(); ++j)
        for (int i = 0; i < oneArgs.size(); ++i)
        {
            QVector3D p = SceneFunction(oneArgs[i]);
            QVector3D other_p = SceneFunction(twoArgs[j]);

            float dist = other_p.distanceToPoint(p);
            if (dist < best_dist && abs(oneArgs[i].x() - twoArgs[j].x()) > minArgDist.x() &&
                abs(oneArgs[i].y() - twoArgs[j].y()) > minArgDist.y())
            {
                best_dist = dist;
                best_params = {oneArgs[i].x(), oneArgs[i].y(), twoArgs[j].x(), twoArgs[j].y()};
            }
        }

    return best_params;
}

QVector2D IntersectionAware::FindClosestPointsFarFromArgs(QVector3D pos, QVector2D args, int samples)
{
    QVector2D best_params = {NAN, NAN};
    float best_dist = INFINITY;
    const QVector2D minArgDist = {(UMax - UMin) / 20, (VMax - VMin) / 20};

    std::default_random_engine generator;
    std::uniform_real_distribution<float> Udist(UMin, UMax);
    std::uniform_real_distribution<float> Vdist(VMin, VMax);

    for (int i = 0; i < samples; ++i)
    {
        float u = Udist(generator);
        float v = Vdist(generator);

        QVector2D curArgs = {u, v};
        QVector3D p = SceneFunction(curArgs);

        float dist = pos.distanceToPoint(p);
        if (dist < best_dist && abs(curArgs.x() - args.x()) > minArgDist.x() &&
            abs(curArgs.y() - args.y()) > minArgDist.y())
        {
            best_dist = dist;
            best_params = {u, v};
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


