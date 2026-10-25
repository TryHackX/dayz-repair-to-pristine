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

    void RepairToPristineUpdatedSettings()
    {
        ConfigVersion = 3;
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
