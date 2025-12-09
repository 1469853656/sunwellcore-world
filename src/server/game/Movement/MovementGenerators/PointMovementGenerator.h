

#ifndef TRINITY_POINTMOVEMENTGENERATOR_H
#define TRINITY_POINTMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "FollowerReference.h"

template<class T>
class PointMovementGenerator : public MovementGeneratorMedium<T, PointMovementGenerator<T> >
{
public:
    PointMovementGenerator(uint32 _id, float _x, float _y, float _z, float _speed = 0.0f, const Movement::PointsArray* _path = nullptr, bool generatePath = false, bool forceDestination = false)
        : id(_id)
        , i_x(_x)
        , i_y(_y)
        , i_z(_z)
        , speed(_speed)
        , _generatePath(generatePath)
        , _forceDestination(forceDestination)
        , _isCharging( false )
    {
        if (_path)
            m_precomputedPath = *_path;
    }

    void DoInitialize(T*);
    void DoFinalize(T*);
    void DoReset(T*);
    bool DoUpdate(T*, uint32);

    void MovementInform(T*);

    void unitSpeedChanged() override { i_recalculateSpeed = true; }

    MovementGeneratorType GetMovementGeneratorType() override { return POINT_MOTION_TYPE; }

    bool GetDestination(float& x, float& y, float& z) const
    {
        x = i_x;
        y = i_y;
        z = i_z;
        return true;
    }

    bool IsCharging() const override { return _isCharging; }

private:
    uint32 id;
    float i_x, i_y, i_z;
    float speed;
    bool i_recalculateSpeed;
    Movement::PointsArray m_precomputedPath;
    bool _generatePath;
    bool _forceDestination;
    bool _isCharging;
};

class AssistanceMovementGenerator : public PointMovementGenerator<Creature>
{
public:
    AssistanceMovementGenerator(float _x, float _y, float _z)
        : PointMovementGenerator<Creature>(0, _x, _y, _z, 0.0f, nullptr, true, true)
    {
    }

    MovementGeneratorType GetMovementGeneratorType() override { return ASSISTANCE_MOTION_TYPE; }
    void Finalize(Unit*) override;
};

// Does almost nothing - just doesn't allows previous movegen interrupt current effect.
class EffectMovementGenerator : public MovementGenerator
{
public:
    explicit EffectMovementGenerator(uint32 Id)
        : m_Id(Id)
    {
    }
    void Initialize(Unit*) override { }
    void Finalize(Unit*) override;
    void Reset(Unit*) override  { }
    bool Update(Unit*, uint32) override;
    MovementGeneratorType GetMovementGeneratorType() override { return EFFECT_MOTION_TYPE; }

private:
    uint32 m_Id;
};

#endif
