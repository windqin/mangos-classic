/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 5097, 5098
SDCategory: Game Objects
EndScriptData

*/

#include "AI/ScriptDevAI/PreCompiledHeader.h"
/* ContentData
go_andorhal_tower
EndContentData */


/*######
## go_andorhal_tower
######*/

enum
{
    QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE = 5097,
    QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE    = 5098,
    NPC_ANDORHAL_TOWER_1                     = 10902,
    NPC_ANDORHAL_TOWER_2                     = 10903,
    NPC_ANDORHAL_TOWER_3                     = 10904,
    NPC_ANDORHAL_TOWER_4                     = 10905,
    GO_ANDORHAL_TOWER_1                      = 176094,
    GO_ANDORHAL_TOWER_2                      = 176095,
    GO_ANDORHAL_TOWER_3                      = 176096,
    GO_ANDORHAL_TOWER_4                      = 176097
};

bool GOUse_go_andorhal_tower(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 uiKillCredit = 0;
        switch (pGo->GetEntry())
        {
            case GO_ANDORHAL_TOWER_1:   uiKillCredit = NPC_ANDORHAL_TOWER_1;   break;
            case GO_ANDORHAL_TOWER_2:   uiKillCredit = NPC_ANDORHAL_TOWER_2;   break;
            case GO_ANDORHAL_TOWER_3:   uiKillCredit = NPC_ANDORHAL_TOWER_3;   break;
            case GO_ANDORHAL_TOWER_4:   uiKillCredit = NPC_ANDORHAL_TOWER_4;   break;
        }
        if (uiKillCredit)
            pPlayer->KilledMonsterCredit(uiKillCredit);
    }
    return true;
}

/*####
## go_bells
####*/

enum
{
    // Bells
    EVENT_ID_BELLS = 1024
};

enum BellHourlySoundFX
{
    BELLTOLLTRIBAL = 6595, // Horde
    BELLTOLLHORDE = 6675,
    BELLTOLLALLIANCE = 6594, // Alliance
    BELLTOLLNIGHTELF = 6674,
    BELLTOLLDWARFGNOME = 7234
};

enum BellHourlySoundAreas
{
    UNDERCITY_AREA = 1497,
    IRONFORGE_1_AREA = 809,
    IRONFORGE_2_AREA = 1537,
    DARNASSUS_AREA = 1657,
    TELDRASSIL_ZONE = 141,
    BRILL_AREA = 2118,
    TARREN_MILL_AREA = 272
};

enum BellHourlyObjects
{
    GO_HORDE_BELL = 175885,
    GO_ALLIANCE_BELL = 176573
};

struct go_ai_bell : public GameObjectAI
{
    go_ai_bell(GameObject* go) : GameObjectAI(go), m_uiBellTolls(0), m_uiBellSound(GetBellSound(go)), m_uiBellTimer(0)
    {
        m_go->SetNotifyOnEventState(true);
        m_go->SetActiveObjectState(true);
    }

    uint32 m_uiBellTolls;
    uint32 m_uiBellSound;
    uint32 m_uiBellTimer;

    uint32 GetBellSound(GameObject* pGo)
    {
        uint32 soundId;
        switch (pGo->GetEntry())
        {
            case GO_HORDE_BELL:
                switch (pGo->GetAreaId())
                {
                    case UNDERCITY_AREA:
                    case BRILL_AREA:
                    case TARREN_MILL_AREA:
                        soundId = BELLTOLLTRIBAL;
                        break;
                    default:
                        soundId = BELLTOLLHORDE;
                        break;
                }
                break;
            case GO_ALLIANCE_BELL:
            {
                switch (pGo->GetAreaId())
                {
                    case IRONFORGE_1_AREA:
                    case IRONFORGE_2_AREA:
                        soundId = BELLTOLLDWARFGNOME;
                        break;
                    case DARNASSUS_AREA:
                        soundId = BELLTOLLNIGHTELF;
                        break;
                    default:
                        soundId = BELLTOLLALLIANCE;
                        break;
                }
                break;
            }
        }
        return soundId;
    }

    void OnEventHappened(uint16 event_id, bool activate, bool resume) override
    {
        if (event_id == EVENT_ID_BELLS && activate && !resume)
        {
            time_t curTime = time(nullptr);
            tm localTm = *localtime(&curTime);
            m_uiBellTolls = (localTm.tm_hour + 11) % 12;

            if (m_uiBellTolls)
                m_uiBellTimer = 3000;

            m_go->GetMap()->PlayDirectSoundToMap(m_uiBellSound, m_go->GetZoneId());
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiBellTimer)
        {
            if (m_uiBellTimer <= uiDiff)
            {
                m_go->GetMap()->PlayDirectSoundToMap(m_uiBellSound, m_go->GetZoneId());

                m_uiBellTolls--;
                if (m_uiBellTolls)
                    m_uiBellTimer = 3000;
                else
                    m_uiBellTimer = 0;
            }
            else
                m_uiBellTimer -= uiDiff;
        }
    }
};

GameObjectAI* GetAI_go_bells(GameObject* go)
{
    return new go_ai_bell(go);
}

void AddSC_go_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_andorhal_tower";
    pNewScript->pGOUse =          &GOUse_go_andorhal_tower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_bells";
    pNewScript->GetGameObjectAI = &GetAI_go_bells;
    pNewScript->RegisterSelf();
}
