/** GENERATED BUILD FILE. DO NOT MODIFY. */

/**
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CONFIG_H
#define CONFIG_H

#pragma region Global Settings

#define DEFAULT_SCENE_INDEX 0
#define TARGET_FPS 60
#define TARGET_MEM_BYTES 4000000 // Change to 8000000 for Expansion Pak

#define TOTAL_ENGINE_MEM_BUDGET (SCENE_DATA_MEM + GLOBAL_DATA_MEM) // 80000 default
#define SCENE_DATA_MEM 64000
#define GLOBAL_DATA_MEM 16000

#define GAME_NAME "default"
#define SCENE_DATA_PATH "./{GAME_NAME}_data/scenes.bin"

#pragma endregion

#pragma region Rendering

#define FRAMEBUFFER_COUNT 2
#define DITHER_MODE 15 // DITHER_NONE_NONE by default

#pragma endregion

#endif // CONFIG_H
