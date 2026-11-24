# Grow Your Farm (C++)

A small 3D farming game starter made with C++ and raylib.

## Features
- 3D garden with 25 soil plots
- Four seed types with different prices, growth times, and sell values
- Crops grow in real time while the game is running
- Buy seeds, plant them, and harvest ripe crops for coins
- Simple seed shop and inventory display

## Controls
- `1` to `4`: Select a seed while the shop is closed
- `B`: Open or close the seed shop
- In the shop, press `1` to `4` to buy that seed
- Left-click an empty soil plot to plant
- Left-click a fully grown crop to harvest it

## Build on Windows with Visual Studio and vcpkg

1. Install CMake and Visual Studio 2022 with the **Desktop development with C++** workload.
2. Install vcpkg, then install raylib:

   ```powershell
   vcpkg install raylib:x64-windows
   ```

3. From this folder, configure the project (replace the path with your vcpkg location):

   ```powershell
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
   ```

4. Build it:

   ```powershell
   cmake --build build --config Release
   ```

5. Run `build/Release/GrowYourFarm.exe`.

## Notes
This is a starter game project, not a Roblox project. It uses raylib for its 3D window, rendering, input, and camera ray picking.
