#pragma once

#include "ScriptedCreature.h"
#include "PassiveAI.h"

class Creature;

struct MythicAI : public ScriptedAI
{
    explicit MythicAI(Creature* creature);
    virtual ~MythicAI() { }

    void Reset() override;
    void EnterCombat(Unit* /*who*/) override;
    void UpdateAI(uint32 /*diff*/) override;
    void AttackStart(Unit* /*who*/) override;
    void JustDied(Unit* /*who*/) override;
    void MythicDoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/);
    bool CanAIAttack(Unit const* /*who*/) const override;

public:
    TaskScheduler task;
    InstanceScript* instance;

protected:
    inline bool IsMythic() const { return _mythicInstance; }
    inline uint32 GetMythicKeyLevel() const { return instance ? instance->GetMythicKeyLevel() : 0; }
private:
    uint32 _mythicKeyLevel;
    bool _mythicInstance;
};

struct MythicCompanionAI : public CritterAI
{
    MythicCompanionAI(Creature* creature) : CritterAI(creature) { }

    virtual ~MythicCompanionAI() { }

    virtual void UpdateLevel(uint32 /*newLevel*/) { }
};
