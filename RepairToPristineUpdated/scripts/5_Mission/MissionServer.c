modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        RepairToPristineUpdatedConfig.Init();
    }
    
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        
        if (identity)
        {
            RepairToPristineUpdatedConfig.SyncConfig(identity);
        }
    }
}