SET @FOUNTAIN:= 151164;

UPDATE `gameobject` SET `state` = 0 WHERE `guid` = @FOUNTAIN;

SET @TEXT:= "Dedicated to those that fell to the Scourge during the war in the frozen wastes.                     
$b                Guild <Sunced> 10/22/2020$B                        
$B                        Agmashgodx
$B                        Bas 
$B                        Bengan 
$B                        Brisko 
$B                        Chrys 
$B                        Copycopycopy 
$B                        Daviri 
$B                        Doctox 
$B                        Drafter 
$B                        Droma 
$B                        Duality 
$B                        Fingerbangxd 
$B                        Foxy 
$B                        Jamie 
$B                        Luuz 
$B                        Marsxd 
$B                        Shasmvp 
$B                        Shocko 
$B                        Shzzie 
$B                        Spoonmanz 
$B                        Tuss 
$B                        Viho 
$B                        Vorpalmage 
$B                        Youremom
$B                        Zovulontwo";

UPDATE `page_text` SET `text` = @TEXT WHERE `entry` = 3605;

DELETE FROM `gameobject` WHERE `guid` = 342 AND `id` = 202443;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(342, 202443, 571, 1, 1, 5804.53, 638.542, 647.648, 2.46091, 0, 0, 0, 1, 120, 255, 1, 0);
