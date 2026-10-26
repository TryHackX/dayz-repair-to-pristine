# Repair To Pristine Updated

Repair items and vehicle parts with vanilla repair kits up to Pristine - and optionally Ruined ones - or with the new toolbox. Fully configurable on the server. For DayZ 1.30 (Badlands).

[Steam Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=3655699332) · DayZ 1.30 (Badlands) · author: TryHackX

## Support

If the mod is useful for you or your server, you can support further development:

| Currency | Network | Address |
|---|---|---|
| Monero (XMR) | Monero | `45hvee4Jv7qeAm6SrBzXb9YVjb8DkHtFtFh7qkDMxS9zYX3NRi1dV27MtSdVC5X8T1YVoiG8XFiJkh4p9UncqWGxHi4tiwk` |
| Bitcoin (BTC) | Bitcoin (on-chain) | `bc1qncavcek4kknpvykedxas8kxash9kdng990qed2` |
| Ethereum (ETH) | Ethereum (mainnet) | `0xa3d38d5Cf202598dd782C611e9F43f342C967cF5` |

## Features

- Repair items from Worn, Damaged or Badly Damaged up to **Pristine** with the vanilla repair kits
  (Weapon Cleaning Kit, Sewing Kit, Leather Sewing Kit, Whetstone, Duct Tape, Tire Repair Kit, Electronic Repair Kit, Epoxy Putty, Blowtorch).
- Optional repair of **Ruined** items (off by default).
- Vehicles: car, motorbike and boat parts, chassis and engine - each group with its own switches and kit cost modifiers.
- Kit usage modifiers for normal, pristine and ruined repairs.
- Ruined wheels are replaced with an undamaged wheel model after repair.
- Optional kits for parts vanilla cannot repair: car radiators (Duct Tape, Epoxy Putty, Blowtorch) and spark / glow plugs (Electronic Repair Kit). Off by default.
- Battery repair with the Electronic Repair Kit has its own group (on / off, cost, modifiers); the defaults match vanilla.
- **Toolbox** with own models and textures: Toolbox, Ratchet and Steel Brush.
  - The toolbox has a slot for every tool and cargo space. Its weight includes everything inside.
  - With the right tools inside it repairs vehicle engines without a blowtorch (cars, motorbikes, boats, also DayZ Expansion helicopters, boats and cars).
  - It also repairs detached radiators, spark and glow plugs, batteries, doors, hoods, trunks and motorbike shields.
  - The toolbox itself is repaired with Epoxy Putty or a Blowtorch.
- **Every repair group has its own settings:**
  - on / off, worn -> Pristine and Ruined -> Badly Damaged,
  - cost modifiers per health level,
  - the repaired classes (any mod),
  - kits with their costs, or tools with their own wear,
  - toolbox wear and repair time.

## Configuration

`<server profile>/RepairToPristineUpdated/RepairToPristineConfig.json` (created on the first start, synced to players):

| Setting | Default | Meaning |
|---|---|---|
| `EnablePristineRepair` | 1 | items can be repaired up to Pristine |
| `EnableRuinedRepair` | 0 | Ruined items can be repaired |
| `RepairKitModifier` / `PristineRepairKitModifier` / `RuinedRepairKitModifier` | 0.8 / 1.2 / 2.5 | kit usage per repair |
| `VehicleParts` / `VehicleChassis` / `VehicleEngine` | pristine on, ruined off | the same switches and modifiers per vehicle group |
| `PartKits` | Radiator off, SparkPlug off, Battery on, Toolbox on | kit repairs with their own rules (items without a vanilla kit, and batteries): `Enabled`, `EnablePristineRepair`, `EnableRuinedRepair`, three modifiers, `Items`, `Kits` (`ClassName`, `Cost` %) |
| `ToolboxRepairs` | Engine, Radiator, SparkPlug, Battery, BodyParts on | the same rules plus `Tools` (`ClassName`, `Wear` %), `ToolboxWear` % (0 = off) and `RepairTime` s |
| `DebugMode` | 0 | extra lines in the server log |

A ruined engine destroys the whole vehicle in vanilla (every car, truck, motorbike and the boat). Nothing can repair such a wreck, and the server removes it after `CleanupLifetimeRuined` (330 s by default). So `EnableRuinedRepair` in `VehicleEngine` and `ToolboxRepairs` → `Engine` only works on mod vehicles whose engine does not destroy them.

## Toolbox

- **Engine:** hold the toolbox, look at the engine (engine off) and choose *Repair engine with toolbox*.
  - Works on cars, motorbikes and boats, also on DayZ Expansion helicopters, boats and cars.
  - Default tools: hammer, wrench, ratchet, pliers, screwdriver.
- **Detached parts:** combine the toolbox with the part and choose *Repair with toolbox*.
  - Radiator and Expansion hydraulic hoses: pliers + duct tape.
  - Spark and glow plug (also the Expansion helicopter igniter plug): pliers + steel brush.
  - Car, truck and Expansion helicopter / aircraft battery: screwdriver + duct tape.
  - Door, hood, trunk (every car mod) and motorbike shield: hammer + epoxy putty.
- **Tools** count in their slots and in the cargo.
  - Every tool loses its own `Wear` % of health per repair. Duct tape, epoxy putty and the electronic kit lose that % of their quantity.
  - The wear is multiplied by the group modifier of the repaired health level.
- **Group settings:**
  - `ToolboxWear` > 0 also wears the toolbox itself.
  - `RepairTime` sets the repair time per group.
  - `Items` takes class names from any mod.
  - An empty `Tools` list means no tools are needed. A group removed from the file gets its defaults again.
- **Example:** the whole default config is in `RepairToPristineConfig.default.json`.
- **Loot:** the items spawn only after you add `rtp_types.xml` and `rtp_spawnabletypes.xml` from the mod folder to your mission's `cfgeconomycore.xml`. See `README.txt` for the exact lines.

## Installation

- Add the mod to the `-mod=` parameter of the server **and** the client.
- Copy `keys/TryHackX_New.bikey` to the `keys` folder of the server.

## Building from source

1. DayZ Tools → **AddonBuilder**: source directory `RepairToPristineUpdated`, addon prefix `RepairToPristineUpdated`, output `addons/`.
2. Sign the PBO with your own key (DayZ Tools → DSUtils / DSCreateKey + DSSignFile) and give servers your `.bikey`.
   The release on Steam Workshop is signed with `TryHackX_New`.

## Compatibility

- DayZ 1.30 (Badlands), tested on a dedicated server.
- No other mods required. Works together with Crafting Repair Tools.
- DayZ Expansion: tested with Expansion Core, Licensed and Vehicles loaded.
  - Helicopter and aircraft batteries, igniter plugs, hydraulic hoses and UAZ doors are repaired with the toolbox.
  - So are the engines of the gyrocopter, the Zodiac boat and the UAZ.
- Replaces the old "Repair My Car" mod. Its scripts do not load on DayZ 1.30, and its BlowTorch class breaks the vanilla blowtorch.

## License

© TryHackX. You are free to use this mod on your server. Please do not repack or reupload it without permission.
