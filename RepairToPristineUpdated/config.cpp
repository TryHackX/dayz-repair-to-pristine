class CfgPatches
{
    class RepairToPristineUpdated
    {
        units[] = {"RTP_Toolbox","RTP_Ratchet","RTP_SteelBrush"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Gear_Tools","DZ_Gear_Consumables"};
    };
};

class CfgMods
{
    class RepairToPristineUpdated
    {
        dir = "RepairToPristineUpdated";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Repair To Pristine Updated";
        credits = "TryHackX";
        author = "TryHackX";
        authorID = "76561199846115789";
        version = "2.2";
        extra = 0;
        type = "mod";
        dependencies[] = {"Game","World","Mission"};
        class defs
        {
            class imageSets
            {
                files[] = {"RepairToPristineUpdated/gui/imagesets/rtp_icons.imageset"};
            };
            class gameScriptModule
            {
                value = "";
                files[] = {"RepairToPristineUpdated/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"RepairToPristineUpdated/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"RepairToPristineUpdated/scripts/5_Mission"};
            };
        };
    };
};

class CfgVehicles
{
    class Inventory_Base;
    class Container_Base;

    // Toolbox: tools go into their own slots (or the cargo), repairs are set in the server config
    class RTP_Toolbox: Container_Base
    {
        scope = 2;
        displayName = "$STR_RTP_Toolbox0";
        descriptionShort = "$STR_RTP_Toolbox1";
        model = "RepairToPristineUpdated\data\toolbox\rtp_toolbox.p3d";
        weight = 3000;
        itemSize[] = {6,4};
        itemsCargoSize[] = {6,4};
        allowOwnedCargoManipulation = 1;
        canBeDigged = 1;
        isMeleeWeapon = 1;
        attachments[] = {"RTP_Wrench","RTP_Ratchet","RTP_Screwdriver","RTP_Pliers","RTP_Hammer","RTP_SteelBrush","RTP_DuctTape","RTP_EpoxyPutty","RTP_ElectronicKit"};
        hiddenSelections[] = {"zbytek"};
        hiddenSelectionsTextures[] = {"RepairToPristineUpdated\data\toolbox\toolbox_co.paa"};
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 300;
                    healthLevels[] =
                    {
                        {1.0,{"RepairToPristineUpdated\data\toolbox\toolbox.rvmat"}},
                        {0.7,{"RepairToPristineUpdated\data\toolbox\toolbox.rvmat"}},
                        {0.5,{"RepairToPristineUpdated\data\toolbox\toolbox_damage.rvmat"}},
                        {0.3,{"RepairToPristineUpdated\data\toolbox\toolbox_damage.rvmat"}},
                        {0.0,{"RepairToPristineUpdated\data\toolbox\toolbox_destruct.rvmat"}}
                    };
                };
            };
        };
        soundImpactType = "metal";
        class AnimEvents
        {
            class SoundWeapon
            {
                class pickup
                {
                    soundSet = "sewingkit_pickup_SoundSet";
                    id = 797;
                };
                class drop
                {
                    soundset = "ammobox_drop_SoundSet";
                    id = 898;
                };
            };
        };
    };

    class RTP_Ratchet: Inventory_Base
    {
        scope = 2;
        displayName = "$STR_RTP_Ratchet0";
        descriptionShort = "$STR_RTP_Ratchet1";
        model = "RepairToPristineUpdated\data\ratchet\rtp_ratchet.p3d";
        rotationFlags = 17;
        weight = 450;
        itemSize[] = {1,3};
        fragility = 0.001;
        inventorySlot[] = {"RTP_Ratchet"};
        isMeleeWeapon = 1;
        openItemSpillRange[] = {20,40};
        hiddenSelections[] = {"zbytek"};
        hiddenSelectionsTextures[] = {"RepairToPristineUpdated\data\ratchet\ratchet_co.paa"};
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0,{"RepairToPristineUpdated\data\ratchet\ratchet.rvmat"}},
                        {0.7,{"RepairToPristineUpdated\data\ratchet\ratchet.rvmat"}},
                        {0.5,{"RepairToPristineUpdated\data\ratchet\ratchet_damage.rvmat"}},
                        {0.3,{"RepairToPristineUpdated\data\ratchet\ratchet_damage.rvmat"}},
                        {0.0,{"RepairToPristineUpdated\data\ratchet\ratchet_destruct.rvmat"}}
                    };
                };
            };
        };
        soundImpactType = "metal";
        class MeleeModes
        {
            class Default
            {
                ammo = "MeleeBluntLight_1";
                range = 1.4;
            };
            class Heavy
            {
                ammo = "MeleeBluntHeavy_1";
                range = 1.4;
            };
            class Sprint
            {
                ammo = "MeleeBluntHeavy_1";
                range = 3.9;
            };
        };
        class AnimEvents
        {
            class SoundWeapon
            {
                class drop
                {
                    soundset = "wrench_drop_SoundSet";
                    id = 898;
                };
            };
        };
    };

    class RTP_SteelBrush: Inventory_Base
    {
        scope = 2;
        displayName = "$STR_RTP_SteelBrush0";
        descriptionShort = "$STR_RTP_SteelBrush1";
        model = "RepairToPristineUpdated\data\steelbrush\rtp_steelbrush.p3d";
        rotationFlags = 17;
        weight = 200;
        itemSize[] = {1,3};
        fragility = 0.01;
        inventorySlot[] = {"RTP_SteelBrush"};
        openItemSpillRange[] = {20,40};
        hiddenSelections[] = {"zbytek"};
        hiddenSelectionsTextures[] = {"RepairToPristineUpdated\data\steelbrush\steelbrush_co.paa"};
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 80;
                    healthLevels[] =
                    {
                        {1.0,{"RepairToPristineUpdated\data\steelbrush\steelbrush.rvmat"}},
                        {0.7,{"RepairToPristineUpdated\data\steelbrush\steelbrush.rvmat"}},
                        {0.5,{"RepairToPristineUpdated\data\steelbrush\steelbrush_damage.rvmat"}},
                        {0.3,{"RepairToPristineUpdated\data\steelbrush\steelbrush_damage.rvmat"}},
                        {0.0,{"RepairToPristineUpdated\data\steelbrush\steelbrush_destruct.rvmat"}}
                    };
                };
            };
        };
        soundImpactType = "wood";
    };

    // vanilla tools and materials fit into the toolbox slots ("+=" keeps the slots of other mods)
    class Wrench: Inventory_Base
    {
        inventorySlot[] += {"RTP_Wrench"};
    };
    class Screwdriver: Inventory_Base
    {
        inventorySlot[] += {"RTP_Screwdriver"};
    };
    class Pliers: Inventory_Base
    {
        inventorySlot[] += {"RTP_Pliers"};
    };
    class Hammer: Inventory_Base
    {
        inventorySlot[] += {"RTP_Hammer"};
    };
    class DuctTape: Inventory_Base
    {
        inventorySlot[] += {"RTP_DuctTape"};
    };
    class EpoxyPutty: Inventory_Base
    {
        inventorySlot[] += {"RTP_EpoxyPutty"};
    };
    class ElectronicRepairKit: Inventory_Base
    {
        inventorySlot[] += {"RTP_ElectronicKit"};
    };
};

