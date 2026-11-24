#include "raylib.h"
#include "raymath.h"

#include <array>
#include <cmath>
#include <string>
#include <sstream>

constexpr int GRID = 5;
constexpr float TILE = 2.0f;
constexpr float HALF_GRID = (GRID * TILE) / 2.0f;

struct SeedType {
    const char* name;
    int price;
    int sellValue;
    float growSeconds;
    Color color;
};

const std::array<SeedType, 4> SEEDS{{
    {"Carrot",      5,  12, 12.0f, ORANGE},
    {"Tomato",      9,  22, 18.0f, RED},
    {"Pumpkin",    15,  40, 26.0f, GOLD},
    {"Star Berry", 25, 75, 35.0f, SKYBLUE}
}};

struct Plot {
    int seed = -1;
    float age = 0.0f;
};

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 800, "Grow Your Farm - C++ Farming Game");
    SetTargetFPS(60);

    Camera3D camera{};
    camera.position = {12.0f, 18.0f, 16.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::array<Plot, GRID * GRID> plots{};
    std::array<int, 4> inventory{{3, 0, 0, 0}};
    int coins = 35;
    int selectedSeed = 0;
    bool shopOpen = false;
    int hoveredPlot = -1;
    std::string message = "Welcome! Plant a carrot to get started.";

    auto plotCenter = [](int index) {
        int x = index % GRID;
        int z = index / GRID;
        return Vector3{
            -HALF_GRID + TILE * 0.5f + x * TILE,
            0.0f,
            -HALF_GRID + TILE * 0.5f + z * TILE
        };
    };

    auto getHoveredPlot = [&]() -> int {
        Ray ray = GetMouseRay(GetMousePosition(), camera);
        if (std::fabs(ray.direction.y) < 0.0001f) return -1;
        float distance = -ray.position.y / ray.direction.y;
        if (distance < 0.0f) return -1;
        Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));
        int x = static_cast<int>(std::floor((hit.x + HALF_GRID) / TILE));
        int z = static_cast<int>(std::floor((hit.z + HALF_GRID) / TILE));
        if (x < 0 || x >= GRID || z < 0 || z >= GRID) return -1;
        return z * GRID + x;
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (Plot& plot : plots) {
            if (plot.seed >= 0 && plot.age < SEEDS[plot.seed].growSeconds) {
                plot.age += dt;
                if (plot.age > SEEDS[plot.seed].growSeconds) {
                    plot.age = SEEDS[plot.seed].growSeconds;
                }
            }
        }

        hoveredPlot = getHoveredPlot();

        if (IsKeyPressed(KEY_B)) {
            shopOpen = !shopOpen;
            message = shopOpen ? "Seed shop: press 1-4 to buy seeds." : "Shop closed.";
        }

        if (!shopOpen) {
            for (int i = 0; i < 4; ++i) {
                if (IsKeyPressed(KEY_ONE + i)) {
                    selectedSeed = i;
                    message = std::string("Selected ") + SEEDS[i].name + ".";
                }
            }
        } else {
            for (int i = 0; i < 4; ++i) {
                if (IsKeyPressed(KEY_ONE + i)) {
                    if (coins >= SEEDS[i].price) {
                        coins -= SEEDS[i].price;
                        inventory[i]++;
                        message = std::string("Bought 1 ") + SEEDS[i].name + " seed.";
                    } else {
                        message = "Not enough coins for that seed.";
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !shopOpen && hoveredPlot >= 0) {
            Plot& plot = plots[hoveredPlot];
            if (plot.seed < 0) {
                if (inventory[selectedSeed] > 0) {
                    inventory[selectedSeed]--;
                    plot.seed = selectedSeed;
                    plot.age = 0.0f;
                    message = std::string("Planted ") + SEEDS[selectedSeed].name + "!";
                } else {
                    message = "You don't have that seed. Press B to visit the shop.";
                }
            } else if (plot.age >= SEEDS[plot.seed].growSeconds) {
                coins += SEEDS[plot.seed].sellValue;
                message = std::string("Harvested ") + SEEDS[plot.seed].name +
                          "! +" + std::to_string(SEEDS[plot.seed].sellValue) + " coins.";
                plot = Plot{};
            } else {
                message = "That crop is still growing. Come back soon!";
            }
        }

        BeginDrawing();
        ClearBackground(Color{145, 205, 238, 255});

        BeginMode3D(camera);
        DrawPlane({0.0f, -0.08f, 0.0f}, {42.0f, 42.0f}, Color{105, 170, 92, 255});

        // Decorative path around the garden
        DrawCube({0.0f, -0.015f, 7.2f}, 13.0f, 0.06f, 1.2f, BEIGE);
        DrawCube({-7.2f, -0.015f, 0.0f}, 1.2f, 0.06f, 13.0f, BEIGE);

        for (int i = 0; i < GRID * GRID; ++i) {
            Vector3 c = plotCenter(i);
            bool hovered = (i == hoveredPlot && !shopOpen);
            Color soil = hovered ? Color{151, 103, 61, 255} : Color{126, 82, 48, 255};
            DrawCube({c.x, 0.03f, c.z}, TILE - 0.08f, 0.12f, TILE - 0.08f, soil);
            DrawCubeWires({c.x, 0.03f, c.z}, TILE - 0.08f, 0.12f, TILE - 0.08f, Color{82, 54, 34, 255});

            const Plot& plot = plots[i];
            if (plot.seed >= 0) {
                const SeedType& seed = SEEDS[plot.seed];
                float progress = plot.age / seed.growSeconds;
                float height = 0.25f + progress * 1.1f;
                DrawCylinder({c.x, 0.16f + height * 0.5f, c.z}, 0.07f, 0.045f, height, 8, DARKGREEN);

                float leafSize = 0.18f + progress * 0.18f;
                DrawSphere({c.x - 0.18f, 0.27f + height * 0.55f, c.z}, leafSize, GREEN);
                DrawSphere({c.x + 0.18f, 0.30f + height * 0.60f, c.z + 0.08f}, leafSize, LIME);

                if (progress > 0.35f) {
                    float fruitSize = 0.12f + progress * 0.18f;
                    DrawSphere({c.x, 0.35f + height * 0.72f, c.z}, fruitSize, seed.color);
                }
                if (progress >= 1.0f) {
                    DrawSphere({c.x, 0.58f + height, c.z}, 0.10f, GOLD);
                }
            }
        }

        // A little wooden seed-shop stand
        DrawCube({7.0f, 0.65f, -5.4f}, 3.2f, 1.3f, 1.8f, Color{154, 95, 52, 255});
        DrawCube({7.0f, 1.45f, -5.4f}, 3.8f, 0.25f, 2.2f, Color{105, 60, 35, 255});
        DrawCube({7.0f, 2.0f, -5.4f}, 3.0f, 0.9f, 0.18f, Color{245, 220, 150, 255});
        DrawSphere({6.2f, 2.65f, -5.4f}, 0.25f, GREEN);
        DrawSphere({7.0f, 2.65f, -5.4f}, 0.25f, ORANGE);
        DrawSphere({7.8f, 2.65f, -5.4f}, 0.25f, RED);
        EndMode3D();

        DrawRectangle(0, 0, 1280, 86, Color{35, 52, 40, 235});
        DrawText("GROW YOUR FARM", 24, 15, 30, RAYWHITE);
        DrawText(TextFormat("COINS: %i", coins), 390, 20, 26, GOLD);
        DrawText(TextFormat("SELECTED: %s  [ %i seeds ]", SEEDS[selectedSeed].name, inventory[selectedSeed]),
                 590, 22, 21, RAYWHITE);

        DrawRectangle(18, 105, 305, 180, Color{35, 52, 40, 220});
        DrawText("HOW TO PLAY", 34, 118, 22, GOLD);
        DrawText("1-4: Select seed", 34, 153, 18, RAYWHITE);
        DrawText("B: Open / close seed shop", 34, 179, 18, RAYWHITE);
        DrawText("Click empty soil: Plant", 34, 205, 18, RAYWHITE);
        DrawText("Click ripe crop: Harvest", 34, 231, 18, RAYWHITE);
        DrawText("Crops grow while you play!", 34, 257, 17, LIME);

        DrawRectangle(18, 700, 1244, 72, Color{35, 52, 40, 225});
        DrawText(message.c_str(), 35, 716, 22, RAYWHITE);
        DrawText("Seed shop is the wooden stand in the garden.", 35, 744, 16, BEIGE);

        if (shopOpen) {
            DrawRectangle(390, 170, 500, 430, Color{28, 43, 34, 245});
            DrawRectangleLines(390, 170, 500, 430, GOLD);
            DrawText("SEED SHOP", 535, 195, 34, GOLD);
            for (int i = 0; i < 4; ++i) {
                int y = 260 + i * 72;
                DrawCircle(440, y + 12, 13, SEEDS[i].color);
                DrawText(TextFormat("%i. %s  -  %i coins", i + 1, SEEDS[i].name, SEEDS[i].price),
                         470, y, 22, RAYWHITE);
                DrawText(TextFormat("Sell for %i coins", SEEDS[i].sellValue), 470, y + 27, 16, LIGHTGRAY);
            }
            DrawText("Press B to close", 535, 550, 18, BEIGE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
