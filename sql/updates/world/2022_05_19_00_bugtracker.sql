-- Omen of Clarity
UPDATE `spell_proc_event` SET `ppmRate`=0, `CustomChance`=6 WHERE `entry`=16864;

-- Ravenous Jaws
UPDATE `creature` SET `position_z`=-2.5 WHERE `guid`=124279;
UPDATE `creature` SET `position_z`=-6 WHERE `guid`=124283;
UPDATE `creature` SET `position_x`=7788.080, `position_y`=4875.196, `position_z`=-4.529 WHERE `guid`=124285;
UPDATE `creature` SET `position_z`=-2.5 WHERE `guid`=124286;
UPDATE `creature` SET `position_z`=-9 WHERE `guid`=124287;
UPDATE `creature` SET `position_z`=-12 WHERE `guid`=124289;
UPDATE `creature` SET `position_z`=-10 WHERE `guid`=124290;
UPDATE `creature` SET `MovementType`=1, `spawndist`=5 WHERE `id`=29392;
