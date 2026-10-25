// Vehicles are repaired by actions, not by PluginRepairing. Vanilla repairs vehicle parts and zones only from
// damaged / badly damaged and never above worn. Per group (parts, chassis, engine) the server config adds
// worn -> pristine and ruined -> badly damaged, and scales the kit usage (vanilla: 25 % of the kit per repair).
// Client side: when the vanilla condition refuses, the same checks run again with the extra health levels allowed.
// Server side: vanilla conditions always pass, so the config is enforced when the repair is applied.
class RTP_VehicleRepair
{
    //! Vanilla splits vehicle zones between the engine action ("Engine") and the chassis action (the rest)
    static int GetZoneGroup(string zone)
    {
        if (zone == "Engine")
            return RTP_VehicleRepairGroup.ENGINE;

        return RTP_VehicleRepairGroup.CHASSIS;
    }

    //! Health levels vanilla refuses (worn, ruined) but the config allows
    static bool IsExtraLevelAllowed(int group, int level)
    {
        if (level != GameConstants.STATE_WORN && level != GameConstants.STATE_RUINED)
            return false;

        return RepairToPristineUpdatedConfig.CanRepairVehicleLevel(group, level);
    }

    //! Client: first zone of the targeted component that the config lets us repair beyond vanilla.
    //! onlyZone / skipZone mirror the vanilla zone filters ("" = no filter).
    static string FindExtraZone(EntityAI vehicle, ActionTarget target, string onlyZone, string skipZone, bool viewLod)
    {
        array<string> selections = new array<string>();
        if (viewLod)
            vehicle.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);
        else
            vehicle.GetActionComponentNameList(target.GetComponentIndex(), selections);

        foreach (string selection : selections)
        {
            string zone = "";
            if (!DamageSystem.GetDamageZoneFromComponentName(vehicle, selection, zone))
                continue;

            if ((onlyZone != "" && zone != onlyZone) || (skipZone != "" && zone == skipZone))
                continue;

            if (IsExtraLevelAllowed(GetZoneGroup(zone), vehicle.GetHealthLevel(zone)))
                return zone;
        }

        return "";
    }

    //! Raises the health one level, with the values vanilla uses (worn -> full health)
    static void SetNextLevelHealth(EntityAI target, string zone, int level)
    {
        float maxHealth = target.GetMaxHealth(zone, "");
        float randomValue = Math.RandomFloatInclusive(maxHealth * 0.05, maxHealth * 0.15);

        switch (level)
        {
            case GameConstants.STATE_WORN:
                target.SetHealth01(zone, "", 1);
                break;
            case GameConstants.STATE_DAMAGED:
                target.SetHealth(zone, "", maxHealth * GameConstants.DAMAGE_DAMAGED_VALUE + randomValue);
                break;
            case GameConstants.STATE_BADLY_DAMAGED:
                target.SetHealth(zone, "", maxHealth * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE + randomValue);
                break;
            case GameConstants.STATE_RUINED:
                target.SetHealth(zone, "", maxHealth * GameConstants.DAMAGE_RUINED_VALUE + randomValue);
                break;
        }
    }

    //! Server: one repair step of a part item (car door, hood, trunk, motorbike part)
    static void RepairPart(EntityAI part, ItemBase kit)
    {
        int level = part.GetHealthLevel("");
        if (!RepairToPristineUpdatedConfig.CanRepairVehicleLevel(RTP_VehicleRepairGroup.PARTS, level))
            return;

        SetNextLevelHealth(part, "", level);
        ConsumeKit(kit, RepairToPristineUpdatedConfig.GetVehicleKitShare(RTP_VehicleRepairGroup.PARTS, level));
        LogRepair(part, "", level);
    }

    //! Vanilla: whole kit removed when only 1 unit is left, otherwise share of the max quantity
    static void ConsumeKit(ItemBase kit, float share)
    {
        if (!kit || !kit.HasQuantity() || share <= 0)
            return;

        if (kit.GetQuantity() > 1)
            kit.SetQuantity(kit.GetQuantity() - kit.GetQuantityMax() * share);
        else
            kit.Delete();
    }

    static void LogRepair(EntityAI target, string zone, int levelBefore)
    {
        if (RepairToPristineUpdatedConfig.IsDebugEnabled())
            Print("[RepairToPristineUpdated] Repaired vehicle " + target.GetType() + " zone '" + zone + "' level " + levelBefore + " -> " + target.GetHealthLevel(zone));
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Parts: car doors, hoods, trunks (Epoxy Putty, Blowtorch) and motorbike parts (Blowtorch)

modded class ActionRepairCarPart
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        CarDoor carDoor = CarDoor.Cast(target.GetObject());
        if (!carDoor || !player || player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;

        if (target.GetParent())
        {
            InventoryLocation loc = new InventoryLocation();
            if (!carDoor.GetInventory().GetCurrentInventoryLocation(loc) || loc.GetSlot() == -1)
                return false;

            CarScript car = CarScript.Cast(target.GetParent());
            string slotName = InventorySlots.GetSlotName(loc.GetSlot());
            if (car && slotName && car.GetCarDoorsState(slotName) != CarDoorState.DOORS_OPEN)
                return false;
        }

        return RTP_VehicleRepair.IsExtraLevelAllowed(RTP_VehicleRepairGroup.PARTS, carDoor.GetHealthLevel(""));
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarDoor carDoor = CarDoor.Cast(action_data.m_Target.GetObject());
        if (!carDoor)
        {
            super.OnFinishProgressServer(action_data);
            return;
        }

        RTP_VehicleRepair.RepairPart(carDoor, action_data.m_MainItem);
    }
}

