//
// Created by ksm on 6/19/22.
//

#include "IntersectionCurve.h"
#include "Scene/Utilities/Utilites.h"

IntersectionCurve::IntersectionCurve(const QString &name, const std::vector<QVector4D> &intersectionPoints,
                                     std::function<QVector3D(QVector2D args)> sceneFunction1,
                                     std::function<QVector3D(QVector2D args)> sceneFunction2, bool isCycle)
        : IEntity(INTERSECTION_CURVE_CLASS)
{
    AddComponent(p_Drawing = DynamicDrawing::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));

    m_paramPoints = intersectionPoints;
    m_sceneFunction1 = sceneFunction1;
    m_sceneFunction2 = sceneFunction2;
    m_cycle = isCycle;

    if (m_cycle)
        m_paramPoints.push_back(m_paramPoints.front());

    InitializeDrawing();

    selectedNotifier = p_Selected->Selected.addNotifier(
            [this]()
            {
                if (p_Selected->Selected)
                    DrawingColor = Selectable::SelectedColor;
                else
                    DrawingColor = DefaultColor;
            });

}

void IntersectionCurve::InitializeDrawing()
{
    p_Drawing->SetVertexData(GenerateGeometryVertices());
    p_Drawing->p_bufferLayout.Push<float>(3);//position
    if (auto sh = Renderer::GetShader(DEFAULT_SHADER).lock())
        p_Drawing->AttachShader(sh);

    p_Drawing->p_renderingFunction = ASSIGN_DRAWING_FUNCTION(&IntersectionCurve::DrawingFunction);
    p_Drawing->p_uniformFunction = ASSIGN_UNIFORM_FUNCTION(&IntersectionCurve::UniformFunction);
}

void IntersectionCurve::DrawingFunction(QOpenGLContext *context)
{
    Renderer::DrawLineStrip(context->functions(), GetIndexCount());
}

void IntersectionCurve::UniformFunction(std::shared_ptr<ShaderWrapper> shader)
{
    shader->SetUniform("u_ObjectColor", ColorToVector4D(DrawingColor));
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
}

std::vector<float> IntersectionCurve::GenerateGeometryVertices()
{
    std::vector<float> res;
    res.reserve(m_paramPoints.size() * 3);

    for (const QVector4D &arg: m_paramPoints)
    {
        auto p = m_sceneFunction1({arg.x(),arg.y()});
        res.push_back(p.x());
        res.push_back(p.y());
        res.push_back(p.z());
    }

    return res;
}

int IntersectionCurve::GetIndexCount()
{
    return m_paramPoints.size();
}

QImage IntersectionCurve::GetTrimmingTextureOne(std::shared_ptr<IntersectionAware> plane)
{
    std::vector<QVector2D> args(m_paramPoints.size());

    for (int i = 0; i < m_paramPoints.size(); ++i)
        args[i] = m_paramPoints[i].toVector2D();

    return GetTrimmingTexture(args, plane);
}

QImage IntersectionCurve::GetTrimmingTextureTwo(std::shared_ptr<IntersectionAware> plane)
{
    std::vector<QVector2D> args(m_paramPoints.size());

    for (int i = 0; i < m_paramPoints.size(); ++i)
        args[i] = {m_paramPoints[i].z(), m_paramPoints[i].w()};

    return GetTrimmingTexture(args, plane);
}

QImage IntersectionCurve::GetTrimmingTexture(const std::vector<QVector2D> &points, std::shared_ptr<IntersectionAware> plane)
{
    const int size = 512;
    QImage res({size, size}, QImage::Format_Mono);
    res.fill(Qt::color1);

    //zapelnij pixele z krzywej przeciecia
    QPainter painter(&res);
    QBrush brush(Qt::color0, Qt::SolidPattern);
    painter.setBrush(brush);
    for (int i = 1; i < points.size(); ++i)
    {
        bool wrapX = 0, wrapY = 0;
        int x1 = (points[i - 1].x()) * size / plane->UMax;
        int y1 = (points[i - 1].y()) * size / plane->VMax;
        int x2 = (points[i].x()) * size / plane->UMax;
        int y2 = (points[i].y()) * size / plane->VMax;


        if (abs(x2 - x1) > size / 2)
            wrapX = true;
        if (abs(y2 - y1) > size / 2)
            wrapY = true;

        //Jezeli sie zapetlamy, to narysuj dwukrotnie
        if (wrapX)
        {
            if (x1 > size / 2)
            {
                int x2p = std::clamp(x2 + 512, 0, 512);
                int x1p = std::clamp(x1 - 512, 0, 512);

                painter.drawLine(x1, y1, x2p, y2);
                painter.drawLine(x1p, y1, x2, y2);
            }
            else
            {
                int x2p = std::clamp(x2 - 512, 0, 512);
                int x1p = std::clamp(x1 + 512, 0, 512);

                painter.drawLine(x1, y1, x2p, y2);
                painter.drawLine(x1p, y1, x2, y2);
            }
        }

        if (wrapY)
        {
            if (y1 > size / 2)
            {
                int y1p = std::clamp(y1 - 512, 0, 512);
                int y2p = std::clamp(y2 + 512, 0, 512);

                painter.drawLine(x1, y1, x2, y2p);
                painter.drawLine(x1, y1p, x2, y2);
            }
            else
            {
                int y1p = std::clamp(y1 + 512, 0, 512);
                int y2p = std::clamp(y2 - 512, 0, 512);

                painter.drawLine(x1, y1, x2, y2p);
                painter.drawLine(x1, y1p, x2, y2);
            }
        }

        if (!wrapX and !wrapY)
            painter.drawLine(x1, y1, x2, y2);
    }

    int x1 = (points[0].x()) * size / plane->UMax;
    int y1 = (points[0].y()) * size / plane->VMax;
    bool test = res.pixelIndex(x1, y1) != 0;

    //zapusc algorytm FloodFill (4spojny) dla pierwszego bialego pixela
    for (int i = 0; i < size * size; ++i)
    {
        int x = i / size;
        int y = i % size;

        if (res.pixelIndex(x, y) != 0)
        {
            FloodFill4({x, y}, 0, res);
            break;
        }
    }

    return res;
}

void IntersectionCurve::FloodFill4(QPoint start, uint color, QImage& image)
{

    std::stack<QPoint> s;
    s.push(start);

    while (!s.empty())
    {
        QPoint cur = s.top();
        s.pop();

        if (cur.x() < 0 || cur.x() >= image.width() ||
            cur.y() < 0 || cur.y() >= image.height())
            continue;

        if (image.pixelIndex(cur) == color)
            continue;

        image.setPixel(cur, color);
        s.push({cur.x() + 1, cur.y()});
        s.push({cur.x(), cur.y() + 1});
        s.push({cur.x() - 1, cur.y()});
        s.push({cur.x(), cur.y() - 1});
    }
}
