// Engine repair with the toolbox in hands for every vehicle listed in ToolboxRepairs.Engine (cars, motorbikes, boats,
// also Expansion vehicles built on CarScript). The vanilla engine actions differ per vehicle type (the boat one needs a
// blowtorch), so this action finds the engine zone itself. One health level per repair step.
class ActionRTPRepairEngineWithToolboxCB : ActionRepairVehiclePartCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousRepeat(RepairToPristineUpdatedConfig.GetToolboxSettings().Engine.RepairTime);
    }
}

class ActionRTPRepairEngineWithToolbox : ActionRepairVehiclePartBase
{
    protected float m_RTP_ToolModifier = -1;   // set by AdjustVehicleHealthServer, -1 = nothing was repaired

    void ActionRTPRepairEngineWithToolbox()
    {
        m_CallbackClass = ActionRTPRepairEngineWithToolboxCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_FullBody = true;
        m_LockTargetOnUse = false;
        m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
        m_Text = "#STR_RTP_RepairEngineToolbox";
    }

    override bool CanBeUsedWithBrokenLegs()
    {
        return false;
    }

    static bool IsEngineOff(Object vehicle)
    {
        Car car = Car.Cast(vehicle);
        if (car)
            return !car.EngineIsOn();

        Motorbike motorbike = Motorbike.Cast(vehicle);
        if (motorbike)
            return !motorbike.EngineIsOn();

        Boat boat = Boat.Cast(vehicle);
        if (boat)
            return !boat.EngineIsOn();

        return false;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        RTP_Toolbox toolbox = RTP_Toolbox.Cast(item);
        EntityAI vehicle = EntityAI.Cast(target.GetObject());
        RTP_ToolboxRepairGroup engine = RepairToPristineUpdatedConfig.GetToolboxSettings().Engine;
        if (!toolbox || !vehicle || !engine.Enabled || !engine.Matches(vehicle) || !IsEngineOff(vehicle))
            return false;

        if (!toolbox.HasTools(engine))
            return false;

        // the zone comes from the client, the server checks the health rules when the repair is applied
        if (g_Game.IsDedicatedServer())
            return true;

        array<string> selections = new array<string>();
        vehicle.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);
        foreach (string selection : selections)
        {
            string zone = "";
            if (!DamageSystem.GetDamageZoneFromComponentName(vehicle, selection, zone) || zone != "Engine")
                continue;

            if (engine.CanRepairLevel(vehicle.GetHealthLevel(zone)))
            {
                m_CurrentDamageZone = zone;
                return true;
            }
        }

        return false;
    }

    override protected void AdjustVehicleHealthServer(ActionData action_data)
    {
        m_RTP_ToolModifier = -1;

        EntityAI vehicle = EntityAI.Cast(action_data.m_Target.GetObject());
        string zone = RepairVehiclePartActionData.Cast(action_data).m_DamageZone;
        if (!g_Game.IsMultiplayer())
            zone = m_CurrentDamageZone;

        RTP_ToolboxRepairGroup engine = RepairToPristineUpdatedConfig.GetToolboxSettings().Engine;
        if (!vehicle || zone != "Engine" || !engine.Matches(vehicle))
            return;

        int level = vehicle.GetHealthLevel(zone);
        if (!engine.CanRepairLevel(level))
            return;

        m_RTP_ToolModifier = engine.GetModifier(level);
        RTP_ToolboxRepairs.RaiseOneLevel(vehicle, zone);
    }

    override protected void AdjustItemQuantityServer(ActionData action_data)
    {
        RTP_Toolbox toolbox = RTP_Toolbox.Cast(action_data.m_MainItem);
        if (toolbox && m_RTP_ToolModifier >= 0)
            toolbox.UseTools(RepairToPristineUpdatedConfig.GetToolboxSettings().Engine, m_RTP_ToolModifier);

        m_RTP_ToolModifier = -1;
    }
}

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);

        actions.Insert(ActionRTPRepairEngineWithToolbox);
    }
}

