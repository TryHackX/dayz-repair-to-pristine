modded class PluginRepairing
{
    override bool Repair(PlayerBase player, ItemBase repair_kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true)
    {
        int health_level = item.GetHealthLevel(damage_zone);

        switch (health_level)
        {
            case GameConstants.STATE_PRISTINE:
                break;
            case GameConstants.STATE_RUINED:
                if (!RepairToPristineUpdatedConfig.IsRuinedRepairEnabled())
                    return false;
                CalculateHealth(player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty);
                break;
            case GameConstants.STATE_WORN:
                if (RTP_CanReachPristine(item))
                {
                    CalculateHealth(player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty);
                }
                break;
            default:
                CalculateHealth(player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty);
                break;
        }

        return true;
    }

    override void CalculateHealth(PlayerBase player, ItemBase kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true)
    {
        EntityAI entity;
        Class.CastTo(entity, item);

        if (entity != null)
        {
            entity.SetAllowDamage(true);
        }

        int health_levels_count = item.GetNumberOfHealthLevels(damage_zone);
        int current_level = item.GetHealthLevel(damage_zone);

        int target_level = Math.Clamp(current_level - 1, 0, health_levels_count - 1);
        if (!RTP_CanReachPristine(item))
        {
            target_level = Math.Clamp(target_level, GameConstants.STATE_WORN, health_levels_count - 1);
        }
        float health_coef = item.GetHealthLevelValue(target_level, damage_zone);

        // kit depletion: same rules as vanilla (only real repair kits), cost scaled by the configured modifier
        float kit_qty_before = -1;
        if (kit && kit.ConfigGetInt("repairKitType") && kit.HasQuantity())
        {
            kit_qty_before = kit.GetQuantity();
            float kit_repair_cost = RTP_GetKitRepairCost(kit, item) * RepairToPristineUpdatedConfig.GetModifierForHealthLevel(current_level);
            kit_repair_cost = Math.Clamp(kit_repair_cost, 0, 100);

            if (use_kit_qty)
            {
                if (kit_qty_before > kit_repair_cost)
                    kit.SetQuantity(kit_qty_before - kit_repair_cost);
                else
                    kit.SetQuantity(0);
            }
        }

        if (item.GetHealth01(damage_zone, "Health") < health_coef)
        {
            item.SetHealth01(damage_zone, "Health", health_coef);
        }

        if (RepairToPristineUpdatedConfig.IsDebugEnabled())
        {
            string kit_info = "none";
            if (kit)
                kit_info = kit.GetType() + " qty " + kit_qty_before + " -> " + kit.GetQuantity();
            Print("[RepairToPristineUpdated] Repaired " + item.GetType() + " zone '" + damage_zone + "' level " + current_level + " -> " + item.GetHealthLevel(damage_zone) + ", kit: " + kit_info);
        }

        if (entity != null)
        {
            entity.ProcessInvulnerabilityCheck(entity.GetInvulnerabilityTypeString());
            RTP_RestoreRuinedWheel(entity, player);
        }
    }

    override bool CanRepair(ItemBase repair_kit, Object item, string damage_zone = "")
    {
        if (!repair_kit || !item)
            return false;

        int state = item.GetHealthLevel(damage_zone);

        int lowest_vanilla_level = GameConstants.STATE_DAMAGED;
        if (item.CanBeRepairedToPristine())
            lowest_vanilla_level = GameConstants.STATE_WORN;

        if (!RepairToPristineUpdatedConfig.CanRepairHealthLevel(state, lowest_vanilla_level))
            return false;

        int repair_kit_type = repair_kit.ConfigGetInt("repairKitType");
        if (!repair_kit_type) // not a repair kit (tools, planks) - vanilla bypasses the kit checks too
            return true;

        float cost = RTP_GetKitRepairCost(repair_kit, item) * RepairToPristineUpdatedConfig.GetModifierForHealthLevel(state);
        if (repair_kit.HasQuantity() && repair_kit.GetQuantity() < cost)
            return false;

        array<int> repairable_with_types = new array<int>;
        item.ConfigGetIntArray("repairableWithKits", repairable_with_types);

        foreach (int repairable_with_type : repairable_with_types)
        {
            if (RTP_IsRepairValid(repair_kit_type, repairable_with_type))
                return true;
        }

        return false;
    }

    protected bool RTP_CanReachPristine(Object item)
    {
        return item.CanBeRepairedToPristine() || RepairToPristineUpdatedConfig.IsPristineRepairEnabled();
    }

    protected bool RTP_IsRepairValid(int repair_kit_type, int repairable_with_type)
    {
        return repair_kit_type > 0 && repairable_with_type > 0 && repair_kit_type == repairable_with_type;
    }

    protected float RTP_GetKitRepairCost(ItemBase repair_kit, Object item)
    {
        array<int> allowedRepairKitTypes = new array<int>();
        array<float> repairKitCosts = new array<float>();

        item.ConfigGetIntArray("repairableWithKits", allowedRepairKitTypes);
        item.ConfigGetFloatArray("repairCosts", repairKitCosts);

        int repairKitType = repair_kit.ConfigGetInt("repairKitType");

        foreach (int i, int allowedKitType : allowedRepairKitTypes)
        {
            if (allowedKitType == repairKitType && i < repairKitCosts.Count())
            {
                return repairKitCosts.Get(i);
            }
        }

        return 0;
    }

    // A ruined wheel is a separate class (e.g. HatchbackWheel_Ruined); once repaired, swap it back to the normal wheel.
    protected void RTP_RestoreRuinedWheel(EntityAI entity, PlayerBase player)
    {
        CarWheel wheel;
        if (!Class.CastTo(wheel, entity) || wheel.GetHealthLevel() == GameConstants.STATE_RUINED)
            return;

        string wheelType = wheel.GetType();
        if (wheelType.Length() <= 7 || wheelType.Substring(wheelType.Length() - 7, 7) != "_Ruined")
            return;

        string newWheelType;
        CarWheel_Ruined ruinedWheel;
        if (Class.CastTo(ruinedWheel, wheel))
            newWheelType = ruinedWheel.GetReplacement();
        if (newWheelType == "")
            newWheelType = wheelType.Substring(0, wheelType.Length() - 7);

        if (!GetGame().ConfigIsExisting("CfgVehicles " + newWheelType))
            return;

        ReplaceWheelLambda lambda = new ReplaceWheelLambda(wheel, newWheelType, player);
        lambda.SetTransferParams(true, true, true);
        wheel.GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
    }
}
