# Repair To Pristine Updated

Repair items and vehicle parts with vanilla repair kits up to Pristine - and optionally Ruined ones - fully configurable on the server. For DayZ 1.30 (Badlands).

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

## Configuration

`<server profile>/RepairToPristineUpdated/RepairToPristineConfig.json` (created on the first start, synced to players):

| Setting | Default | Meaning |
|---|---|---|
| `EnablePristineRepair` | 1 | items can be repaired up to Pristine |
| `EnableRuinedRepair` | 0 | Ruined items can be repaired |
| `RepairKitModifier` / `PristineRepairKitModifier` / `RuinedRepairKitModifier` | 0.8 / 1.2 / 2.5 | kit usage per repair |
| `VehicleParts` / `VehicleChassis` / `VehicleEngine` | pristine on, ruined off | the same switches and modifiers per vehicle group |
| `DebugMode` | 0 | extra lines in the server log |

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

## License

© TryHackX. You are free to use this mod on your server. Please do not repack or reupload it without permission.
