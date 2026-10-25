class CfgPatches
{
    class RepairToPristineUpdated
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data","DZ_Scripts"};
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
        version = "2.1";
        extra = 0;
        type = "mod";
        dependencies[] = {"Game","World","Mission"};
        class defs
        {
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