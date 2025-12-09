#include "SoloQueue.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "Containers.h"
#include "WorldSession.h"
#include "Player.h"
#include "Chat.h"

void SoloQueue::UpdateMMRTolerance(uint32 diff)
{
    diff += 10 * IN_MILLISECONDS;

    if (mmrChangeTimer <= diff)
    {
        mmrChangeTimer = sWorld->getIntConfig(CONFIG_SOLOQUEUE_MMR_TOLERANCE_INTERVAL_SECS) * IN_MILLISECONDS;
        mmrTolerance += sWorld->getIntConfig(CONFIG_SOLOQUEUE_MMR_TOLERANCE_INTERVAL_VALUE);
    }
    else
        mmrChangeTimer -= diff;
}

void SoloQueue::SetLastDPSList(std::set<uint64> const& list)
{
    if (!list.empty())
    {
        lastDPSList.clear();
        lastDPSList.insert(list.begin(), list.end());
    }
}

std::set<uint64> const& SoloQueue::GetLastDPSList() const
{
    return lastDPSList;
}

void OptimizedSoloQueue::SetLastDPSList(std::set<uint64> const& list)
{
    if (!list.empty())
    {
        lastDPSList.clear();
        lastDPSList.insert(list.begin(), list.end());
    }
}

std::set<uint64> const& OptimizedSoloQueue::GetLastDPSList() const
{
    return lastDPSList;
}

SoloQueueMgr::SoloQueueMgr() : _updateQueuesTimer(10 * IN_MILLISECONDS)
{
}

bool SoloQueueMgr::AddToPreparationQueue(Player* player, Battleground* bg, BattlegroundQueue* bgQueue, PvPDifficultyEntry const* bracketEntry, uint32 joinTime /*= 0*/)
{
    if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE))
    {
        ChatHandler(player->GetSession()).SendSysMessage("SoloQueue is disabled!");
        return false;
    }

    uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
    if (!arenaTeamId)
        return false;

    ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(arenaTeamId);
    if (!team)
        return false;

    if (uint32(player->GetAverageItemLevel()) < sWorld->getIntConfig(CONFIG_SOLOQUEUE_REQUIRED_ITEM_LEVEL))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You must have an item level of at least %d to queue up for Solo Queue.", sWorld->getIntConfig(CONFIG_SOLOQUEUE_REQUIRED_ITEM_LEVEL));
        return false;
    }

    uint32 specialization = GetTalentsSpecialization(player);
    if (specialization == SPEC_TANK)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You cannot join the Solo Queue as a tank.");
        return false;
    }

    // 15007 - Resurrection Sickness
    if (player->HasAura(15007))
    {
        ChatHandler(player->GetSession()).SendSysMessage("You cannot enter the arena under the effect Resurrection Sickness.");
        return false;
    }

    if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_GEMS_SOCKET_REQ))
    {
        bool allGemSocketsFilled = true;
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
            if (ItemRef item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; ++enchant_slot)
                {
                    uint8 socketColor = item->GetTemplate()->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color;
                    if (!socketColor)
                        continue;

                    if (!item->GetEnchantmentId(EnchantmentSlot(enchant_slot)))
                    {
                        allGemSocketsFilled = false;
                        break;
                    }
                }

                if (!allGemSocketsFilled)
                    break;
            }

        if (!allGemSocketsFilled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot join SoloQ without having filled all sockets with gems.");
            return false;
        }
    }

    uint32 requiredResilience = sWorld->getIntConfig(CONFIG_SOLOQUEUE_RESILIENCE_REQ);
    if (requiredResilience && requiredResilience > (player->GetUInt32Value(static_cast<uint16>(PLAYER_FIELD_COMBAT_RATING_1) + static_cast<uint16>(CR_CRIT_TAKEN_SPELL))))
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot join the solo queue without having at least %d resilience", requiredResilience);
        return false;
    }

    bool rejoined = false;
    uint32 currentTime = World::GetGameTimeMS();
    if (!joinTime)
        joinTime = currentTime;
    else
        rejoined = true;

    SoloQueue soloQueue;
    soloQueue.arenaTeamId = arenaTeamId;
    soloQueue.ownerGUID = player->GetGUID();
    soloQueue.talentsSpecialization = specialization;
    soloQueue.playerClass = player->getClass();
    soloQueue.mmrChangeTimer = sWorld->getIntConfig(CONFIG_SOLOQUEUE_MMR_TOLERANCE_INTERVAL_SECS) * IN_MILLISECONDS;
    soloQueue.mmrTolerance = sWorld->getIntConfig(CONFIG_SOLOQUEUE_MMR_TOLERANCE_INITIAL);
    soloQueue.joinTime = joinTime;
    soloQueue.SetLastDPSList(team->GetLastDPSList());

    _preparationQueue[arenaTeamId] = std::move(soloQueue);

    uint32 avgTime = bgQueue->GetAverageQueueWaitTime(nullptr, bracketEntry->GetBracketId());
    uint32 queueSlot = player->AddBattlegroundQueueId(BATTLEGROUND_QUEUE_SOLO_QUEUE);

    WorldPacket data;
    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, getMSTimeDiff(soloQueue.joinTime, currentTime), ARENA_TYPE_SOLO_QUEUE, TEAM_NEUTRAL, true);
    player->GetSession()->SendPacket(&data);

    if (!rejoined)
        ChatHandler(player->GetSession()).SendSysMessage("You have been added to the queue. Please wait while we find other players for your team!");

    return true;
}

