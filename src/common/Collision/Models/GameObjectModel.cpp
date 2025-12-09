
#include "VMapFactory.h"
#include "VMapManager2.h"
#include "VMapDefinitions.h"
#include "WorldModel.h"
#include "MapTree.h"

#include "GameObjectModel.h"
#include "Log.h"
#include "Timer.h"

using G3D::Vector3;
using G3D::Ray;
using G3D::AABox;

struct GameobjectModelData
{
    GameobjectModelData(const std::string& name_, const AABox& box) :
        bound(box), name(name_) {}

    AABox bound;
    std::string name;
};

typedef std::unordered_map<uint32, GameobjectModelData> ModelList;
ModelList model_list;

void LoadGameObjectModelList(std::string const& dataPath)
{
//#ifndef NO_CORE_FUNCS
    uint32 oldMSTime = getMSTime();
//#endif

    FILE* model_list_file = fopen((dataPath + "vmaps/" + VMAP::GAMEOBJECT_MODELS).c_str(), "rb");
    if (!model_list_file)
    {
        sLog->outError("Unable to open '%s' file.", VMAP::GAMEOBJECT_MODELS);
        return;
    }

    uint32 name_length, displayId;
    char buff[500];
    while (true)
    {
        Vector3 v1, v2;
        if (fread(&displayId, sizeof(uint32), 1, model_list_file) != 1)
            if (feof(model_list_file))  // EOF flag is only set after failed reading attempt
                break;

        if (fread(&name_length, sizeof(uint32), 1, model_list_file) != 1
            || name_length >= sizeof(buff)
            || fread(&buff, sizeof(char), name_length, model_list_file) != name_length
            || fread(&v1, sizeof(Vector3), 1, model_list_file) != 1
            || fread(&v2, sizeof(Vector3), 1, model_list_file) != 1)
        {
            sLog->outError("File '%s' seems to be corrupted!", VMAP::GAMEOBJECT_MODELS);
            break;
        }

        model_list.insert
        (
            ModelList::value_type( displayId, GameobjectModelData(std::string(buff, name_length), AABox(v1, v2)) )
        );
    }

    fclose(model_list_file);
    sLog->outString(">> Loaded %u GameObject models in %u ms", uint32(model_list.size()), GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

GameObjectModel::~GameObjectModel()
{
    if (iModel)
        ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->releaseModelInstance(name);
}

bool GameObjectModel::initialize(std::unique_ptr<GameObjectModelOwnerBase> go, std::string const& dataPath)
{
    ModelList::const_iterator it = model_list.find(go->GetDisplayId());
    if (it == model_list.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        sLog->outError("GameObject model %s has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    iModel = ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->acquireModelInstance(dataPath + "vmaps/", it->second.name);

    if (!iModel)
        return false;

    name = it->second.name;
    //flags = VMAP::MOD_M2;
    //adtId = 0;
    //ID = 0;
    iPos = go->GetPosition();

	phasemask = go->GetPhaseMask();

    iScale = go->GetScale();
    iInvScale = 1.f / iScale;

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(go->GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    iBound = rotated_bounds + iPos;
#ifdef SPAWN_CORNERS
    // test:
    for (int i = 0; i < 8; ++i)
    {
        Vector3 pos(iBound.corner(i));
        go->DebugVisualiseCorners(pos);
    }
#endif

    owner = std::move(go);
    return true;
}

std::unique_ptr<GameObjectModel> GameObjectModel::Create(std::unique_ptr<GameObjectModelOwnerBase> go, std::string const& dataPath)
{
    std::unique_ptr<GameObjectModel> mdl = std::make_unique< GameObjectModel >();
    if (!mdl->initialize(std::move(go), dataPath))
        return {};

    return std::move( mdl );
}

bool GameObjectModel::intersectRay(const G3D::Ray& ray, float& MaxDist, bool StopAtFirstHit, uint32 ph_mask) const
{
    if (!(phasemask & ph_mask) || !owner->IsSpawned())
        return false;

    float time = ray.intersectionTime(iBound);
    if (time == G3D::inf())
        return false;

    // child bounds are defined in object space:
    Vector3 p = iInvRot * (ray.origin() - iPos) * iInvScale;
    Ray modRay(p, iInvRot * ray.direction());
    float distance = MaxDist * iInvScale;
    bool hit = iModel->IntersectRay(modRay, distance, StopAtFirstHit);
    if (hit)
    {
        distance *= iScale;
        MaxDist = distance;
    }
    return hit;
}

bool GameObjectModel::UpdatePosition()
{
    if (!iModel)
        return false;

    ModelList::const_iterator it = model_list.find(owner->GetDisplayId());
    if (it == model_list.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        //VMAP_ERROR_LOG("misc", "GameObject model %s has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    iPos = owner->GetPosition();
    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(owner->GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    iBound = rotated_bounds + iPos;
#ifdef SPAWN_CORNERS
    // test:
    for (int i = 0; i < 8; ++i)
    {
        Vector3 pos(iBound.corner(i));
        go->DebugVisualiseCorners(pos);
    }
#endif

    return true;
}

void GameObjectModel::intersectPoint(G3D::Vector3 const& point, VMAP::AreaInfo& info, uint32 phase_mask) const
{
    if (!isEnabled() || !owner->IsSpawned())
        return;

    if (!(phasemask & phase_mask))
        return;

    if (!iBound.contains(point))
        return;

    // child bounds are defined in object space:
    Vector3 pModel = iInvRot * (point - iPos) * iInvScale;
    Vector3 zDirModel = iInvRot * Vector3(0.f, 0.f, -1.f);
    float zDist;
    if (iModel->IntersectPoint(pModel, zDirModel, zDist, info))
    {
        Vector3 modelGround = pModel + zDist * zDirModel;
        float world_Z = ((modelGround * iInvRot) * iScale + iPos).z;
        if (info.ground_Z < world_Z)
            info.ground_Z = world_Z;
    }
}