class CfgSlots
{
    class Slot_RTP_Wrench
    {
        name = "RTP_Wrench";
        displayName = "$STR_CfgVehicles_Wrench0";
        ghostIcon = "set:rtp_icons image:wrench";
    };
    class Slot_RTP_Ratchet
    {
        name = "RTP_Ratchet";
        displayName = "$STR_RTP_Ratchet0";
        ghostIcon = "set:rtp_icons image:ratchet";
    };
    class Slot_RTP_Screwdriver
    {
        name = "RTP_Screwdriver";
        displayName = "$STR_CfgVehicles_Screwdriver0";
        ghostIcon = "set:rtp_icons image:screwdriver";
    };
    class Slot_RTP_Pliers
    {
        name = "RTP_Pliers";
        displayName = "$STR_CfgVehicles_Pliers0";
        ghostIcon = "set:dayz_inventory image:plierstools";
    };
    class Slot_RTP_Hammer
    {
        name = "RTP_Hammer";
        displayName = "$STR_CfgVehicles_Hammer0";
        ghostIcon = "set:dayz_inventory image:hammertools";
    };
    class Slot_RTP_SteelBrush
    {
        name = "RTP_SteelBrush";
        displayName = "$STR_RTP_SteelBrush0";
        ghostIcon = "set:rtp_icons image:steelbrush";
    };
    class Slot_RTP_DuctTape
    {
        name = "RTP_DuctTape";
        displayName = "$STR_CfgVehicles_DuctTape0";
        ghostIcon = "set:rtp_icons image:ducttape";
    };
    class Slot_RTP_EpoxyPutty
    {
        name = "RTP_EpoxyPutty";
        displayName = "$STR_CfgVehicles_EpoxyPutty0";
        ghostIcon = "set:rtp_icons image:epoxy";
    };
    class Slot_RTP_ElectronicKit
    {
        name = "RTP_ElectronicKit";
        displayName = "$STR_CfgVehicles_ElectronicRepairKit0";
        ghostIcon = "set:rtp_icons image:electronickit";
    };
};
