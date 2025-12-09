
#ifndef _GAMEOBJECT_MODEL_H
#define _GAMEOBJECT_MODEL_H

#include <G3D/Matrix3.h>
#include <G3D/Vector3.h>
#include <G3D/AABox.h>
#include <G3D/Ray.h>

#include "Define.h"

namespace VMAP
{
    class WorldModel;
    struct AreaInfo;
}

class GameObjectModelOwnerBase
{
public:
    virtual ~GameObjectModelOwnerBase() = default;

    virtual bool IsSpawned() const { return false; }
    virtual uint32 GetDisplayId() const { return 0; }
    virtual uint32 GetPhaseMask() const { return 0; }
    virtual G3D::Vector3 GetPosition() const { return G3D::Vector3::zero(); }
    virtual float GetOrientation() const { return 0.0f; }
    virtual float GetScale() const { return 1.0f; }
    virtual void DebugVisualizeCorner(G3D::Vector3 const& /*corner*/) const { }
};

class GameObjectModel /*, public Intersectable*/
{
    uint32 phasemask;
    G3D::AABox iBound;
    G3D::Matrix3 iInvRot;
    G3D::Vector3 iPos;
    //G3D::Vector3 iRot;
    float iInvScale;
    float iScale;
    VMAP::WorldModel* iModel;
    std::unique_ptr<GameObjectModelOwnerBase> owner;

    bool initialize(std::unique_ptr<GameObjectModelOwnerBase> go, std::string const& datapath);

public:
    std::string name;

    const G3D::AABox& getBounds() const { return iBound; }

    GameObjectModel()
        : phasemask(0)
        , iInvScale(0)
        , iScale(0)
        , iModel(nullptr)
        , owner(nullptr)
    {
    }
    ~GameObjectModel();

    const G3D::Vector3& getPosition() const { return iPos;}

    /**    Enables\disables collision. */
    void disable() { phasemask = 0;}
    void enable(uint32 ph_mask) { phasemask = ph_mask;}

    bool isEnabled() const {return phasemask != 0;}

    bool intersectRay(const G3D::Ray& Ray, float& MaxDist, bool StopAtFirstHit, uint32 ph_mask) const;
    void intersectPoint(G3D::Vector3 const& point, VMAP::AreaInfo& info, uint32 phaseMask) const;

    static std::unique_ptr<GameObjectModel> Create(std::unique_ptr<GameObjectModelOwnerBase> go, std::string const& dataPath);

    bool UpdatePosition();
};

#endif // _GAMEOBJECT_MODEL_H
