//! Vehicle repair groups, each with its own settings
enum RTP_VehicleRepairGroup
{
    PARTS,      // car doors, hoods, trunks, motorbike parts
    CHASSIS,    // vehicle body zones (everything except the engine)
    ENGINE
}

//! Repair rules for one vehicle group. Kit usage: vanilla takes 25 % of the kit per repair, scaled by the modifiers.
class RepairToPristineUpdatedVehicleSettings
{
    bool EnablePristineRepair;
    bool EnableRuinedRepair;
    float RepairKitModifier;
    float PristineRepairKitModifier;
    float RuinedRepairKitModifier;

    void RepairToPristineUpdatedVehicleSettings()
    {
        EnablePristineRepair = true;
        EnableRuinedRepair = false;
        RepairKitModifier = 1.0;
        PristineRepairKitModifier = 1.2;
        RuinedRepairKitModifier = 2.5;
    }
}

//! A tool or material a toolbox repair needs (class name, subclasses count too). Wear: % of the tool's health used
//! per repair; items with quantity (duct tape, epoxy putty, electronic repair kit ...) lose that % of their quantity.
class RTP_ToolSetting
{
    string ClassName;
    float Wear;

    void RTP_ToolSetting(string className = "", float wear = 0)
    {
        ClassName = className;
        Wear = wear;
    }
}

//! A repair kit for the items of a part kit group. Cost: % of the kit used per repair.
class RTP_KitSetting
{
    string ClassName;
    float Cost;

    void RTP_KitSetting(string className = "", float cost = 0)
    {
        ClassName = className;
        Cost = cost;
    }
}

//! Rules of one repair group: which items (class names, subclasses count too), which health levels and how much
//! of the kit / tools a repair takes (the modifiers multiply kit costs and tool wear).
class RTP_RepairGroup
{
    bool Enabled;
    bool EnablePristineRepair;      // worn -> pristine
    bool EnableRuinedRepair;        // ruined -> badly damaged
    float RepairModifier;           // normal repairs (badly damaged -> damaged -> worn)
    float PristineRepairModifier;   // worn -> pristine
    float RuinedRepairModifier;     // ruined -> badly damaged
    ref TStringArray Items;

    bool Matches(Object item)
    {
        if (!item || !Items)
            return false;

        foreach (string type : Items)
        {
            if (item.IsKindOf(type))
                return true;
        }

        return false;
    }

    bool CanRepairLevel(int level)
    {
        if (!Enabled || level <= GameConstants.STATE_PRISTINE)
            return false;

        if (level >= GameConstants.STATE_RUINED)
            return EnableRuinedRepair;

        if (level == GameConstants.STATE_WORN)
            return EnablePristineRepair;

        return true;
    }

    float GetModifier(int level)
    {
        if (level >= GameConstants.STATE_RUINED)
            return RuinedRepairModifier;

        if (level == GameConstants.STATE_WORN)
            return PristineRepairModifier;

        return RepairModifier;
    }

    //! The json loader turns a group missing in the file into a blank one (off, no items, all modifiers 0)
    bool IsBlank()
    {
        return !Enabled && (!Items || Items.Count() == 0) && RepairModifier == 0 && PristineRepairModifier == 0 && RuinedRepairModifier == 0;
    }

    void SetRules(bool enabled, float modifier)
    {
        Enabled = enabled;
        EnablePristineRepair = true;
        EnableRuinedRepair = false;
        RepairModifier = modifier;
        PristineRepairModifier = 1.2;
        RuinedRepairModifier = 2.5;
    }

    //! true when a value had to be corrected
    protected bool ClampRules()
    {
        bool changed = RepairModifier < 0 || PristineRepairModifier < 0 || RuinedRepairModifier < 0 || !Items;
        RepairModifier = Math.Max(RepairModifier, 0);
        PristineRepairModifier = Math.Max(PristineRepairModifier, 0);
        RuinedRepairModifier = Math.Max(RuinedRepairModifier, 0);
        if (!Items)
            Items = new TStringArray();
        return changed;
    }
}

