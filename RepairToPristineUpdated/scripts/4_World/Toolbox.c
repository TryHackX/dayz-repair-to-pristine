// Toolbox: tools sit in their own slots or anywhere in the cargo (other mods can redefine the slots of vanilla tools,
// so the cargo always counts). Which tools a repair needs is set per repair group in the server config.
class RTP_Toolbox extends Container_Base
{
    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionRTPRepairEngineWithToolbox);
    }

    //! A usable tool of this type (or a subclass) from a slot or the cargo, null when there is none
    ItemBase FindTool(string type)
    {
        GameInventory inventory = GetInventory();
        for (int i = 0; i < inventory.AttachmentCount(); i++)
        {
            ItemBase attachment = ItemBase.Cast(inventory.GetAttachmentFromIndex(i));
            if (IsUsableTool(attachment, type))
                return attachment;
        }

        CargoBase cargo = inventory.GetCargo();
        if (cargo)
        {
            for (int j = 0; j < cargo.GetItemCount(); j++)
            {
                ItemBase item = ItemBase.Cast(cargo.GetItem(j));
                if (IsUsableTool(item, type))
                    return item;
            }
        }

        return null;
    }

    protected bool IsUsableTool(ItemBase item, string type)
    {
        if (!item || item.IsSetForDeletion() || item.IsRuined() || !item.IsKindOf(type))
            return false;

        return !item.HasQuantity() || item.GetQuantity() > 0;
    }

    bool HasTools(RTP_ToolboxRepairGroup group)
    {
        if (!group || !group.Tools)
            return false;

        foreach (RTP_ToolSetting tool : group.Tools)
        {
            if (!FindTool(tool.ClassName))
                return false;
        }

        return true;
    }

    //! Server: every tool loses its Wear % of health (items with quantity: of their quantity), the toolbox loses
    //! ToolboxWear % of its own health; both multiplied by the modifier of the repaired health level
    void UseTools(RTP_ToolboxRepairGroup group, float modifier)
    {
        foreach (RTP_ToolSetting setting : group.Tools)
        {
            ItemBase tool = FindTool(setting.ClassName);
            if (!tool || setting.Wear <= 0)
                continue;

            float wear = setting.Wear / 100 * modifier;
            if (tool.HasQuantity())
                tool.AddQuantity(-tool.GetQuantityMax() * wear);
            else
                tool.AddHealth("", "", -tool.GetMaxHealth("", "") * wear);
        }

        if (group.ToolboxWear > 0)
            AddHealth("", "", -GetMaxHealth("", "") * group.ToolboxWear / 100 * modifier);
    }
}

// Hand poses: the toolbox is carried by its top handle like the vanilla ammo box, the ratchet and the steel brush
// are held like the screwdriver (the models are built around the same grip points)
modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);

        pType.AddItemInHandsProfileIK("RTP_Toolbox", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/AmmoBox.anm");
        pType.AddItemInHandsProfileIK("RTP_Ratchet", "dz/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", pBehavior, "dz/anims/anm/player/ik/gear/screwdriver.anm");
        pType.AddItemInHandsProfileIK("RTP_SteelBrush", "dz/anims/workspaces/player/player_main/weapons/player_main_1h_knife.asi", pBehavior, "dz/anims/anm/player/ik/gear/screwdriver.anm");
    }
}

class RTP_ToolboxRepairs
{
    //! Server: one health level up (worn -> pristine, ruined -> badly damaged), same values as the vehicle repairs
    static void RaiseOneLevel(EntityAI target, string zone)
    {
        int level = target.GetHealthLevel(zone);
        bool isVehicle = Transport.Cast(target) != null;
        if (!isVehicle)
            target.SetAllowDamage(true);

        RTP_VehicleRepair.SetNextLevelHealth(target, zone, level);

        if (!isVehicle)
            target.ProcessInvulnerabilityCheck(target.GetInvulnerabilityTypeString());
        RTP_VehicleRepair.LogRepair(target, zone, level);
    }
}