void SoloQueueMgr::RemoveFromPreparationQueue(Player* player, bool castDeserter /*= false*/)
{
    uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
    if (!arenaTeamId)
        return;

    auto itr = _preparationQueue.find(arenaTeamId);
    if (itr != _preparationQueue.end())
    {
        _preparationQueue.erase(itr);
        player->RemoveBattlegroundQueueId(BATTLEGROUND_QUEUE_SOLO_QUEUE);
    }
    else if (castDeserter && !player->HasAura(26013))
    {
        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_AURA_DURATION, 5 * MINUTE * IN_MILLISECONDS);
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, 1);
        player->CastCustomSpell(26013, values, player, TRIGGERED_FULL_MASK);

        ChatHandler(player->GetSession()).SendSysMessage("You have been removed from the Solo Queue and given a Deserter penalty.");
    }
}

bool SoloQueueMgr::IsInPreparationQueue(Player* player)
{
    uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
    if (!arenaTeamId)
        return false;

    return _preparationQueue.find(arenaTeamId) != _preparationQueue.end();
}

SoloQueue const* SoloQueueMgr::GetSoloQueue(Player* player) const
{
    uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
    if (!arenaTeamId)
        return nullptr;

    auto itr = _preparationQueue.find(arenaTeamId);
    if (itr != _preparationQueue.end())
        return &itr->second;

    return nullptr;
}

