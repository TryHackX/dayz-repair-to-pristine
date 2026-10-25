modded class DayZGame
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        if (rpc_type == RepairToPristineUpdatedConfig.RPC_SYNC_CONFIG)
        {
            if (GetGame().IsClient() || !GetGame().IsMultiplayer())
            {
                RepairToPristineUpdatedConfig.OnRPC(ctx);
            }
            return;
        }

        super.OnRPC(sender, target, rpc_type, ctx);
    }
}