//! Repairs with vanilla kits under the group's own rules (radiators, plugs, batteries, the toolbox itself)
class RTP_KitRepairGroup : RTP_RepairGroup
{
    ref array<ref RTP_KitSetting> Kits;

    void AddKit(string className, float cost)
    {
        if (!Kits)
            Kits = new array<ref RTP_KitSetting>();

        RTP_KitSetting kit = new RTP_KitSetting(className, cost);
        Kits.Insert(kit);
    }

    //! Cost in % of the kit for one repair (before the modifiers), -1 when this kit cannot repair the group
    float GetKitCost(Object kit)
    {
        if (!kit || !Kits)
            return -1;

        foreach (RTP_KitSetting k : Kits)
        {
            if (k && kit.IsKindOf(k.ClassName))
                return k.Cost;
        }

        return -1;
    }

    bool Clamp()
    {
        bool changed = ClampRules();
        if (!Kits)
        {
            Kits = new array<ref RTP_KitSetting>();
            changed = true;
        }
        for (int i = Kits.Count() - 1; i >= 0; i--)
        {
            if (!Kits[i])
            {
                Kits.Remove(i);
                changed = true;
            }
        }
        foreach (RTP_KitSetting k : Kits)
        {
            if (k.Cost < 0 || k.Cost > 100)
            {
                k.Cost = Math.Clamp(k.Cost, 0, 100);
                changed = true;
            }
        }
        return changed;
    }
}

//! Repairs with the toolbox in hands: the listed tools must be in the toolbox (slot or cargo)
class RTP_ToolboxRepairGroup : RTP_RepairGroup
{
    float RepairTime;               // seconds per repair (engine: per repair step)
    float ToolboxWear;              // % of the toolbox's own health per repair, 0 = the toolbox does not wear
    ref array<ref RTP_ToolSetting> Tools;

    void AddTool(string className, float wear)
    {
        if (!Tools)
            Tools = new array<ref RTP_ToolSetting>();

        RTP_ToolSetting tool = new RTP_ToolSetting(className, wear);
        Tools.Insert(tool);
    }

    bool Clamp()
    {
        bool changed = ClampRules();
        if (!Tools)
        {
            Tools = new array<ref RTP_ToolSetting>();
            changed = true;
        }
        float repairTime = Math.Clamp(RepairTime, 1, 120);
        float toolboxWear = Math.Clamp(ToolboxWear, 0, 100);
        if (repairTime != RepairTime || toolboxWear != ToolboxWear)
            changed = true;
        RepairTime = repairTime;
        ToolboxWear = toolboxWear;
        for (int i = Tools.Count() - 1; i >= 0; i--)
        {
            if (!Tools[i])
            {
                Tools.Remove(i);
                changed = true;
            }
        }
        foreach (RTP_ToolSetting tool : Tools)
        {
            if (tool.Wear < 0 || tool.Wear > 100)
            {
                tool.Wear = Math.Clamp(tool.Wear, 0, 100);
                changed = true;
            }
        }
        return changed;
    }
}

//! Kit repairs with their own rules: items vanilla cannot repair with a kit, and batteries (vanilla: Electronic Repair
//! Kit, cost 25). A listed item uses only these rules and kits.
class RepairToPristineUpdatedPartKitSettings
{
    ref RTP_KitRepairGroup Radiator;
    ref RTP_KitRepairGroup SparkPlug;
    ref RTP_KitRepairGroup Battery;
    ref RTP_KitRepairGroup Toolbox;