void SoloQueueMgr::Update(uint32 diff)
{
    if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE))
        return;

    // Update every 10 seconds
    if (_updateQueuesTimer <= diff)
    {
        _updateQueuesTimer = 10 * IN_MILLISECONDS;

        if (_preparationQueue.empty())
            return;

        std::unordered_set<uint32> removeList;
        std::vector<std::pair<std::set<uint64>, std::vector<SoloQueue>>> readySoloQueues;
        for (SoloQueuePreparationQueue::iterator itr = _preparationQueue.begin(); itr != _preparationQueue.end(); ++itr)
        {
            uint32 arenaTeamId = itr->first;
            if (removeList.find(arenaTeamId) != removeList.end())
                continue;

            ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(arenaTeamId);
            if (!team)
            {
                removeList.insert(arenaTeamId);
                continue;
            }

            SoloQueue& soloQueue = itr->second;

            Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(soloQueue.ownerGUID);
            if (!player)
            {
                removeList.insert(arenaTeamId);
                continue;
            }

            // Player was somehow removed from the battleground queue, remove him from the preparation queue to prevent crash
            if (player->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_SOLO_QUEUE) == PLAYER_MAX_BATTLEGROUND_QUEUES)
            {
                removeList.insert(arenaTeamId);
                continue;
            }

            soloQueue.UpdateMMRTolerance(diff);

            std::vector<std::pair<uint32, SoloQueue>> acceptedList;

            // now check compatibility with other players
            for (SoloQueuePreparationQueue::iterator itr2 = _preparationQueue.begin(); itr2 != _preparationQueue.end(); ++itr2)
            {
                uint32 otherArenaTeamId = itr2->first;
                if (removeList.find(otherArenaTeamId) != removeList.end())
                    continue;

                // don't check for me
                if (arenaTeamId == otherArenaTeamId)
                    continue;

                ArenaTeam* otherTeam = sArenaTeamMgr->GetArenaTeamById(otherArenaTeamId);
                if (!otherTeam)
                {
                    removeList.insert(otherArenaTeamId);
                    continue;
                }

                SoloQueue& otherSoloQueue = itr2->second;
                Player* otherPlayer = ObjectAccessor::FindPlayerInOrOutOfWorld(otherSoloQueue.ownerGUID);
                if (!otherPlayer)
                {
                    removeList.insert(otherArenaTeamId);
                    continue;
                }

                // Player was somehow removed from the battleground queue, remove him from the preparation queue to prevent crash
                if (otherPlayer->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_SOLO_QUEUE) == PLAYER_MAX_BATTLEGROUND_QUEUES)
                {
                    removeList.insert(otherArenaTeamId);
                    continue;
                }

                if (player->getLevel() != otherPlayer->getLevel())
                    continue;

                uint32 mmrDiff = 0;
                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_MMR_MATCHING))
                {
                    mmrDiff = std::abs(team->GetStats().MatchMakerRating - otherTeam->GetStats().MatchMakerRating);
                    if (mmrDiff > soloQueue.mmrTolerance)
                        continue;
                }

                acceptedList.emplace_back(mmrDiff, otherSoloQueue);
            }

            if (acceptedList.size() < 2)
                continue;

            std::set<uint64> currentDPSList;
            std::vector<SoloQueue> newList;
            newList.reserve(3);
            if (!SelectRandomSoloQueues(soloQueue, acceptedList, newList, currentDPSList))
                continue;

            readySoloQueues.emplace_back(currentDPSList, newList);
        }

        Trinity::Containers::RandomShuffle(readySoloQueues);

        PvPDifficultyEntry const* bracketEntry = nullptr;
        std::vector<std::pair<OptimizedSoloQueue, OptimizedSoloQueue>> optimizedSoloQueues;
        for (auto const& itr : readySoloQueues)
        {
            std::vector<SoloQueue> acceptedList = itr.second;

            bool cancelList = false;
            for (auto const& iter : acceptedList)
            {
                if (removeList.find(iter.arenaTeamId) != removeList.end())
                {
                    cancelList = true;
                    break;
                }

                if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                {
                    if (std::any_of(acceptedList.begin(), acceptedList.end(), [iter](SoloQueue const& soloQueue) { return iter.arenaTeamId != soloQueue.arenaTeamId && iter.playerClass == soloQueue.playerClass; }))
                    {
                        cancelList = true;
                        break;
                    }
                }
            }

            if (cancelList)
                continue;

            SoloQueue const& leadingSoloQueue = *acceptedList.begin();
            Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(leadingSoloQueue.ownerGUID);
            if (!player)
                continue;

            uint32 averageMMRRating = CalculateAverageMMR(acceptedList);
            uint32 averageRating = CalculateAverageRating(acceptedList);
            uint32 maxRatingDifferenceA = sBattlegroundMgr->GetMaxRatingDifference(averageMMRRating);

            Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
            if (!bg)
                continue;

            BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_SOLO_QUEUE);
            bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), player->getLevel());
            if (!bracketEntry)
                continue;

            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_OPTIMIZED_QUEUES))
            {
                std::unordered_set<uint32> mySoloQueues;
                for (auto const& iter : acceptedList)
                    mySoloQueues.insert(iter.arenaTeamId);

                uint32 discardRatingTime = sBattlegroundMgr->GetRatingDiscardTimer(BATTLEGROUND_QUEUE_SOLO_QUEUE);
                uint32 discardTime = (World::GetGameTimeMS() <= discardRatingTime) ? 0 : World::GetGameTimeMS() - discardRatingTime;

                uint32 arenaMinMMRRatingTeamA = (averageMMRRating <= maxRatingDifferenceA) ? 0 : averageMMRRating - maxRatingDifferenceA;
                uint32 arenaMaxMMRRatingTeamA = averageMMRRating + maxRatingDifferenceA;

                uint32 averageJoinTime = CalculateAverageJoinTime(acceptedList);
                if (averageJoinTime < discardTime)
                {
                    uint32 timeOverDiscardTime = discardTime - averageJoinTime;
                    uint32 timeInMinutes = 1 + (timeOverDiscardTime / 60000);
                    if (timeInMinutes <= 5)
                    {
                        arenaMinMMRRatingTeamA = (averageMMRRating <= (maxRatingDifferenceA * timeInMinutes)) ? 0 : averageMMRRating - (maxRatingDifferenceA * timeInMinutes);
                        arenaMaxMMRRatingTeamA = averageMMRRating + (maxRatingDifferenceA * timeInMinutes);
                    }
                }

                std::array<std::vector<std::pair<uint32, OptimizedSoloQueue>>, 2> groupsByDiffMMR;
                for (auto const& itr2 : readySoloQueues)
                {
                    std::vector<SoloQueue> acceptedList2 = itr2.second;

                    bool cancelList2 = false;
                    for (auto const& iter2 : acceptedList2)
                    {
                        if ((removeList.find(iter2.arenaTeamId) != removeList.end()) || (mySoloQueues.find(iter2.arenaTeamId) != mySoloQueues.end()))
                        {
                            cancelList2 = true;
                            break;
                        }

                        if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                        {
                            if (std::any_of(acceptedList2.begin(), acceptedList2.end(), [iter2](SoloQueue const& soloQueue) { return iter2.arenaTeamId != soloQueue.arenaTeamId && iter2.playerClass == soloQueue.playerClass; }))
                            {
                                cancelList = true;
                                break;
                            }
                        }
                    }

                    if (cancelList2)
                        continue;

                    SoloQueue const& leadingSoloQueue2 = *acceptedList2.begin();
                    Player* player2 = ObjectAccessor::FindPlayerInOrOutOfWorld(leadingSoloQueue2.ownerGUID);
                    if (!player2)
                        continue;

                    uint32 averageMMRRating2 = CalculateAverageMMR(acceptedList2);
                    uint32 averageRating2 = CalculateAverageRating(acceptedList2);
                    uint32 maxRatingDifferenceB = sBattlegroundMgr->GetMaxRatingDifference(averageMMRRating2);

                    uint32 arenaMinMMRRatingTeamB = (averageMMRRating2 <= maxRatingDifferenceB) ? 0 : averageMMRRating2 - maxRatingDifferenceB;
                    uint32 arenaMaxMMRRatingTeamB = averageMMRRating2 + maxRatingDifferenceB;

                    uint32 averageJoinTime2 = CalculateAverageJoinTime(acceptedList2);
                    if (averageJoinTime2 < discardTime)
                    {
                        uint32 timeOverDiscardTime2 = discardTime - averageJoinTime2;
                        uint32 timeInMinutes2 = 1 + (timeOverDiscardTime2 / 60000);
                        if (timeInMinutes2 <= 5)
                        {
                            arenaMinMMRRatingTeamB = (averageMMRRating2 <= (maxRatingDifferenceB * timeInMinutes2)) ? 0 : averageMMRRating2 - (maxRatingDifferenceB * timeInMinutes2);
                            arenaMaxMMRRatingTeamB = averageMMRRating2 + (maxRatingDifferenceB * timeInMinutes2);
                        }
                    }

                    if (((arenaMinMMRRatingTeamB >= arenaMinMMRRatingTeamA && arenaMinMMRRatingTeamB <= arenaMaxMMRRatingTeamA)
                            || (arenaMaxMMRRatingTeamB >= arenaMinMMRRatingTeamA && arenaMaxMMRRatingTeamB <= arenaMaxMMRRatingTeamA)
                            || (arenaMinMMRRatingTeamA >= arenaMinMMRRatingTeamB && arenaMinMMRRatingTeamA <= arenaMaxMMRRatingTeamB)
                            || (arenaMaxMMRRatingTeamA >= arenaMinMMRRatingTeamB && arenaMaxMMRRatingTeamA <= arenaMaxMMRRatingTeamB)))
                    {
                        OptimizedSoloQueue secondOptimizedSoloQueue;
                        secondOptimizedSoloQueue.acceptedList = acceptedList2;
                        secondOptimizedSoloQueue.averageMMRRating = averageMMRRating2;
                        secondOptimizedSoloQueue.averageRating = averageRating2;
                        secondOptimizedSoloQueue.SetLastDPSList(itr2.first);
                        secondOptimizedSoloQueue.leaderArenaTeamID = leadingSoloQueue2.arenaTeamId;
                        secondOptimizedSoloQueue.playerGUID = player2->GetGUID();

                        uint32 mmrDiff = std::abs(static_cast<int32>(averageMMRRating - averageMMRRating2));
                        if (mmrDiff <= sWorld->getIntConfig(CONFIG_ARENA_MAX_MMR_DIFF))
                            groupsByDiffMMR[1].emplace_back(0, secondOptimizedSoloQueue);

                        groupsByDiffMMR[0].emplace_back(mmrDiff, secondOptimizedSoloQueue);
                    }
                }

                if (!groupsByDiffMMR[0].empty())
                {
                    OptimizedSoloQueue const* secondOptimizedSoloQueue = nullptr;
                    // 1. If we have opponents in the nearest container (Array index 1) - select random one
                    if (!groupsByDiffMMR[1].empty())
                        secondOptimizedSoloQueue = &Trinity::Containers::SelectRandomContainerElement(groupsByDiffMMR[1]).second;
                    else
                    {
                        // 2. If all opponents are too far away - sort the main container and select the one with lowest MMR diff (Array index 0)
                        std::sort(groupsByDiffMMR[0].begin(), groupsByDiffMMR[0].end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                        secondOptimizedSoloQueue = &groupsByDiffMMR[0].begin()->second;
                    }

                    if (!secondOptimizedSoloQueue)
                        continue;

                    for (auto const& iter : acceptedList)
                        removeList.insert(iter.arenaTeamId);

                    for (auto const& iter2 : secondOptimizedSoloQueue->acceptedList)
                        removeList.insert(iter2.arenaTeamId);

                    OptimizedSoloQueue firstOptimizedSoloQueue;
                    firstOptimizedSoloQueue.acceptedList = acceptedList;
                    firstOptimizedSoloQueue.averageMMRRating = averageMMRRating;
                    firstOptimizedSoloQueue.averageRating = averageRating;
                    firstOptimizedSoloQueue.SetLastDPSList(itr.first);
                    firstOptimizedSoloQueue.leaderArenaTeamID = leadingSoloQueue.arenaTeamId;
                    firstOptimizedSoloQueue.playerGUID = player->GetGUID();

                    optimizedSoloQueues.emplace_back(std::move(firstOptimizedSoloQueue), *secondOptimizedSoloQueue);
                }
            }
            else
            {
                for (auto const& iter : acceptedList)
                    removeList.insert(iter.arenaTeamId);

                bgQueue.AddSoloQueueArenaGroup(player, bracketEntry, acceptedList, ARENA_TYPE_SOLO_QUEUE, averageRating, averageMMRRating, leadingSoloQueue.arenaTeamId, itr.first);
            }
        }

        // remove now from preparation queue
        for (uint32 arenaTeamId : removeList)
            _preparationQueue.erase(arenaTeamId);

        for (auto const& itr : optimizedSoloQueues)
        {
            OptimizedSoloQueue const& firstOptimizedSoloQueue = itr.first;
            OptimizedSoloQueue const& secondOptimizedSoloQueue = itr.second;

            BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_SOLO_QUEUE);
            bgQueue.StartInstantSoloQueueArena(firstOptimizedSoloQueue, secondOptimizedSoloQueue, bracketEntry);
        }
    }
    else
        _updateQueuesTimer -= diff;
}

