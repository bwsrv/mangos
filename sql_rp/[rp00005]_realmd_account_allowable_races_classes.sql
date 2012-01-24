ALTER TABLE account ADD COLUMN allowable_races int(10) UNSIGNED NOT NULL default 10 AFTER rp_allow;
ALTER TABLE account ADD COLUMN allowable_classes int(10) UNSIGNED NOT NULL default 13 AFTER allowable_races;

/*
allowable_races and allowable_classes - bitmasks

Available by default:
  races:   orc, night elf
  classes: warrior, hunter, rogue

Races:
    HUMAN         - 0x0001 - 1
    ORC           - 0x0002 - 2
    DWARF         - 0x0004 - 4
    NIGHTELF      - 0x0008 - 8
    UNDEAD        - 0x0010 - 16
    TAUREN        - 0x0020 - 32
    GNOME         - 0x0040 - 64
    TROLL         - 0x0080 - 128
    BLOODELF      - 0x0200 - 512
    DRAENEI       - 0x0400 - 1024

Classes:
    WARRIOR       - 0x0001 - 1
    PALADIN       - 0x0002 - 2
    HUNTER        - 0x0004 - 4
    ROGUE         - 0x0008 - 8
    PRIEST        - 0x0010 - 16
    DEATH_KNIGHT  - 0x0020 - 32
    SHAMAN        - 0x0040 - 64
    MAGE          - 0x0080 - 128
    WARLOCK       - 0x0100 - 256
    DRUID         - 0x0400 - 1024
*/