    static RTP_KitRepairGroup DefaultGroup(string name)
    {
        RTP_KitRepairGroup g = new RTP_KitRepairGroup();
        switch (name)
        {
            case "Radiator":
                g.SetRules(false, 0.8);
                g.Items = {"CarRadiator", "ExpansionHydraulicHoses"};
                g.AddKit("DuctTape", 25);
                g.AddKit("EpoxyPutty", 25);
                g.AddKit("Blowtorch", 25);
                break;
            case "SparkPlug":
                g.SetRules(false, 0.8);
                g.Items = {"SparkPlug", "GlowPlug"};
                g.AddKit("ElectronicRepairKit", 10);
                break;
            case "Battery":
                g.SetRules(true, 0.8);
                g.Items = {"CarBattery", "TruckBattery"};
                g.AddKit("ElectronicRepairKit", 25);
                break;
            case "Toolbox":
                g.SetRules(true, 0.8);
                g.Items = {"RTP_Toolbox"};
                g.AddKit("EpoxyPutty", 25);
                g.AddKit("Blowtorch", 25);
                break;
        }
        return g;
    }

    void SetDefaults()
    {
        Radiator = DefaultGroup("Radiator");
        SparkPlug = DefaultGroup("SparkPlug");
        Battery = DefaultGroup("Battery");
        Toolbox = DefaultGroup("Toolbox");
    }

    //! Groups missing in the loaded file get the defaults; true when something was added or corrected
    bool Validate()
    {
        bool changed = false;
        if (!Radiator || Radiator.IsBlank())
        {
            Radiator = DefaultGroup("Radiator");
            changed = true;
        }
        if (!SparkPlug || SparkPlug.IsBlank())
        {
            SparkPlug = DefaultGroup("SparkPlug");
            changed = true;
        }
        if (!Battery || Battery.IsBlank())
        {
            Battery = DefaultGroup("Battery");
            changed = true;
        }
        if (!Toolbox || Toolbox.IsBlank())
        {
            Toolbox = DefaultGroup("Toolbox");
            changed = true;
        }
        bool clampedRadiator = Radiator.Clamp();
        bool clampedPlug = SparkPlug.Clamp();
        bool clampedBattery = Battery.Clamp();
        bool clampedToolbox = Toolbox.Clamp();
        return changed || clampedRadiator || clampedPlug || clampedBattery || clampedToolbox;
    }

    //! The group that lists the item, null when none does
    RTP_KitRepairGroup Find(Object item)
    {
        if (Radiator && Radiator.Matches(item))
            return Radiator;
        if (SparkPlug && SparkPlug.Matches(item))
            return SparkPlug;
        if (Battery && Battery.Matches(item))
            return Battery;
        if (Toolbox && Toolbox.Matches(item))
            return Toolbox;
        return null;
    }
}

//! Repairs with the toolbox: the engine of a vehicle (action) and detached parts (crafting: toolbox + part)
class RepairToPristineUpdatedToolboxSettings
{
    ref RTP_ToolboxRepairGroup Engine;
    ref RTP_ToolboxRepairGroup Radiator;
    ref RTP_ToolboxRepairGroup SparkPlug;
    ref RTP_ToolboxRepairGroup Battery;
    ref RTP_ToolboxRepairGroup BodyParts;

    static RTP_ToolboxRepairGroup DefaultGroup(string name)
    {
        RTP_ToolboxRepairGroup g = new RTP_ToolboxRepairGroup();
        g.ToolboxWear = 0;
        switch (name)
        {
            case "Engine":
                g.SetRules(true, 1.0);
                g.RepairTime = 10;
                g.Items = {"CarScript", "MotorbikeScript", "BoatScript"};
                g.AddTool("Hammer", 3);
                g.AddTool("Wrench", 4);
                g.AddTool("RTP_Ratchet", 4);
                g.AddTool("Pliers", 3);
                g.AddTool("Screwdriver", 3);
                break;
            case "Radiator":
                g.SetRules(true, 0.8);
                g.RepairTime = 6;
                g.Items = {"CarRadiator", "ExpansionHydraulicHoses"};
                g.AddTool("Pliers", 3);
                g.AddTool("DuctTape", 15);
                break;
            case "SparkPlug":
                g.SetRules(true, 0.8);
                g.RepairTime = 5;
                g.Items = {"SparkPlug", "GlowPlug"};
                g.AddTool("Pliers", 2);
                g.AddTool("RTP_SteelBrush", 4);
                break;
            case "Battery":
                g.SetRules(true, 0.8);
                g.RepairTime = 6;
                g.Items = {"CarBattery", "TruckBattery", "ExpansionHelicopterBattery", "ExpansionAircraftBattery"};
                g.AddTool("Screwdriver", 3);
                g.AddTool("DuctTape", 15);
                break;
            case "BodyParts":
                g.SetRules(true, 1.0);
                g.RepairTime = 8;
                g.Items = {"CarDoor", "Motorbike_02_ShieldFront_ColorBase"};
                g.AddTool("Hammer", 4);
                g.AddTool("EpoxyPutty", 15);
                break;
        }
        return g;
    }