bool SoloQueueMgr::SelectRandomSoloQueues(SoloQueue& soloQueue, std::vector<std::pair<uint32, SoloQueue>> const& acceptedList, std::vector<SoloQueue>& newList, std::set<uint64>& currentDPSList)
{
    std::map<uint8 /*specialization*/, std::array<std::vector<std::pair<uint32, SoloQueue>>, 2>> specList;

    for (auto& itr : acceptedList)
    {
        uint8 spec = itr.second.talentsSpecialization;
        uint32 mmrDiff = itr.first;
        if (mmrDiff <= sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MAX_MMR_DIFF))
            specList[spec][1].emplace_back(0, itr.second); // Put 0 as mmr diff - does not matter in this container

        specList[spec][0].emplace_back(mmrDiff, itr.second);
    }

    newList.clear();
    newList.push_back(soloQueue);
    switch (soloQueue.talentsSpecialization)
    {
        // Find 1 dps and 1 heal
        case SPEC_DPS:
        {
            currentDPSList.insert(soloQueue.ownerGUID);

            /* Find healer */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedHealSoloQueues = specList[SPEC_HEAL][0];
            if (orderedHealSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestHealSoloQueues = specList[SPEC_HEAL][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                {
                    return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                    {
                         return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* healer = nullptr;
            if (!nearestHealSoloQueues.empty())
                healer = &Trinity::Containers::SelectRandomContainerElement(nearestHealSoloQueues).second;
            else
            {
                // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedHealSoloQueues.begin(), orderedHealSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                healer = &orderedHealSoloQueues.begin()->second;
            }

            if (!healer)
                return false;

            newList.push_back(*healer);

            /* Find DPS */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedDPSSoloQueues = specList[SPEC_DPS][0];
            if (orderedDPSSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestDPSSoloQueues = specList[SPEC_DPS][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                {
                    return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                    {
                       return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                    });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                {
                    return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* dps = nullptr;
            if (!nearestDPSSoloQueues.empty())
                dps = &Trinity::Containers::SelectRandomContainerElement(nearestDPSSoloQueues).second;
            else
            {
                // 3.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedDPSSoloQueues.begin(), orderedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                dps = &orderedDPSSoloQueues.begin()->second;
            }

            if (!dps)
                return false;

            newList.push_back(*dps);
            currentDPSList.insert(dps->ownerGUID);

            return true;
        }
        // Find 2 dpses
        case SPEC_HEAL:
        {
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ALTERNATIVE_DPS_SPECS))
            {
                /* Find Melee DPS */
                // 1. Check main container size (at least 2 elements)
                std::vector<std::pair<uint32, SoloQueue>>& orderedMeleeDPSSoloQueues = specList[SPEC_DPS_MELEE][0];
                if (orderedMeleeDPSSoloQueues.empty())
                    return false;

                std::vector<std::pair<uint32, SoloQueue>>& nearestMeleeDPSSoloQueues = specList[SPEC_DPS_MELEE][1];

                // 2. Class stacking
                if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                {
                    std::erase_if(orderedMeleeDPSSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == playerClass;
                    });
                    if (orderedMeleeDPSSoloQueues.empty())
                        return false;

                    // 2.1 Class stacking on nearest container (if not empty)
                    if (!nearestMeleeDPSSoloQueues.empty())
                        std::erase_if(nearestMeleeDPSSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueDPS)
                        {
                            return soloQueueDPS.second.playerClass == playerClass;
                        });
                }

                // 3. DPS last classes played with the player
                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
                {
                    std::erase_if(orderedMeleeDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList()](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID);
                    });
                    if (orderedMeleeDPSSoloQueues.empty())
                        return false;

                    // 3.1 DPS last classes played with the player on nearest container (if not empty)
                    if (!nearestMeleeDPSSoloQueues.empty())
                        std::erase_if(nearestMeleeDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList()](auto const& soloQueueDPS)
                        {
                            return lastDPSList.contains(soloQueueDPS.second.ownerGUID);
                        });
                }

                // 4. Select container:
                // 4.1 If nearest container is not empty - select random element from it.
                SoloQueue const* meleeDPS = nullptr;
                if (!nearestMeleeDPSSoloQueues.empty())
                    meleeDPS = &Trinity::Containers::SelectRandomContainerElement(nearestMeleeDPSSoloQueues).second;
                else
                {
                    // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                    std::sort(orderedMeleeDPSSoloQueues.begin(), orderedMeleeDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                    meleeDPS = &orderedMeleeDPSSoloQueues.begin()->second;
                }

                if (!meleeDPS)
                    return false;

                newList.push_back(*meleeDPS);
                currentDPSList.insert(meleeDPS->ownerGUID);

                /* Find Ranged DPS */
                // 5. Check main container size (at least 2 elements)
                std::vector<std::pair<uint32, SoloQueue>>& orderedRangedDPSSoloQueues = specList[SPEC_DPS_RANGED][0];
                if (orderedRangedDPSSoloQueues.empty())
                    return false;

                std::vector<std::pair<uint32, SoloQueue>>& nearestRangedDPSSoloQueues = specList[SPEC_DPS_RANGED][1];

                // 6. Class stacking
                if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                {
                    std::erase_if(orderedRangedDPSSoloQueues, [playerClass = soloQueue.playerClass, meleeDPSClass = meleeDPS->playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == meleeDPSClass;
                    });
                    if (orderedRangedDPSSoloQueues.empty())
                        return false;

                    // 6.1 Class stacking on nearest container (if not empty)
                    if (!nearestRangedDPSSoloQueues.empty())
                        std::erase_if(nearestRangedDPSSoloQueues, [playerClass = soloQueue.playerClass, meleeDPSClass = meleeDPS->playerClass](auto const& soloQueueDPS)
                        {
                            return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == meleeDPSClass;
                        });
                }

                // 7. DPS last classes played with the player
                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
                {
                    std::erase_if(orderedRangedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), meleeDPSList = meleeDPS->GetLastDPSList(), meleeOwnerGUID = meleeDPS->ownerGUID](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || meleeDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(meleeOwnerGUID);
                    });
                    if (orderedRangedDPSSoloQueues.empty())
                        return false;

                    // 7.1 DPS last classes played with the player on nearest container (if not empty)
                    if (!nearestRangedDPSSoloQueues.empty())
                        std::erase_if(nearestRangedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), meleeDPSList = meleeDPS->GetLastDPSList(), meleeOwnerGUID = meleeDPS->ownerGUID](auto const& soloQueueDPS)
                        {
                            return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || meleeDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(meleeOwnerGUID);
                        });
                }

                // 8. Select container:
                // 8.1 If nearest container is not empty - select random element from it.
                SoloQueue const* rangedDPS = nullptr;
                if (!nearestRangedDPSSoloQueues.empty())
                    rangedDPS = &Trinity::Containers::SelectRandomContainerElement(nearestRangedDPSSoloQueues).second;
                else
                {
                    // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                    std::sort(orderedRangedDPSSoloQueues.begin(), orderedRangedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                    rangedDPS = &orderedRangedDPSSoloQueues.begin()->second;
                }

                if (!rangedDPS)
                    return false;

                newList.push_back(*rangedDPS);
                currentDPSList.insert(rangedDPS->ownerGUID);
            }
            else
            {
                /* Find 2 DPS */
                // 1. Check main container size (at least 2 elements)
                std::vector<std::pair<uint32, SoloQueue>>& orderedDPSSoloQueues = specList[SPEC_DPS][0];
                if (orderedDPSSoloQueues.size() < 2)
                    return false;

                std::vector<std::pair<uint32, SoloQueue>>& nearestDPSSoloQueues = specList[SPEC_DPS][1];

                // 2. Class stacking
                if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                {
                    std::erase_if(orderedDPSSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == playerClass;
                    });
                    if (orderedDPSSoloQueues.size() < 2)
                        return false;

                    // 2.1 Class stacking on nearest container (if not empty)
                    if (!nearestDPSSoloQueues.empty())
                        std::erase_if(nearestDPSSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueDPS)
                        {
                             return soloQueueDPS.second.playerClass == playerClass;
                        });
                }

                // 3. DPS last classes played with the player
                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
                {
                    std::erase_if(orderedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList()](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID);
                    });
                    if (orderedDPSSoloQueues.size() < 2)
                        return false;

                    // 3.1 DPS last classes played with the player on nearest container (if not empty)
                    if (!nearestDPSSoloQueues.empty())
                        std::erase_if(nearestDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList()](auto const& soloQueueDPS)
                        {
                            return lastDPSList.contains(soloQueueDPS.second.ownerGUID);
                        });
                }

                // 4. Select container:
                // 4.1 If nearest container is not empty - select random element from it.
                SoloQueue const* dps1 = nullptr;
                bool alreadySorted = false;
                if (!nearestDPSSoloQueues.empty())
                    dps1 = &Trinity::Containers::SelectRandomContainerElement(nearestDPSSoloQueues).second;
                else
                {
                    // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                    alreadySorted = true;
                    std::sort(orderedDPSSoloQueues.begin(), orderedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                    dps1 = &orderedDPSSoloQueues.begin()->second;
                }

                if (!dps1)
                    return false;

                newList.push_back(*dps1);
                currentDPSList.insert(dps1->ownerGUID);

                // 5. Remove first DPS from both containers
                std::erase_if(orderedDPSSoloQueues, [dpsOwnerGUID = dps1->ownerGUID](auto const& soloQueueDPS) { return soloQueueDPS.second.ownerGUID == dpsOwnerGUID; });
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [dpsOwnerGUID = dps1->ownerGUID](auto const& soloQueueDPS) { return soloQueueDPS.second.ownerGUID == dpsOwnerGUID; });

                // 6. Class stacking (against first DPS)
                if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
                {
                    std::erase_if(orderedDPSSoloQueues, [dpsClass = dps1->playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == dpsClass;
                    });
                    if (orderedDPSSoloQueues.empty())
                        return false;

                    // 6.1 Class stacking on nearest container (if not empty)
                    if (!nearestDPSSoloQueues.empty())
                        std::erase_if(nearestDPSSoloQueues, [dpsClass = dps1->playerClass](auto const& soloQueueDPS)
                        {
                            return soloQueueDPS.second.playerClass == dpsClass;
                        });
                }

                // 7. DPS last classes played with the player (against first DPS)
                if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
                {
                    std::erase_if(orderedDPSSoloQueues, [dpsLastDPSList = dps1->GetLastDPSList(), dpsOwnerGUID = dps1->ownerGUID](auto const& soloQueueDPS)
                    {
                        return dpsLastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(dpsOwnerGUID);
                    });
                    if (orderedDPSSoloQueues.empty())
                        return false;

                    // 7.1 DPS last classes played with the player on nearest container (if not empty)
                    if (!nearestDPSSoloQueues.empty())
                        std::erase_if(nearestDPSSoloQueues, [dpsLastDPSList = dps1->GetLastDPSList(), dpsOwnerGUID = dps1->ownerGUID](auto const& soloQueueDPS)
                        {
                            return dpsLastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(dpsOwnerGUID);
                        });
                }

                // 8. Select container:
                // 8.1 If nearest container is not empty - select random element from it.
                SoloQueue const* dps2 = nullptr;
                if (!nearestDPSSoloQueues.empty())
                    dps2 = &Trinity::Containers::SelectRandomContainerElement(nearestDPSSoloQueues).second;
                else
                {
                    // 8.2 Select the one with lowest MMR diff
                    // 8.2.5 Sort if needed (e.g. First DPS taken from nearest container and now we want second DPS from main container - we must sort it before)
                    if (!alreadySorted)
                        std::sort(orderedDPSSoloQueues.begin(), orderedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                    dps2 = &orderedDPSSoloQueues.begin()->second;
                }

                if (!dps2)
                    return false;

                newList.push_back(*dps2);
                currentDPSList.insert(dps2->ownerGUID);
            }

            return true;
        }
        // Find 1 ranged dps and 1 heal
        case SPEC_DPS_MELEE:
        {
            currentDPSList.insert(soloQueue.ownerGUID);

            /* Find healer */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedHealSoloQueues = specList[SPEC_HEAL][0];
            if (orderedHealSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestHealSoloQueues = specList[SPEC_HEAL][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                {
                    return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                    {
                        return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* healer = nullptr;
            if (!nearestHealSoloQueues.empty())
                healer = &Trinity::Containers::SelectRandomContainerElement(nearestHealSoloQueues).second;
            else
            {
                // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedHealSoloQueues.begin(), orderedHealSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                healer = &orderedHealSoloQueues.begin()->second;
            }

            if (!healer)
                return false;

            newList.push_back(*healer);

            /* Find Ranged DPS */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedDPSSoloQueues = specList[SPEC_DPS_RANGED][0];
            if (orderedDPSSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestDPSSoloQueues = specList[SPEC_DPS_RANGED][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                {
                    return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                    });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                {
                    return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* dps = nullptr;
            if (!nearestDPSSoloQueues.empty())
                dps = &Trinity::Containers::SelectRandomContainerElement(nearestDPSSoloQueues).second;
            else
            {
                // 3.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedDPSSoloQueues.begin(), orderedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                dps = &orderedDPSSoloQueues.begin()->second;
            }

            if (!dps)
                return false;

            newList.push_back(*dps);
            currentDPSList.insert(dps->ownerGUID);

            return true;
        }
        // Find 1 melee dps and 1 heal
        case SPEC_DPS_RANGED:
        {
            currentDPSList.insert(soloQueue.ownerGUID);

            /* Find healer */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedHealSoloQueues = specList[SPEC_HEAL][0];
            if (orderedHealSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestHealSoloQueues = specList[SPEC_HEAL][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [playerClass = soloQueue.playerClass](auto const& soloQueueHeal) { return soloQueueHeal.second.playerClass == playerClass; });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                {
                    return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                });
                if (orderedHealSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestHealSoloQueues.empty())
                    std::erase_if(nearestHealSoloQueues, [ownerGUID = soloQueue.ownerGUID](auto const& soloQueueHeal)
                    {
                        return soloQueueHeal.second.GetLastDPSList().contains(ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* healer = nullptr;
            if (!nearestHealSoloQueues.empty())
                healer = &Trinity::Containers::SelectRandomContainerElement(nearestHealSoloQueues).second;
            else
            {
                // 4.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedHealSoloQueues.begin(), orderedHealSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                healer = &orderedHealSoloQueues.begin()->second;
            }

            if (!healer)
                return false;

            newList.push_back(*healer);

            /* Find Melee DPS */
            // 1. Check if main container is empty
            std::vector<std::pair<uint32, SoloQueue>>& orderedDPSSoloQueues = specList[SPEC_DPS_MELEE][0];
            if (orderedDPSSoloQueues.empty())
                return false;

            std::vector<std::pair<uint32, SoloQueue>>& nearestDPSSoloQueues = specList[SPEC_DPS_MELEE][1];

            // 2. Class stacking
            if (!sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING))
            {
                std::erase_if(orderedDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                {
                    return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 2.1 Class stacking on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [playerClass = soloQueue.playerClass, healerClass = healer->playerClass](auto const& soloQueueDPS)
                    {
                        return soloQueueDPS.second.playerClass == playerClass || soloQueueDPS.second.playerClass == healerClass;
                    });
            }

            // 3. DPS last classes played with the player
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK))
            {
                std::erase_if(orderedDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                {
                    return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                });
                if (orderedDPSSoloQueues.empty())
                    return false;

                // 3.1 DPS last classes played with the player on nearest container (if not empty)
                if (!nearestDPSSoloQueues.empty())
                    std::erase_if(nearestDPSSoloQueues, [lastDPSList = soloQueue.GetLastDPSList(), ownerGUID = soloQueue.ownerGUID, healerLastDPSList = healer->GetLastDPSList()](auto const& soloQueueDPS)
                    {
                        return lastDPSList.contains(soloQueueDPS.second.ownerGUID) || soloQueueDPS.second.GetLastDPSList().contains(ownerGUID) || healerLastDPSList.contains(soloQueueDPS.second.ownerGUID);
                    });
            }

            // 4. Select container:
            // 4.1 If nearest container is not empty - select random element from it.
            SoloQueue const* dps = nullptr;
            if (!nearestDPSSoloQueues.empty())
                dps = &Trinity::Containers::SelectRandomContainerElement(nearestDPSSoloQueues).second;
            else
            {
                // 3.2 Sort the main container by MMR diff and select the one with lowest MMR diff
                std::sort(orderedDPSSoloQueues.begin(), orderedDPSSoloQueues.end(), [](auto const& left, auto const& right) { return left.first < right.first; });
                dps = &orderedDPSSoloQueues.begin()->second;
            }

            if (!dps)
                return false;

            newList.push_back(*dps);
            currentDPSList.insert(dps->ownerGUID);

            return true;
        }
        default:
            break;
    }

    return false;
}

uint32 SoloQueueMgr::CalculateAverageMMR(std::vector<SoloQueue> const& list)
{
    uint32 averageMMR = 0;
    for (SoloQueue const& soloQueue : list)
        if (ArenaTeam const* arenaTeam = sArenaTeamMgr->GetArenaTeamById(soloQueue.arenaTeamId))
            averageMMR += arenaTeam->GetStats().MatchMakerRating;

    if (!averageMMR)
        return 0;

    averageMMR /= static_cast<uint32>(list.size());
    return averageMMR;
}

uint32 SoloQueueMgr::CalculateAverageRating(std::vector<SoloQueue> const& list)
{
    uint32 averageRating = 0;
    for (SoloQueue const& soloQueue : list)
        if (ArenaTeam const* arenaTeam = sArenaTeamMgr->GetArenaTeamById(soloQueue.arenaTeamId))
            averageRating += arenaTeam->GetStats().Rating;

    if (!averageRating)
        return 0;

    averageRating /= static_cast<uint32>(list.size());
    return averageRating;
}

uint32 SoloQueueMgr::CalculateAverageJoinTime(std::vector<SoloQueue> const& list)
{
    uint32 currentTimer = World::GetGameTimeMS();
    uint32 averageWaitingTimer = 0;
    for (SoloQueue const& soloQueue : list)
        if (currentTimer > soloQueue.joinTime)
            averageWaitingTimer += (currentTimer - soloQueue.joinTime);
        else
            return 0;

    if (!averageWaitingTimer)
        return 0;

    averageWaitingTimer /= static_cast<uint32>(list.size());
    return currentTimer - averageWaitingTimer;
}

void SoloQueueMgr::PrintSoloQueueStats(ChatHandler* handler)
{
    std::vector<uint32> specCount = { 0, 0, 0 };
    uint32 total = 0;
    for (auto& itr : _preparationQueue)
    {
        uint8 spec = itr.second.talentsSpecialization;
        ++specCount[spec];
        ++total;
    }

    std::string specs[3] = { "DPS", "Heal", "Tank" };

    handler->PSendSysMessage("Players in soloqueue: %u\nSpecializations:", total);
    for (uint8 i = 0; i < MAX_SPECES; ++i)
        handler->PSendSysMessage("%s: %u", specs[i].c_str(), specCount[i]);
}

uint32 SoloQueueMgr::GetTalentsSpecialization(Player* player)
{
    bool alternativeDPSSpecs = sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ALTERNATIVE_DPS_SPECS);
    uint8 playerClass = player->getClass();
    switch (playerClass)
    {
        case CLASS_ROGUE:
            if (alternativeDPSSpecs)
                return SPEC_DPS_MELEE;
            [[fallthrough]];
        case CLASS_HUNTER:
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            if (alternativeDPSSpecs)
                return SPEC_DPS_RANGED;
            return SPEC_DPS;
        default:
            break;
    }

    switch (player->GetMostPointsTalentTree())
    {
        case TALENT_TREE_WARRIOR_PROTECTION:
        case TALENT_TREE_PALADIN_PROTECTION:
        case TALENT_TREE_DEATH_KNIGHT_BLOOD:
            return SPEC_TANK;
        case TALENT_TREE_PALADIN_HOLY:
        case TALENT_TREE_PRIEST_DISCIPLINE:
        case TALENT_TREE_PRIEST_HOLY:
        case TALENT_TREE_SHAMAN_RESTORATION:
        case TALENT_TREE_DRUID_RESTORATION:
            return SPEC_HEAL;
        case TALENT_TREE_WARRIOR_ARMS:
        case TALENT_TREE_WARRIOR_FURY:
        case TALENT_TREE_DEATH_KNIGHT_FROST:
        case TALENT_TREE_DEATH_KNIGHT_UNHOLY:
        case TALENT_TREE_PALADIN_RETRIBUTION:
        case TALENT_TREE_SHAMAN_ENCHANCEMENT:
        case TALENT_TREE_DRUID_FERAL_COMBAT:
            if (alternativeDPSSpecs)
                return SPEC_DPS_MELEE;
            break;
        default:
            break;
    }

    return alternativeDPSSpecs ? SPEC_DPS_RANGED : SPEC_DPS;
}

