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

        Print("[RepairToPristineUpdated] ===== CLIENT CONFIG RECEIVED =====");
        Print("[RepairToPristineUpdated] ConfigVersion: " + m_Settings.ConfigVersion);
        Print("[RepairToPristineUpdated] RepairKitModifier: " + m_Settings.RepairKitModifier);
        Print("[RepairToPristineUpdated] PristineRepairKitModifier: " + m_Settings.PristineRepairKitModifier);
        Print("[RepairToPristineUpdated] EnablePristineRepair: " + m_Settings.EnablePristineRepair);
        Print("[RepairToPristineUpdated] EnableRuinedRepair: " + m_Settings.EnableRuinedRepair);
        Print("[RepairToPristineUpdated] RuinedRepairKitModifier: " + m_Settings.RuinedRepairKitModifier);
        Print("[RepairToPristineUpdated] DebugMode: " + m_Settings.DebugMode);
        PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
        PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
        PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
        Print("[RepairToPristineUpdated] ==================================");
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

            if (!loadedSettings.VehicleParts)
                loadedSettings.VehicleParts = new RepairToPristineUpdatedVehicleSettings();
            if (!loadedSettings.VehicleChassis)
                loadedSettings.VehicleChassis = new RepairToPristineUpdatedVehicleSettings();
            if (!loadedSettings.VehicleEngine)
                loadedSettings.VehicleEngine = new RepairToPristineUpdatedVehicleSettings();

            bool clampedParts = ClampVehicleSettings(loadedSettings.VehicleParts);
            bool clampedChassis = ClampVehicleSettings(loadedSettings.VehicleChassis);
            bool clampedEngine = ClampVehicleSettings(loadedSettings.VehicleEngine);
            if (clampedParts || clampedChassis || clampedEngine)
            {
                needsUpdate = true;
                Print("[RepairToPristineUpdated] Negative vehicle modifiers are not allowed, clamped to 0");
            }

            loadedSettings.ConfigVersion = 3;
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
            PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
            PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
            PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
            Print("[RepairToPristineUpdated] ==================================");
        }
        else
        {
            Print("[RepairToPristineUpdated] Config file not found, creating default config");
            SaveConfig();
            Print("[RepairToPristineUpdated] Created default config at: " + configPath);
            Print("[RepairToPristineUpdated] ===== DEFAULT CONFIG =====");
            Print("[RepairToPristineUpdated] RepairKitModifier: " + m_Settings.RepairKitModifier);
            Print("[RepairToPristineUpdated] PristineRepairKitModifier: " + m_Settings.PristineRepairKitModifier);
            Print("[RepairToPristineUpdated] EnablePristineRepair: " + m_Settings.EnablePristineRepair);
            Print("[RepairToPristineUpdated] EnableRuinedRepair: " + m_Settings.EnableRuinedRepair);
            Print("[RepairToPristineUpdated] RuinedRepairKitModifier: " + m_Settings.RuinedRepairKitModifier);
            PrintVehicleSettings("VehicleParts", m_Settings.VehicleParts);
            PrintVehicleSettings("VehicleChassis", m_Settings.VehicleChassis);
            PrintVehicleSettings("VehicleEngine", m_Settings.VehicleEngine);
            Print("[RepairToPristineUpdated] ==============================");
        }
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
