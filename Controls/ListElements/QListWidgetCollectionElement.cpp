//
// Created by ksm on 4/17/22.
//

#include "QListWidgetCollectionElement.h"
#include "Scene/SceneECS.h"

QListWidgetCollectionElement::QListWidgetCollectionElement(QListWidget *parent, std::shared_ptr<SceneElement> element)
        : QListWidgetItem(element->Name.value(), parent), element(element)
{
    nameNotifier = element->Name.addNotifier([this]()
                                             {
                                                 if (auto elem = this->element.lock())
                                                     this->setText(elem->Name);
                                             });

}

unsigned int QListWidgetCollectionElement::GetAttachedObjectID()
{
    if (auto elem = element.lock())
        return elem->GetAttachedObjectID();
    return SceneECS::NON_OBJECT_ID;
}