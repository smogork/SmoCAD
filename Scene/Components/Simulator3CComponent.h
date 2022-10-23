//
// Created by ksm on 10/23/22.
//

#ifndef SMOCAD_SIMULATOR3CCOMPONENT_H
#define SMOCAD_SIMULATOR3CCOMPONENT_H

#include "IComponent.h"
#include "Scene/Entities/Simulator/CutterObject.h"
#include "Scene/Entities/Simulator/CutterPath.h"
#include "Scene/Entities/Simulator/BlockUpperWall.h"
#include "Scene/Entities/Simulator/BlockLowerWall.h"
#include "Scene/Entities/Simulator/BlockSideWall.h"
#include "Scene/Entities/Simulator/BlockParameters.h"
#include "Scene/Entities/Simulator/CutterPathPolyline.h"

class Simulator3CComponent : public IComponent
{
private:
    std::unique_ptr<CutterObject> m_cutter;
    std::unique_ptr<CutterPath> m_cutterPath;
    std::unique_ptr<CutterPathPolyline> m_pathPolyline;
    std::unique_ptr<BlockUpperWall> m_blockUpper;
    std::unique_ptr<BlockLowerWall> m_blockLower;
    std::unique_ptr<BlockSideWall> m_blockSide;
    BlockParameters m_blockParams;
    
    QImage m_heightMap;
    std::shared_ptr<QOpenGLTexture> m_heightTexture;
    
    void InitializeHeightMap();
    
public:
    static std::shared_ptr<Simulator3CComponent> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans);
    void UnregisterComponent();
    
    std::shared_ptr<Transform> p_Transform;
    
    Simulator3CComponent(unsigned int oid, std::shared_ptr<Transform> simulatorTransform);
    explicit Simulator3CComponent(unsigned int oid);
    ~Simulator3CComponent() override;
};

#endif //SMOCAD_SIMULATOR3CCOMPONENT_H
