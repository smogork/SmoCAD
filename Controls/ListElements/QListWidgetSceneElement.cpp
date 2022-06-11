//
// Created by ksm on 4/17/22.
//

#include "QListWidgetSceneElement.h"
#include "Scene/SceneECS.h"

QListWidgetSceneElement::QListWidgetSceneElement(QListWidget *parent,
                                                 std::shared_ptr<SceneElement> element)
        : QListWidgetItem(element->Name.value(), parent), element(element)
{
    nameNotifier = element->Name.addNotifier(
            [this]()
            {
                if (auto elem = this->element.lock())
                    this->setText(elem->Name);
            });
    
    selectedNotifier = element->p_Selected->Selected.addNotifier(
            [this]()
            {
                if (auto elem = this->element.lock())
                    this->setSelected(elem->p_Selected->Selected);
            });
    
    if (element->p_CompositeAware)
        insideCompositeNotifier = element->p_CompositeAware->InsideComposite.addNotifier(
                [this]()
                {
                    if (auto elem = this->element.lock())
                        this->setSelected(elem->p_CompositeAware->InsideComposite);
                });
}

void QListWidgetSceneElement::SelectItem()
{
    if (auto elem = element.lock())
    {
        elem->p_Selected->Selected = true;
    }
}

unsigned int QListWidgetSceneElement::GetAttachedObjectID()
{
    if (auto elem = element.lock())
        return elem->GetAttachedObjectID();
    return SceneECS::NON_OBJECT_ID;
}

void QListWidgetSceneElement::Rename(const QString &name)
{
    if (auto elem = element.lock())
        elem->Name = name;
}

const QString QListWidgetSceneElement::GetName()
{
    if (auto elem = element.lock())
        return elem->Name;
    return "";
}
