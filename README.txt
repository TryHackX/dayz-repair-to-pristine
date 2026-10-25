Repair To Pristine Updated 2.1 - server setup (DayZ 1.30 Badlands)

1. Copy keys\TryHackX_New.bikey to your server's "keys" folder.

2. Start the server once - the config is created automatically (older configs are migrated):
   <server profile>\RepairToPristineUpdated\RepairToPristineConfig.json

   Items (repaired by combining a kit with the item):
     "RepairKitModifier": 0.8           kit usage for normal repairs (badly damaged -> damaged -> worn)
     "PristineRepairKitModifier": 1.2   kit usage for worn -> pristine
     "EnablePristineRepair": 1          1 = items can be repaired to pristine
     "EnableRuinedRepair": 0            1 = ruined items can be repaired (ruined -> badly damaged)
     "RuinedRepairKitModifier": 2.5     kit usage for ruined -> badly damaged
     "DebugMode": 0                     1 = every repair is written to the server script log

   Vehicles - three groups with the same five settings each:
     "VehicleParts"    car doors, hoods, trunks (Epoxy Putty, Blowtorch), motorbike parts (Blowtorch)
     "VehicleChassis"  body zones of cars (Epoxy Putty, planks, tarp, Blowtorch), motorbikes (Blowtorch)
                       and boats (Tire Repair Kit)
     "VehicleEngine"   engines of cars, motorbikes and boats (Blowtorch)

     "EnablePristineRepair": 1          worn -> pristine
     "EnableRuinedRepair": 0            1 = ruined -> badly damaged (off by default in every group)
     "RepairKitModifier": 1.0           kit usage for normal vehicle repairs (1.0 = vanilla)
     "PristineRepairKitModifier": 1.2
     "RuinedRepairKitModifier": 2.5

   Item modifiers multiply the vanilla kit cost of the item; the kit must have at least cost x modifier
   left (e.g. ruined jeans, cost 25, need a sewing kit with 62.5 % or more). Vehicle modifiers multiply the
   vanilla 25 % of the kit used per repair. 0 = free repairs. The Blowtorch uses its battery as in vanilla.
   The config is sent to every player on connect - restart the server after editing it.

3. What can be repaired
   Weapon Cleaning Kit   - weapons, magazines, suppressors
   Sewing Kit            - clothing, bags, vests (textile)
   Leather Sewing Kit    - leather items, boots
   Duct Tape             - most clothing, bags and gear
   Epoxy Putty           - helmets and other hard items; car doors, hoods, trunks and car body
   Electronic Repair Kit - electronics (NV goggles, radios, lights ...)
   Whetstone             - knives, axes and other blades
   Tire Repair Kit       - wheels, also ruined wheels (the ruined wheel model is swapped back); boat hull
   Blowtorch             - metal items, car / motorbike / boat parts, body and engines
   Wooden Planks         - wooden crates and sea chests
   Pliers                - chainmail
   Car doors, hoods and trunks must be opened before repairing them on the vehicle, engines must be off.
