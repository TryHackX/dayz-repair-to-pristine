Repair To Pristine Updated 2.2 - server setup (DayZ 1.30 Badlands)

1. Copy keys\TryHackX_New.bikey to your server's "keys" folder.

2. Start the server once - the config is created automatically (older configs are migrated):
   <server profile>\RepairToPristineUpdated\RepairToPristineConfig.json
   RepairToPristineConfig.default.json in this folder shows the whole file with its default values.

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
   A ruined engine destroys the whole vehicle in vanilla (every car, truck, motorbike and the boat). Nothing can
   repair such a wreck, and the server removes it after CleanupLifetimeRuined (330 s by default). So
   "EnableRuinedRepair" of "VehicleEngine" and of the toolbox "Engine" group only works on mod vehicles whose engine
   does not destroy them.
   The config is sent to every player on connect - restart the server after editing it.

3. Repair groups (PartKits and ToolboxRepairs)
   Every group has the same rules:
     "Enabled": 1                       the repair is available
     "EnablePristineRepair": 1          worn -> pristine
     "EnableRuinedRepair": 0            ruined -> badly damaged
     "RepairModifier": 1.0              multiplier of kit cost / tool wear for normal repairs
     "PristineRepairModifier": 1.2      ... for worn -> pristine
     "RuinedRepairModifier": 2.5        ... for ruined -> badly damaged
     "Items": [...]                     class names the group repairs (subclasses count too, any mod)
   Each repair moves one health level up.

   "PartKits" - kit repairs with their own rules: items that have no kit in vanilla, and batteries. A listed item
   uses only these kits and rules. "Kits": [{"ClassName": ..., "Cost": % of the kit per repair}]
     "Radiator"   off  CarRadiator, ExpansionHydraulicHoses    Duct Tape 25, Epoxy Putty 25, Blowtorch 25
     "SparkPlug"  off  SparkPlug, GlowPlug (Expansion's helicopter plug is a GlowPlug)   Electronic Repair Kit 10
     "Battery"    on   CarBattery, TruckBattery                Electronic Repair Kit 25 (as in vanilla)
     "Toolbox"    on   RTP_Toolbox                             Epoxy Putty 25, Blowtorch 25
   "Battery" 0 = batteries cannot be repaired with kits. Expansion batteries follow Expansion's own config
   unless you add ExpansionHelicopterBattery / ExpansionAircraftBattery to its "Items". An older config gets the
   "Battery" group with its own item settings (RepairKitModifier ...), so batteries keep working as before.

   "ToolboxRepairs" - repairs with the toolbox in hands. "Tools": [{"ClassName": ..., "Wear": %}]: every listed
   tool must be in the toolbox (in its slot or in the cargo) and loses Wear % of its health per repair; items
   with quantity (duct tape, epoxy putty, electronic kit ...) lose Wear % of their quantity. "Tools": [] = no
   tools needed. "ToolboxWear": % of the toolbox's own health per repair (0 = off). "RepairTime": seconds.
     "Engine"     CarScript, MotorbikeScript, BoatScript (also DayZ Expansion cars, helicopters and boats)
                  look at the engine (engine off) -> "Repair engine with toolbox", 10 s per step
                  Hammer 3, Wrench 4, RTP_Ratchet 4, Pliers 3, Screwdriver 3
     "Radiator"   CarRadiator, ExpansionHydraulicHoses          Pliers 3, DuctTape 15           6 s
     "SparkPlug"  SparkPlug, GlowPlug                           Pliers 2, RTP_SteelBrush 4      5 s
     "Battery"    CarBattery, TruckBattery, ExpansionHelicopterBattery, ExpansionAircraftBattery
                                                                Screwdriver 3, DuctTape 15      6 s
     "BodyParts"  CarDoor (doors, hoods, trunks of every car mod), Motorbike_02_ShieldFront_ColorBase (shields)
                                                                Hammer 4, EpoxyPutty 15         8 s
   Detached parts: combine the toolbox with the part -> "Repair with toolbox".
   A group removed from the file gets its defaults again. Wear and kit costs are 0-100, RepairTime 1-120.

4. What can be repaired with kits
   Weapon Cleaning Kit   - weapons, magazines, suppressors
   Sewing Kit            - clothing, bags, vests (textile)
   Leather Sewing Kit    - leather items, boots
   Duct Tape             - most clothing, bags and gear
   Epoxy Putty           - helmets and other hard items; car doors, hoods, trunks and car body; the toolbox
   Electronic Repair Kit - electronics (NV goggles, radios, lights ...), car and truck batteries (PartKits "Battery")
   Whetstone             - knives, axes and other blades
   Tire Repair Kit       - wheels, also ruined wheels (the ruined wheel model is swapped back); boat hull
   Blowtorch             - metal items, car / motorbike / boat parts, body and engines; the toolbox
   Wooden Planks         - wooden crates and sea chests
   Pliers                - chainmail
   Radiators and spark / glow plugs only when their PartKits group is enabled.
   Car doors, hoods and trunks must be opened before repairing them on the vehicle, engines must be off.

5. Toolbox, Ratchet and Steel Brush (new items, own models)
   The toolbox has a slot for a wrench, ratchet, screwdriver, pliers, hammer, steel brush, duct tape,
   epoxy putty and an electronic repair kit, plus cargo space; its weight includes everything inside.

6. Loot (the items do not spawn until you add them)
   rtp_types.xml          - RTP_Toolbox (10/6, Industrial, Farm), RTP_Ratchet and RTP_SteelBrush (30/20)
   rtp_spawnabletypes.xml - a found toolbox sometimes holds a few tools
   Copy both into your mission folder (e.g. a "rtp_ce" folder) and register them in cfgeconomycore.xml:
     <ce folder="rtp_ce">
       <file name="rtp_types.xml" type="types"/>
       <file name="rtp_spawnabletypes.xml" type="spawnabletypes"/>
     </ce>