// Crafting: toolbox + detached part listed in a ToolboxRepairs group (radiator, plugs, battery, body parts ...).
// One health level per repair. No super.CanDo(): vanilla refuses ingredients with attachments, but the toolbox keeps
// its tools in slots.
class RTP_RepairWithToolbox extends RecipeBase
{
    override void Init()
    {
        m_Name = "#STR_RTP_RepairWithToolbox";
        m_IsInstaRecipe = false;
        m_AnimationLength = 1.5;
        m_Specialty = 0.02;

        m_MinDamageIngredient[0] = -1;
        m_MaxDamageIngredient[0] = 3;
        m_MinQuantityIngredient[0] = -1;
        m_MaxQuantityIngredient[0] = -1;
        m_MinDamageIngredient[1] = 1;   // worn and worse, the repair group decides
        m_MaxDamageIngredient[1] = -1;
        m_MinQuantityIngredient[1] = -1;
        m_MaxQuantityIngredient[1] = -1;

        InsertIngredient(0, "RTP_Toolbox");
        m_IngredientAddHealth[0] = 0;
        m_IngredientSetHealth[0] = -1;
        m_IngredientAddQuantity[0] = 0;
        m_IngredientDestroy[0] = false;
        m_IngredientUseSoftSkills[0] = false;

        InsertIngredient(1, "Inventory_Base");  // the item lists of the repair groups decide
        m_IngredientAddHealth[1] = 0;
        m_IngredientSetHealth[1] = -1;
        m_IngredientAddQuantity[1] = 0;
        m_IngredientDestroy[1] = false;
        m_IngredientUseSoftSkills[1] = false;
    }

    override bool CanDo(ItemBase ingredients[], PlayerBase player)
    {
        RTP_Toolbox toolbox = RTP_Toolbox.Cast(ingredients[0]);
        ItemBase part = ingredients[1];
        if (!toolbox || !part || (part.GetInventory() && part.GetInventory().AttachmentCount() > 0))
            return false;

        RTP_ToolboxRepairGroup group = RepairToPristineUpdatedConfig.GetToolboxSettings().FindPartGroup(part);
        return group && group.CanRepairLevel(part.GetHealthLevel("")) && toolbox.HasTools(group);
    }

    override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
    {
        RTP_Toolbox toolbox = RTP_Toolbox.Cast(ingredients[0]);
        ItemBase part = ingredients[1];
        if (!toolbox || !part)
            return;

        RTP_ToolboxRepairGroup group = RepairToPristineUpdatedConfig.GetToolboxSettings().FindPartGroup(part);
        int level = part.GetHealthLevel("");
        if (!group || !group.CanRepairLevel(level))
            return;

        RTP_ToolboxRepairs.RaiseOneLevel(part, "");
        toolbox.UseTools(group, group.GetModifier(level));
    }
}

modded class PluginRecipesManagerBase
{
    override void RegisterRecipies()
    {
        super.RegisterRecipies();

        RegisterRecipe(new RTP_RepairWithToolbox);
    }
}

// The crafting time of a toolbox repair is the RepairTime of the part's repair group
modded class CAContinuousCraft
{
    override void Setup(ActionData action_data)
    {
        super.Setup(action_data);

        WorldCraftActionData craft = WorldCraftActionData.Cast(action_data);
        if (!craft || !action_data.m_Target || m_AdjustedTimeToComplete <= 0)
            return;

        if (craft.m_RecipeID != PluginRecipesManager.RecipeIDFromClassname("RTP_RepairWithToolbox"))
            return;

        Object part = action_data.m_Target.GetObject();
        if (RTP_Toolbox.Cast(part))
            part = action_data.m_MainItem;

        RTP_ToolboxRepairGroup group = RepairToPristineUpdatedConfig.GetToolboxSettings().FindPartGroup(part);
        if (group)
            m_AdjustedTimeToComplete = group.RepairTime;
    }
}
