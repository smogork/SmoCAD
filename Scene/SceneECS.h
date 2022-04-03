//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_SCENEECS_H
#define SMOCAD_SCENEECS_H


#include <QObject>

class SceneECS: QObject
{
    Q_OBJECT
public:
    static std::unique_ptr<SceneECS> Instance;

    ~SceneECS();



private:
    SceneECS();
};


#endif //SMOCAD_SCENEECS_H