    void SetDefaults()
    {
        Engine = DefaultGroup("Engine");
        Radiator = DefaultGroup("Radiator");
        SparkPlug = DefaultGroup("SparkPlug");
        Battery = DefaultGroup("Battery");
        BodyParts = DefaultGroup("BodyParts");
    }

    //! Groups missing in the loaded file get the defaults; true when something was added or corrected
    bool Validate()
    {
        bool changed = false;
        if (!Engine || Engine.IsBlank())
        {
            Engine = DefaultGroup("Engine");
            changed = true;
        }
        if (!Radiator || Radiator.IsBlank())
        {
            Radiator = DefaultGroup("Radiator");
            changed = true;
        }
        if (!SparkPlug || SparkPlug.IsBlank())
        {
            SparkPlug = DefaultGroup("SparkPlug");
            changed = true;
        }
        if (!Battery || Battery.IsBlank())
        {
            Battery = DefaultGroup("Battery");
            changed = true;
        }
        if (!BodyParts || BodyParts.IsBlank())
        {
            BodyParts = DefaultGroup("BodyParts");
            changed = true;
        }
        bool c1 = Engine.Clamp();
        bool c2 = Radiator.Clamp();
        bool c3 = SparkPlug.Clamp();
        bool c4 = Battery.Clamp();
        bool c5 = BodyParts.Clamp();
        return changed || c1 || c2 || c3 || c4 || c5;
    }

    //! Group of a detached part (crafting repairs), null when no group lists it
    RTP_ToolboxRepairGroup FindPartGroup(Object item)
    {
        if (Radiator && Radiator.Matches(item))
            return Radiator;
        if (SparkPlug && SparkPlug.Matches(item))
            return SparkPlug;
        if (Battery && Battery.Matches(item))
            return Battery;
        if (BodyParts && BodyParts.Matches(item))
            return BodyParts;
        return null;
    }
}

class RepairToPristineUpdatedSettings
{
    int ConfigVersion;
    float RepairKitModifier;
    float PristineRepairKitModifier;
    bool EnablePristineRepair;
    bool EnableRuinedRepair;
    float RuinedRepairKitModifier;
    bool DebugMode;
    ref RepairToPristineUpdatedVehicleSettings VehicleParts;
    ref RepairToPristineUpdatedVehicleSettings VehicleChassis;
    ref RepairToPristineUpdatedVehicleSettings VehicleEngine;
    ref RepairToPristineUpdatedPartKitSettings PartKits;     // created by the config loader
    ref RepairToPristineUpdatedToolboxSettings ToolboxRepairs;   // created by the config loader

    void RepairToPristineUpdatedSettings()
    {
        ConfigVersion = 4;
        RepairKitModifier = 0.8;
        PristineRepairKitModifier = 1.2;
        EnablePristineRepair = true;
        EnableRuinedRepair = false;
        RuinedRepairKitModifier = 2.5;
        DebugMode = false;
        VehicleParts = new RepairToPristineUpdatedVehicleSettings();
        VehicleChassis = new RepairToPristineUpdatedVehicleSettings();
        VehicleEngine = new RepairToPristineUpdatedVehicleSettings();
    }
}
