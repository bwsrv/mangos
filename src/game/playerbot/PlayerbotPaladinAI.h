/*
* Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
* Copyright (C) 2010 Blueboy
* Copyright (C) 2011 MangosR2 
*
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

#ifndef _PlayerbotPaladinAI_H
#define _PlayerbotPaladinAI_H

#include "PlayerbotClassAI.h"

enum
{
    Combat,
    Healing
};

enum PaladinSpells
{
    AURA_MASTERY_1                  = 31821,
    AVENGERS_SHIELD_1               = 31935,
    AVENGING_WRATH_1                = 31884,
    BEACON_OF_LIGHT_1               = 53563,
    BLESSING_OF_KINGS_1             = 20217,
    BLESSING_OF_MIGHT_1             = 19740,
    BLESSING_OF_SANCTUARY_1         = 20911,
    BLESSING_OF_WISDOM_1            = 19742,
    CLEANSE_1                       = 4987,
    CONCENTRATION_AURA_1            = 19746,
    CONSECRATION_1                  = 26573,
    CRUSADER_AURA_1                 = 32223,
    CRUSADER_STRIKE_1               = 35395,
    DEVOTION_AURA_1                 = 465,
    DIVINE_FAVOR_1                  = 20216,
    DIVINE_ILLUMINATION_1           = 31842,
    DIVINE_INTERVENTION_1           = 19752,
    DIVINE_PLEA_1                   = 54428,
    DIVINE_PROTECTION_1             = 498,
    DIVINE_SACRIFICE_1              = 64205,
    DIVINE_SHIELD_1                 = 642,
    DIVINE_STORM_1                  = 53385,
    EXORCISM_1                      = 879,
    FIRE_RESISTANCE_AURA_1          = 19891,
    FLASH_OF_LIGHT_1                = 19750,
    FROST_RESISTANCE_AURA_1         = 19888,
    GREATER_BLESSING_OF_KINGS_1     = 25898,
    GREATER_BLESSING_OF_MIGHT_1     = 25782,
    GREATER_BLESSING_OF_SANCTUARY_1 = 25899,
    GREATER_BLESSING_OF_WISDOM_1    = 25894,
    HAMMER_OF_JUSTICE_1             = 853,
    HAMMER_OF_THE_RIGHTEOUS_1       = 53595,
    HAMMER_OF_WRATH_1               = 24275,
    HAND_OF_FREEDOM_1               = 1044,
    HAND_OF_PROTECTION_1            = 1022,
    HAND_OF_RECKONING_1             = 62124,
    HAND_OF_SACRIFICE_1             = 6940,
    HAND_OF_SALVATION_1             = 1038,
    HOLY_LIGHT_1                    = 635,
    HOLY_SHIELD_1                   = 20925,
    HOLY_SHOCK_1                    = 20473,
    HOLY_WRATH_1                    = 2812,
    JUDGEMENT_OF_JUSTICE_1          = 53407,
    JUDGEMENT_OF_LIGHT_1            = 20271,
    JUDGEMENT_OF_WISDOM_1           = 53408,
    LAY_ON_HANDS_1                  = 633,
    PURIFY_1                        = 1152,
    REDEMPTION_1                    = 7328,
    REPENTANCE_1                    = 20066,
    RETRIBUTION_AURA_1              = 7294,
    RIGHTEOUS_DEFENSE_1             = 31789,
    RIGHTEOUS_FURY_1                = 25780,
    SACRED_SHIELD_1                 = 53601,
    SEAL_OF_COMMAND_1               = 20375,
    SEAL_OF_CORRUPTION_1            = 53736,
    SEAL_OF_JUSTICE_1               = 20164,
    SEAL_OF_LIGHT_1                 = 20165,
    SEAL_OF_RIGHTEOUSNESS_1         = 21084,
    SEAL_OF_VENGEANCE_1             = 31801,
    SEAL_OF_WISDOM_1                = 20166,
    SENSE_UNDEAD_1                  = 5502,
    SHADOW_RESISTANCE_AURA_1        = 19876,
    SHIELD_OF_RIGHTEOUSNESS_1       = 53600,
    TURN_EVIL_1                     = 10326
};
//class Player;

class MANGOS_DLL_SPEC PlayerbotPaladinAI : PlayerbotClassAI
{
public:
    PlayerbotPaladinAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotPaladinAI();

    // all combat actions go here
    bool DoFirstCombatManeuver(Unit*);
    void DoNextCombatManeuver(Unit*);

    // all non combat actions go here, ex buffs, heals, rezzes
    void DoNonCombatActions();

    // buff a specific player, usually a real PC who is not in group
    bool BuffPlayer(Player *target);

private:
    // Heals the target based off its hps
    bool HealTarget (Unit *target);
    // Bless target using greater blessing if possible
    bool Bless(uint32 spellId, Unit *target);

    // Retribution
    uint32 RETRIBUTION_AURA,
           SEAL_OF_COMMAND,
           JUDGEMENT_OF_LIGHT,
           JUDGEMENT_OF_WISDOM,
           GREATER_BLESSING_OF_WISDOM,
           GREATER_BLESSING_OF_MIGHT,
           BLESSING_OF_WISDOM,
           BLESSING_OF_MIGHT,
           HAMMER_OF_JUSTICE,
           RIGHTEOUS_FURY,
           CRUSADER_AURA,
           CRUSADER_STRIKE,
           AVENGING_WRATH,
           DIVINE_STORM,
           JUDGEMENT_OF_JUSTICE;

    // Holy
    uint32 FLASH_OF_LIGHT,
           HOLY_LIGHT,
           DIVINE_SHIELD,
           HAMMER_OF_WRATH,
           CONSECRATION,
           CONCENTRATION_AURA,
           DIVINE_FAVOR,
           SACRED_SHIELD,
           HOLY_SHOCK,
           HOLY_WRATH,
           LAY_ON_HANDS,
           EXORCISM,
           REDEMPTION,
           DIVINE_PLEA,
           SEAL_OF_CORRUPTION,
           SEAL_OF_JUSTICE,
           SEAL_OF_LIGHT,
           SEAL_OF_RIGHTEOUSNESS,
           SEAL_OF_VENGEANCE,
           SEAL_OF_WISDOM;

    // Protection
    uint32 GREATER_BLESSING_OF_KINGS,
           BLESSING_OF_KINGS,
           HAND_OF_PROTECTION,
           SHADOW_RESISTANCE_AURA,
           DEVOTION_AURA,
           FIRE_RESISTANCE_AURA,
           FROST_RESISTANCE_AURA,
           DEFENSIVE_STANCE,
           BERSERKER_STANCE,
           BATTLE_STANCE,
           DIVINE_SACRIFICE,
           DIVINE_PROTECTION,
           DIVINE_INTERVENTION,
           HOLY_SHIELD,
           AVENGERS_SHIELD,
           RIGHTEOUS_DEFENSE,
           BLESSING_OF_SANCTUARY,
           GREATER_BLESSING_OF_SANCTUARY,
           HAND_OF_SACRIFICE,
           SHIELD_OF_RIGHTEOUSNESS;

    // cannot be protected
    uint32 FORBEARANCE;

    // first aid
    uint32 RECENTLY_BANDAGED;

    // racial
    uint32 ARCANE_TORRENT,
           GIFT_OF_THE_NAARU,
           STONEFORM,
           ESCAPE_ARTIST,
           EVERY_MAN_FOR_HIMSELF,
           SHADOWMELD,
           BLOOD_FURY,
           WAR_STOMP,
           BERSERKING,
           WILL_OF_THE_FORSAKEN;

    uint32 SpellSequence, CombatCounter, HealCounter;
};

#endif
