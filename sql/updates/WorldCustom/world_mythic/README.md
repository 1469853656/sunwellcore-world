#One sql might rely on other, it's shit, i know, so here is an execution order:

1. mythic_creature_world_tables.sql - Creates base tables for mythic systems
2. mythic_affix_data
3. mythic_companion_world
4. mythic_events
5. Execute SQLs in folders (Blood furnace, nexus, ZulFarrak)
6. mythic_populate_world_data
7. mythic_creature_template_mythic
8. mythic_creature_mythic_spell_values


source sql/updates/WorldCustom/world_mythic/mythic_create_world_tables.sql
source sql/updates/WorldCustom/world_mythic/mythic_affix_data.sql
source sql/updates/WorldCustom/world_mythic/mythic_companion_world.sql
source sql/updates/WorldCustom/world_mythic/mythic_events.sql
source sql/updates/WorldCustom/world_mythic/BloodFurnace/blood_furance_mythic_conversion.sql
source sql/updates/WorldCustom/world_mythic/BloodFurnace/boss_bogrokk.sql
source sql/updates/WorldCustom/world_mythic/BloodFurnace/boss_kelidan.sql
source sql/updates/WorldCustom/world_mythic/BloodFurnace/boss_the_maker.sql
source sql/updates/WorldCustom/world_mythic/Nexus/boss_anomlus.sql
source sql/updates/WorldCustom/world_mythic/Nexus/boss_grand_magus_telestra.sql
source sql/updates/WorldCustom/world_mythic/Nexus/boss_keristrasza.sql
source sql/updates/WorldCustom/world_mythic/Nexus/boss_ormorok.sql
source sql/updates/WorldCustom/world_mythic/Nexus/nexus_gauntlet_mythic.sql
source sql/updates/WorldCustom/world_mythic/Nexus/nexus_mythic_conversion.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/boss_antusul.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/boss_theka_the_martyr.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/boss_velratha.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/boss_zumrah.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/urkoz_ruuzlu.sql
source sql/updates/WorldCustom/world_mythic/ZulFarrak/zulfarrak_mythic_conversion.sql
source sql/updates/WorldCustom/world_mythic/mythic_populate_world_data.sql
source sql/updates/WorldCustom/world_mythic/mythic_creature_template_mythic.sql
source sql/updates/WorldCustom/world_mythic/mythic_creature_mythic_spell_values.sql


default mythic_states for character database
```
TRUNCATE TABLE `mythic_states`;
INSERT INTO `mythic_states` VALUES
(0, "1603877400"),
(1, "209;542;576"),
(2, "1"),
(3, "1"),
(4, "0");
```