modded class ActionRepairMotorbikePart
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        MotorbikePart motorbikePart = MotorbikePart.Cast(target.GetObject());
        if (!motorbikePart || !player)
            return false;

        if (target.GetParent())
        {
            InventoryLocation loc = new InventoryLocation();
            if (!motorbikePart.GetInventory().GetCurrentInventoryLocation(loc) || loc.GetSlot() == -1)
                return false;
        }

        return RTP_VehicleRepair.IsExtraLevelAllowed(RTP_VehicleRepairGroup.PARTS, motorbikePart.GetHealthLevel(""));
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        MotorbikePart motorbikePart = MotorbikePart.Cast(action_data.m_Target.GetObject());
        if (!motorbikePart)
        {
            super.OnFinishProgressServer(action_data);
            return;
        }

        RTP_VehicleRepair.RepairPart(motorbikePart, action_data.m_MainItem);
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Chassis and engine zones of cars, motorbikes and boats

modded class ActionRepairVehiclePartBase
{
    //! Kit share decided by AdjustVehicleHealthServer for the following AdjustItemQuantityServer:
    //! -1 = not decided (vanilla), 0 = repair refused by the config
    protected float m_RTP_KitShare = -1;

    override protected void AdjustVehicleHealthServer(ActionData action_data)
    {
        m_RTP_KitShare = -1;

        Transport vehicle = Transport.Cast(action_data.m_Target.GetObject());
        string damageZone = RepairVehiclePartActionData.Cast(action_data).m_DamageZone;
        if (!g_Game.IsMultiplayer())
            damageZone = m_CurrentDamageZone;

        if (!vehicle || damageZone == "")
        {
            super.AdjustVehicleHealthServer(action_data);
            return;
        }

        int group = RTP_VehicleRepair.GetZoneGroup(damageZone);
        int level = vehicle.GetHealthLevel(damageZone);
        if (!RepairToPristineUpdatedConfig.CanRepairVehicleLevel(group, level))
        {
            m_RTP_KitShare = 0;
            return;
        }

        m_RTP_KitShare = RepairToPristineUpdatedConfig.GetVehicleKitShare(group, level);

        if (level == GameConstants.STATE_WORN || level == GameConstants.STATE_RUINED)
            RTP_VehicleRepair.SetNextLevelHealth(vehicle, damageZone, level);
        else
            super.AdjustVehicleHealthServer(action_data);

        RTP_VehicleRepair.LogRepair(vehicle, damageZone, level);
    }

    override protected void AdjustItemQuantityServer(ActionData action_data)
    {
        if (m_RTP_KitShare >= 0)
        {
            RTP_VehicleRepair.ConsumeKit(action_data.m_MainItem, m_RTP_KitShare);
            m_RTP_KitShare = -1;
            return;
        }

        super.AdjustItemQuantityServer(action_data);
    }
}

modded class ActionRepairCarChassis
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        CarScript car = CarScript.Cast(target.GetObject());
        if (!car || player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(car, target, "", "Engine", false);
        if (zone == "")
            return false;

        if ((WoodenPlank.Cast(item) || Fabric.Cast(item)) && zone != "BackWood" && zone != "BackTarp")
            return false;

        m_CurrentDamageZone = zone;
        m_LastValidComponentIndex = target.GetComponentIndex();
        return true;
    }

    override void AdjustItemQuantityServer(ActionData action_data)
    {
        // planks and tarps are material (vanilla: one piece per repair), real kits use the configured share
        bool material = WoodenPlank.Cast(action_data.m_MainItem) || Fabric.Cast(action_data.m_MainItem);
        if (m_RTP_KitShare == 0 || (m_RTP_KitShare > 0 && !material))
        {
            RTP_VehicleRepair.ConsumeKit(action_data.m_MainItem, m_RTP_KitShare);
            m_RTP_KitShare = -1;
            return;
        }

        m_RTP_KitShare = -1;
        super.AdjustItemQuantityServer(action_data);
    }
}

modded class ActionRepairCarEngine
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        CarScript car = CarScript.Cast(target.GetObject());
        if (!car || car.EngineIsOn() || player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(car, target, "Engine", "", true);
        if (zone == "")
            return false;

        m_CurrentDamageZone = zone;
        m_LastValidComponentIndex = target.GetComponentIndex();
        return true;
    }
}

modded class ActionRepairMotorbikeChassis
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        Motorbike motorbike = Motorbike.Cast(target.GetObject());
        if (!motorbike)
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(motorbike, target, "", "", false);
        if (zone == "")
            return false;

        m_CurrentDamageZone = zone;
        m_LastValidComponentIndex = target.GetComponentIndex();
        return true;
    }
}

modded class ActionRepairMotorbikeEngine
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        MotorbikeScript motorbike = MotorbikeScript.Cast(target.GetObject());
        if (!motorbike || motorbike.EngineIsOn())
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(motorbike, target, "", "", true);
        if (zone == "")
            return false;

        m_CurrentDamageZone = zone;
        m_LastValidComponentIndex = target.GetComponentIndex();
        return true;
    }
}

modded class ActionRepairBoatChassis
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        BoatScript boat = BoatScript.Cast(target.GetObject());
        if (!boat || player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(boat, target, "", "Engine", false);
        if (zone == "")
            return false;

        m_CurrentDamageZone = zone;
        return true;
    }
}

modded class ActionRepairBoatEngine
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (super.ActionCondition(player, target, item))
            return true;

        if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;

        Blowtorch bt = Blowtorch.Cast(item);
        if (!bt || !bt.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM))
            return false;

        BoatScript boat = BoatScript.Cast(target.GetObject());
        if (!boat || boat.EngineIsOn())
            return false;

        string zone = RTP_VehicleRepair.FindExtraZone(boat, target, "Engine", "", true);
        if (zone == "")
            return false;

        m_CurrentDamageZone = zone;
        return true;
    }
}
