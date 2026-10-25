// Repair recipes that ask PluginRepairing.CanRepair: open the damage window of the repaired item (index 1)
// to worn..ruined, the modded CanRepair then decides with the server config (pristine / ruined repair).
// RepairWithSewingKit, RepairWithLeatherSewingKit and RepairWithTireKit replaced PatchItem in DayZ 1.28.
modded class CleanWeapon extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairWithTape extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class SharpenMelee extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairEpoxy extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairElectric extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairWithSewingKit extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairWithLeatherSewingKit extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

modded class RepairWithTireKit extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }
}

// These two do not go through PluginRepairing.CanRepair, so the config is checked here.
// Vanilla allows only damaged and badly damaged items.
// No super.CanDo() here: on DayZ 1.30 a super call with the ingredients[] array from a modded recipe
// can run on the wrong object ("Calling nonexistent function" VM exception), so the vanilla checks are inlined.
modded class RepairPlanks extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }

    override bool CanDo(ItemBase ingredients[], PlayerBase player)
    {
        // RecipeBase.CanDo: no ingredient may carry attachments
        for (int i = 0; i < MAX_NUMBER_OF_INGREDIENTS; i++)
        {
            if (ingredients[i].GetInventory() && ingredients[i].GetInventory().AttachmentCount() > 0)
                return false;
        }

        return RepairToPristineUpdatedConfig.CanRepairHealthLevel(ingredients[1].GetHealthLevel(), GameConstants.STATE_DAMAGED);
    }
}

modded class RepairWithPliers extends RecipeBase
{
    override void Init()
    {
        super.Init();
        m_MinDamageIngredient[1] = GameConstants.STATE_WORN;
        m_MaxDamageIngredient[1] = -1;
    }

    override bool CanDo(ItemBase ingredients[], PlayerBase player)
    {
        // vanilla RepairWithPliers.CanDo has no conditions
        return RepairToPristineUpdatedConfig.CanRepairHealthLevel(ingredients[1].GetHealthLevel(), GameConstants.STATE_DAMAGED);
    }
}
