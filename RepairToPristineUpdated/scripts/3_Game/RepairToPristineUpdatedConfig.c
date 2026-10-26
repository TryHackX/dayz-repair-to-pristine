class RepairToPristineUpdatedConfig
{
    private static ref RepairToPristineUpdatedSettings m_Settings;
    private static const string CONFIG_ROOT = "$profile:RepairToPristineUpdated/";
    private static const string CONFIG_FILE = "RepairToPristineConfig.json";
    static const int RPC_SYNC_CONFIG = 4523891; 
    
    static void Init()
    {
        if (!m_Settings)
        {
            m_Settings = new RepairToPristineUpdatedSettings();
            if (GetGame().IsServer() || !GetGame().IsMultiplayer())
            {
                LoadConfig();
            }
            else
            {
                Print("[RepairToPristineUpdated] Client initialized - waiting for server config sync");
            }
        }
    }

    static void SyncConfig(PlayerIdentity identity)
    {
        if (!m_Settings) Init();
        
        Print("[RepairToPristineUpdated] Syncing config to client: " + identity.GetName());
        
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(m_Settings.ConfigVersion);
        rpc.Write(m_Settings.RepairKitModifier);
        rpc.Write(m_Settings.PristineRepairKitModifier);
        rpc.Write(m_Settings.EnablePristineRepair);
        rpc.Write(m_Settings.EnableRuinedRepair);
        rpc.Write(m_Settings.RuinedRepairKitModifier);
        rpc.Write(m_Settings.DebugMode);
        WriteVehicleSettings(rpc, m_Settings.VehicleParts);
        WriteVehicleSettings(rpc, m_Settings.VehicleChassis);
        WriteVehicleSettings(rpc, m_Settings.VehicleEngine);
        WritePartKitSettings(rpc, GetPartKitSettings());
        WriteToolboxSettings(rpc, GetToolboxSettings());

        rpc.Send(null, RPC_SYNC_CONFIG, true, identity);
    }

    private static void WriteVehicleSettings(ScriptRPC rpc, RepairToPristineUpdatedVehicleSettings s)
    {
        rpc.Write(s.EnablePristineRepair);
        rpc.Write(s.EnableRuinedRepair);
        rpc.Write(s.RepairKitModifier);
        rpc.Write(s.PristineRepairKitModifier);
        rpc.Write(s.RuinedRepairKitModifier);
    }

    private static void ReadVehicleSettings(ParamsReadContext ctx, RepairToPristineUpdatedVehicleSettings s)
    {
        ctx.Read(s.EnablePristineRepair);
        ctx.Read(s.EnableRuinedRepair);
        ctx.Read(s.RepairKitModifier);
        ctx.Read(s.PristineRepairKitModifier);
        ctx.Read(s.RuinedRepairKitModifier);
    }

    private static void WriteGroupRules(ScriptRPC rpc, RTP_RepairGroup g)
    {
        rpc.Write(g.Enabled);
        rpc.Write(g.EnablePristineRepair);
        rpc.Write(g.EnableRuinedRepair);
        rpc.Write(g.RepairModifier);
        rpc.Write(g.PristineRepairModifier);
        rpc.Write(g.RuinedRepairModifier);
        rpc.Write(g.Items);
    }

    private static void ReadGroupRules(ParamsReadContext ctx, RTP_RepairGroup g)
    {
        ctx.Read(g.Enabled);
        ctx.Read(g.EnablePristineRepair);
        ctx.Read(g.EnableRuinedRepair);
        ctx.Read(g.RepairModifier);
        ctx.Read(g.PristineRepairModifier);
        ctx.Read(g.RuinedRepairModifier);
        g.Items = new TStringArray();
        ctx.Read(g.Items);
    }

    private static void WriteKitGroup(ScriptRPC rpc, RTP_KitRepairGroup g)
    {
        WriteGroupRules(rpc, g);
        rpc.Write(g.Kits.Count());
        foreach (RTP_KitSetting kit : g.Kits)
        {
            rpc.Write(kit.ClassName);
            rpc.Write(kit.Cost);
        }
    }

    private static RTP_KitRepairGroup ReadKitGroup(ParamsReadContext ctx)
    {
        RTP_KitRepairGroup g = new RTP_KitRepairGroup();
        ReadGroupRules(ctx, g);
        g.Kits = new array<ref RTP_KitSetting>();
        int count;
        ctx.Read(count);
        for (int i = 0; i < count; i++)
        {
            string className;
            float cost;
            ctx.Read(className);
            ctx.Read(cost);
            g.AddKit(className, cost);
        }
        return g;
    }

    private static void WriteToolboxGroup(ScriptRPC rpc, RTP_ToolboxRepairGroup g)
    {
        WriteGroupRules(rpc, g);
        rpc.Write(g.RepairTime);
        rpc.Write(g.ToolboxWear);
        rpc.Write(g.Tools.Count());
        foreach (RTP_ToolSetting tool : g.Tools)
        {
            rpc.Write(tool.ClassName);
            rpc.Write(tool.Wear);
        }
    }

    private static RTP_ToolboxRepairGroup ReadToolboxGroup(ParamsReadContext ctx)
    {
        RTP_ToolboxRepairGroup g = new RTP_ToolboxRepairGroup();
        ReadGroupRules(ctx, g);
        ctx.Read(g.RepairTime);
        ctx.Read(g.ToolboxWear);
        g.Tools = new array<ref RTP_ToolSetting>();
        int count;
        ctx.Read(count);
        for (int i = 0; i < count; i++)
        {
            string className;
            float wear;
            ctx.Read(className);
            ctx.Read(wear);
            g.AddTool(className, wear);
        }
        return g;
    }

    private static void WritePartKitSettings(ScriptRPC rpc, RepairToPristineUpdatedPartKitSettings s)
    {
        WriteKitGroup(rpc, s.Radiator);
        WriteKitGroup(rpc, s.SparkPlug);
        WriteKitGroup(rpc, s.Battery);
        WriteKitGroup(rpc, s.Toolbox);
    }

    private static void ReadPartKitSettings(ParamsReadContext ctx, RepairToPristineUpdatedPartKitSettings s)
    {
        s.Radiator = ReadKitGroup(ctx);
        s.SparkPlug = ReadKitGroup(ctx);
        s.Battery = ReadKitGroup(ctx);
        s.Toolbox = ReadKitGroup(ctx);
    }

    private static void WriteToolboxSettings(ScriptRPC rpc, RepairToPristineUpdatedToolboxSettings s)
    {
        WriteToolboxGroup(rpc, s.Engine);
        WriteToolboxGroup(rpc, s.Radiator);
        WriteToolboxGroup(rpc, s.SparkPlug);
        WriteToolboxGroup(rpc, s.Battery);
        WriteToolboxGroup(rpc, s.BodyParts);
    }

    private static void ReadToolboxSettings(ParamsReadContext ctx, RepairToPristineUpdatedToolboxSettings s)
    {
        s.Engine = ReadToolboxGroup(ctx);
        s.Radiator = ReadToolboxGroup(ctx);
        s.SparkPlug = ReadToolboxGroup(ctx);
        s.Battery = ReadToolboxGroup(ctx);
        s.BodyParts = ReadToolboxGroup(ctx);
    }

    private static string Names(TStringArray list)
    {
        string res = "";
        foreach (string name : list)
        {
            if (res != "")
                res += ", ";
            res += name;
        }
        return "[" + res + "]";
    }

    private static string GroupRules(RTP_RepairGroup g)
    {
        string rules = "enabled " + g.Enabled + ", pristine " + g.EnablePristineRepair + ", ruined " + g.EnableRuinedRepair;
        return rules + ", x" + g.RepairModifier + " / x" + g.PristineRepairModifier + " / x" + g.RuinedRepairModifier + ", items " + Names(g.Items);
    }

    private static void PrintKitGroup(string name, RTP_KitRepairGroup g)
    {
        string kits = "";
        foreach (RTP_KitSetting kit : g.Kits)
            kits += " " + kit.ClassName + " " + kit.Cost + "%";
        Print("[RepairToPristineUpdated] " + name + ": " + GroupRules(g) + ", kits" + kits);
    }

    private static void PrintToolboxGroup(string name, RTP_ToolboxRepairGroup g)
    {
        string tools = "";
        foreach (RTP_ToolSetting tool : g.Tools)
            tools += " " + tool.ClassName + " " + tool.Wear + "%";
        Print("[RepairToPristineUpdated] " + name + ": " + GroupRules(g) + ", time " + g.RepairTime + " s, toolbox wear " + g.ToolboxWear + "%, tools" + tools);
    }

    private static void PrintToolboxSettings()
    {
        RepairToPristineUpdatedToolboxSettings s = GetToolboxSettings();
        PrintToolboxGroup("ToolboxRepairs.Engine", s.Engine);
        PrintToolboxGroup("ToolboxRepairs.Radiator", s.Radiator);
        PrintToolboxGroup("ToolboxRepairs.SparkPlug", s.SparkPlug);
        PrintToolboxGroup("ToolboxRepairs.Battery", s.Battery);
        PrintToolboxGroup("ToolboxRepairs.BodyParts", s.BodyParts);
    }

    private static void PrintVehicleSettings(string name, RepairToPristineUpdatedVehicleSettings s)
    {
        string flags = name + ": pristine " + s.EnablePristineRepair + ", ruined " + s.EnableRuinedRepair;
        string costs = ", kit x" + s.RepairKitModifier + " / pristine x" + s.PristineRepairKitModifier + " / ruined x" + s.RuinedRepairKitModifier;
        Print("[RepairToPristineUpdated] " + flags + costs);
    }

    //! Negative modifiers would add quantity to repair kits; returns true when something was clamped
    private static bool ClampVehicleSettings(RepairToPristineUpdatedVehicleSettings s)
    {
        if (s.RepairKitModifier >= 0 && s.PristineRepairKitModifier >= 0 && s.RuinedRepairKitModifier >= 0)
            return false;

        s.RepairKitModifier = Math.Max(s.RepairKitModifier, 0);
        s.PristineRepairKitModifier = Math.Max(s.PristineRepairKitModifier, 0);
        s.RuinedRepairKitModifier = Math.Max(s.RuinedRepairKitModifier, 0);
        return true;
    }
    
    static void OnRPC(ParamsReadContext ctx)
    {
        if (!m_Settings) m_Settings = new RepairToPristineUpdatedSettings();
        
        ctx.Read(m_Settings.ConfigVersion);
        ctx.Read(m_Settings.RepairKitModifier);
        ctx.Read(m_Settings.PristineRepairKitModifier);
        ctx.Read(m_Settings.EnablePristineRepair);
        ctx.Read(m_Settings.EnableRuinedRepair);
        ctx.Read(m_Settings.RuinedRepairKitModifier);
        ctx.Read(m_Settings.DebugMode);
        ReadVehicleSettings(ctx, m_Settings.VehicleParts);
        ReadVehicleSettings(ctx, m_Settings.VehicleChassis);
        ReadVehicleSettings(ctx, m_Settings.VehicleEngine);
        m_Settings.PartKits = new RepairToPristineUpdatedPartKitSettings();
        ReadPartKitSettings(ctx, m_Settings.PartKits);
        m_Settings.ToolboxRepairs = new RepairToPristineUpdatedToolboxSettings();
        ReadToolboxSettings(ctx, m_Settings.ToolboxRepairs);

        Print("[RepairToPristineUpdated] ===== CLIENT CONFIG RECEIVED =====");
        Print("[RepairToPristineUpdated] ConfigVersion: " + m_Settings.ConfigVersion);
        Print("[RepairToPristineUpdated] RepairKitModifier: " + m_Settings.RepairKitModifier);
        Print("[RepairToPristineUpdated] PristineRepairKitModifier: " + m_Settings.PristineRepairKitModifier);
        Print("[RepairToPristineUpdated] EnablePristineRepair: " + m_Settings.EnablePristineRepair);
        Print("[RepairToPristineUpdated] EnableRuinedRepair: " + m_Settings.EnableRuinedRepair);
        Print("[RepairToPristineUpdated] RuinedRepairKitModifier: " + m_Settings.RuinedRepairKitModifier);
        Print("[RepairToPristineUpdated] DebugMode: " + m_Settings.DebugMode);
        PrintPartSettings();
        PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
        PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
        PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
        PrintToolboxSettings();
        Print("[RepairToPristineUpdated] ==================================");
    }

    private static void PrintPartSettings()
    {
        RepairToPristineUpdatedPartKitSettings s = GetPartKitSettings();
        PrintKitGroup("PartKits.Radiator", s.Radiator);
        PrintKitGroup("PartKits.SparkPlug", s.SparkPlug);
        PrintKitGroup("PartKits.Battery", s.Battery);
        PrintKitGroup("PartKits.Toolbox", s.Toolbox);
    }

    static void LoadConfig()
    {
        if (!FileExist(CONFIG_ROOT))
        {
            MakeDirectory(CONFIG_ROOT);
            Print("[RepairToPristineUpdated] Created config directory: " + CONFIG_ROOT);
        }
        
        string configPath = CONFIG_ROOT + CONFIG_FILE;
        
        if (FileExist(configPath))
        {
            Print("[RepairToPristineUpdated] Loading config from: " + configPath);
            
            RepairToPristineUpdatedSettings loadedSettings = new RepairToPristineUpdatedSettings();
            
            JsonFileLoader<RepairToPristineUpdatedSettings>.JsonLoadFile(configPath, loadedSettings);
            
            bool needsUpdate = false;
            int oldVersion = loadedSettings.ConfigVersion;
            
            if (oldVersion < 2)
            {
                Print("[RepairToPristineUpdated] Migrating config from version " + oldVersion + " to version 3");
                needsUpdate = true;

                // fields added in version 2 are missing (0) in older files; in current files 0 is a valid value (free repairs)
                if (loadedSettings.RepairKitModifier == 0)
                {
                    loadedSettings.RepairKitModifier = 0.75;
                    Print("[RepairToPristineUpdated] Migration: Added RepairKitModifier = 0.75");
                }

                if (loadedSettings.PristineRepairKitModifier == 0)
                {
                    loadedSettings.PristineRepairKitModifier = 1.0;
                    Print("[RepairToPristineUpdated] Migration: Added PristineRepairKitModifier = 1.0");
                }

                if (loadedSettings.RuinedRepairKitModifier == 0)
                {
                    loadedSettings.RuinedRepairKitModifier = 2.0;
                    Print("[RepairToPristineUpdated] Migration: Added RuinedRepairKitModifier = 2.0");
                }
            }

            // negative modifiers would add quantity to repair kits
            if (loadedSettings.RepairKitModifier < 0 || loadedSettings.PristineRepairKitModifier < 0 || loadedSettings.RuinedRepairKitModifier < 0)
            {
                loadedSettings.RepairKitModifier = Math.Max(loadedSettings.RepairKitModifier, 0);
                loadedSettings.PristineRepairKitModifier = Math.Max(loadedSettings.PristineRepairKitModifier, 0);
                loadedSettings.RuinedRepairKitModifier = Math.Max(loadedSettings.RuinedRepairKitModifier, 0);
                needsUpdate = true;
                Print("[RepairToPristineUpdated] Negative modifiers are not allowed, clamped to 0");
            }

            // version 3: separate settings for vehicle parts, chassis and engine
            if (oldVersion < 3)
            {
                Print("[RepairToPristineUpdated] Migration: added VehicleParts, VehicleChassis and VehicleEngine settings");
                needsUpdate = true;
            }

            // version 4: kit repairs for radiators / plugs / batteries / the toolbox (PartKits) and the toolbox repairs
            if (oldVersion < 4)
            {
                Print("[RepairToPristineUpdated] Migration: added PartKits and ToolboxRepairs settings");
                needsUpdate = true;
            }

            if (!loadedSettings.VehicleParts)
                loadedSettings.VehicleParts = new RepairToPristineUpdatedVehicleSettings();
            if (!loadedSettings.VehicleChassis)
                loadedSettings.VehicleChassis = new RepairToPristineUpdatedVehicleSettings();
            if (!loadedSettings.VehicleEngine)
                loadedSettings.VehicleEngine = new RepairToPristineUpdatedVehicleSettings();

            // the json loader leaves missing lists empty (an empty list is a valid setting), so a section that is
            // not in the file at all gets the defaults
            string text = ReadText(configPath);
            bool newBatteryGroup = false;
            if (!loadedSettings.PartKits || oldVersion < 4 || !text.Contains("\"PartKits\""))
            {
                loadedSettings.PartKits = new RepairToPristineUpdatedPartKitSettings();
                loadedSettings.PartKits.SetDefaults();
                newBatteryGroup = true;
                needsUpdate = true;
            }
            else if (!loadedSettings.PartKits.Battery || loadedSettings.PartKits.Battery.IsBlank())
            {
                newBatteryGroup = true;
            }
            if (!loadedSettings.ToolboxRepairs || oldVersion < 4 || !text.Contains("\"ToolboxRepairs\""))
            {
                loadedSettings.ToolboxRepairs = new RepairToPristineUpdatedToolboxSettings();
                loadedSettings.ToolboxRepairs.SetDefaults();
                needsUpdate = true;
            }
            bool fixedKits = loadedSettings.PartKits.Validate();
            bool fixedToolbox = loadedSettings.ToolboxRepairs.Validate();
            if (newBatteryGroup)
            {
                // batteries followed the item settings before they got their own group - keep this server's values
                RTP_KitRepairGroup battery = loadedSettings.PartKits.Battery;
                battery.EnablePristineRepair = loadedSettings.EnablePristineRepair;
                battery.EnableRuinedRepair = loadedSettings.EnableRuinedRepair;
                battery.RepairModifier = loadedSettings.RepairKitModifier;
                battery.PristineRepairModifier = loadedSettings.PristineRepairKitModifier;
                battery.RuinedRepairModifier = loadedSettings.RuinedRepairKitModifier;
                needsUpdate = true;
                Print("[RepairToPristineUpdated] Migration: PartKits.Battery added with the item settings of this config");
            }
            if (fixedKits || fixedToolbox)
            {
                needsUpdate = true;
                Print("[RepairToPristineUpdated] PartKits / ToolboxRepairs: missing groups added, values out of range corrected");
            }

            bool clampedParts = ClampVehicleSettings(loadedSettings.VehicleParts);
            bool clampedChassis = ClampVehicleSettings(loadedSettings.VehicleChassis);
            bool clampedEngine = ClampVehicleSettings(loadedSettings.VehicleEngine);
            if (clampedParts || clampedChassis || clampedEngine)
            {
                needsUpdate = true;
                Print("[RepairToPristineUpdated] Negative vehicle modifiers are not allowed, clamped to 0");
            }

            loadedSettings.ConfigVersion = 4;
            m_Settings = loadedSettings;
            
            if (needsUpdate)
            {
                Print("[RepairToPristineUpdated] Config updated with new fields, saving...");
                SaveConfig();
                Print("[RepairToPristineUpdated] Config migration completed successfully!");
            }
            
            Print("[RepairToPristineUpdated] ===== SERVER CONFIG LOADED =====");
            Print("[RepairToPristineUpdated] ConfigVersion: " + m_Settings.ConfigVersion);
            Print("[RepairToPristineUpdated] RepairKitModifier: " + m_Settings.RepairKitModifier);
            Print("[RepairToPristineUpdated] PristineRepairKitModifier: " + m_Settings.PristineRepairKitModifier);
            Print("[RepairToPristineUpdated] EnablePristineRepair: " + m_Settings.EnablePristineRepair);
            Print("[RepairToPristineUpdated] EnableRuinedRepair: " + m_Settings.EnableRuinedRepair);
            Print("[RepairToPristineUpdated] RuinedRepairKitModifier: " + m_Settings.RuinedRepairKitModifier);
            Print("[RepairToPristineUpdated] DebugMode: " + m_Settings.DebugMode);
            PrintPartSettings();
            PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
            PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
            PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
            PrintToolboxSettings();
            Print("[RepairToPristineUpdated] ==================================");
        }
        else
        {
            Print("[RepairToPristineUpdated] Config file not found, creating default config");
            GetPartKitSettings();
            GetToolboxSettings();
            SaveConfig();
            Print("[RepairToPristineUpdated] Created default config at: " + configPath);
            Print("[RepairToPristineUpdated] ===== DEFAULT CONFIG =====");
            Print("[RepairToPristineUpdated] RepairKitModifier: " + m_Settings.RepairKitModifier);
            Print("[RepairToPristineUpdated] PristineRepairKitModifier: " + m_Settings.PristineRepairKitModifier);
            Print("[RepairToPristineUpdated] EnablePristineRepair: " + m_Settings.EnablePristineRepair);
            Print("[RepairToPristineUpdated] EnableRuinedRepair: " + m_Settings.EnableRuinedRepair);
            Print("[RepairToPristineUpdated] RuinedRepairKitModifier: " + m_Settings.RuinedRepairKitModifier);
            PrintPartSettings();
            PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
            PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
            PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
            PrintToolboxSettings();
            Print("[RepairToPristineUpdated] ==============================");
        }
    }
    
    private static string ReadText(string path)
    {
        string text = "";
        string line;
        FileHandle file = OpenFile(path, FileMode.READ);
        if (file)
        {
            while (FGets(file, line) >= 0)
                text += line;
            CloseFile(file);
        }
        return text;
    }

    static void SaveConfig()
    {
        string configPath = CONFIG_ROOT + CONFIG_FILE;
        JsonFileLoader<RepairToPristineUpdatedSettings>.JsonSaveFile(configPath, m_Settings);
        Print("[RepairToPristineUpdated] Config saved to: " + configPath);
    }
    
    static float GetModifier()
    {
        if (!m_Settings) Init();
        return m_Settings.RepairKitModifier;
    }
    
    static float GetPristineModifier()
    {
        if (!m_Settings) Init();
        return m_Settings.PristineRepairKitModifier;
    }
    
    static float GetRuinedModifier()
    {
        if (!m_Settings) Init();
        return m_Settings.RuinedRepairKitModifier;
    }
    
    static bool IsPristineRepairEnabled()
    {
        if (!m_Settings) Init();
        return m_Settings.EnablePristineRepair;
    }
    
    static bool IsRuinedRepairEnabled()
    {
        if (!m_Settings) Init();
        return m_Settings.EnableRuinedRepair;
    }
    
    static bool IsDebugEnabled()
    {
        if (!m_Settings) Init();
        return m_Settings.DebugMode;
    }

    static RepairToPristineUpdatedPartKitSettings GetPartKitSettings()
    {
        if (!m_Settings) Init();
        if (!m_Settings.PartKits)
        {
            m_Settings.PartKits = new RepairToPristineUpdatedPartKitSettings();
            m_Settings.PartKits.SetDefaults();
        }
        return m_Settings.PartKits;
    }

    static RepairToPristineUpdatedToolboxSettings GetToolboxSettings()
    {
        if (!m_Settings) Init();
        if (!m_Settings.ToolboxRepairs)
        {
            m_Settings.ToolboxRepairs = new RepairToPristineUpdatedToolboxSettings();
            m_Settings.ToolboxRepairs.SetDefaults();
        }
        return m_Settings.ToolboxRepairs;
    }

    //! Can something at this health level be repaired? lowestVanillaLevel = the worst-but-one level vanilla accepts
    //! (worn for items that vanilla repairs to pristine, damaged for the rest).
    static bool CanRepairHealthLevel(int level, int lowestVanillaLevel = GameConstants.STATE_DAMAGED)
    {
        if (level <= GameConstants.STATE_PRISTINE)
            return false;

        if (level >= GameConstants.STATE_RUINED)
            return IsRuinedRepairEnabled();

        if (level < lowestVanillaLevel)
            return IsPristineRepairEnabled();

        return true;
    }

    //! Kit usage multiplier for a repair that starts at this health level
    static float GetModifierForHealthLevel(int level)
    {
        if (level >= GameConstants.STATE_RUINED)
            return GetRuinedModifier();

        if (level == GameConstants.STATE_WORN)
            return GetPristineModifier();

        return GetModifier();
    }

    static RepairToPristineUpdatedVehicleSettings GetVehicleSettings(int group)
    {
        if (!m_Settings) Init();

        if (group == RTP_VehicleRepairGroup.PARTS)
            return m_Settings.VehicleParts;

        if (group == RTP_VehicleRepairGroup.CHASSIS)
            return m_Settings.VehicleChassis;

        return m_Settings.VehicleEngine;
    }

    //! Vanilla repairs vehicles from damaged and badly damaged; the group settings add worn (to pristine) and ruined
    static bool CanRepairVehicleLevel(int group, int level)
    {
        if (level <= GameConstants.STATE_PRISTINE)
            return false;

        RepairToPristineUpdatedVehicleSettings s = GetVehicleSettings(group);
        if (level >= GameConstants.STATE_RUINED)
            return s.EnableRuinedRepair;

        if (level == GameConstants.STATE_WORN)
            return s.EnablePristineRepair;

        return true;
    }

    //! Share of the kit's max quantity used by one vehicle repair step (vanilla: 25 %)
    static float GetVehicleKitShare(int group, int level)
    {
        RepairToPristineUpdatedVehicleSettings s = GetVehicleSettings(group);
        float modifier = s.RepairKitModifier;
        if (level >= GameConstants.STATE_RUINED)
            modifier = s.RuinedRepairKitModifier;
        else if (level == GameConstants.STATE_WORN)
            modifier = s.PristineRepairKitModifier;

        return 0.25 * modifier;
    }
}
