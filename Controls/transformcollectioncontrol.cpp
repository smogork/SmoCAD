#include "transformcollectioncontrol.h"
#include "ui_transformcollectioncontrol.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"

TransformCollectionControl::TransformCollectionControl(std::weak_ptr<TransformCollection> collection, QWidget *parent) :
    ComponentControl(parent),
    ui(new Ui::TransformCollectionControl)
{
    ui->setupUi(this);

    m_collection = collection;
    UpdateElementsList();
    if (auto c = m_collection.lock())
    {
        connect(c.get(), &TransformCollection::PointInCollectionModified,
                this, &TransformCollectionControl::UpdateElementsList);
    }
}

TransformCollectionControl::~TransformCollectionControl()
{
    delete ui;
}

void TransformCollectionControl::UpdateElementsList()
{
    m_elements.clear();
    if (auto c = m_collection.lock())
    {
        if (auto scene = SceneECS::Instance().lock())
        {
            if (auto elements = scene->GetSystem<SceneElementSystem>().lock())
            {
                for (const std::weak_ptr<Transform> &wp: c->GetPoints())
                {
                    auto sceneElement = elements->GetComponent(wp.lock()->GetAttachedObjectID()).lock();

                    if (sceneElement)
                        m_elements.push_back(std::make_unique<QListWidgetCollectionElement>(ui->listWidgetPoints, sceneElement));
                }
            }
        }
    }
}


