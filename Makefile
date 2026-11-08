# ---------------------------------------------------------------
# CEngine - N64 (libdragon + Tiny3D)
# ---------------------------------------------------------------
BUILD_DIR = build
SRC_DIR   = src
ROM_NAME  = cengine

N64_INST ?= $(HOME)/.local/n64_toolchain
export N64_INST
export PATH := $(N64_INST)/bin:$(PATH)

include $(N64_INST)/include/n64.mk
include $(N64_INST)/include/t3d.mk

# ---------------------------------------------------------------
# Sources: every .c under src/ (main.c lives in src/engine/)
# ---------------------------------------------------------------
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJS      := $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS      := $(OBJS:.o=.d)

INC_DIRS := $(sort $(dir $(shell find $(SRC_DIR) -name '*.h')))
N64_CFLAGS += $(addprefix -I,$(INC_DIRS))
N64_CFLAGS += -Wno-missing-braces

# ---------------------------------------------------------------
# Assets (optional): anything in assets/ gets packed into the ROM
# filesystem. Remove this block if you don't have assets yet.
# ---------------------------------------------------------------
ASSETS_DIR := src/game/assets
ASSETS_PNG := $(wildcard $(ASSETS_DIR)/*.png)
ASSETS_GLB := $(wildcard $(ASSETS_DIR)/*.glb)
ASSETS_WAV := $(wildcard $(ASSETS_DIR)/*.wav)

ASSETS_CONV := $(ASSETS_PNG:$(ASSETS_DIR)/%.png=filesystem/%.sprite) \
               $(ASSETS_GLB:$(ASSETS_DIR)/%.glb=filesystem/%.t3dm) \
               $(ASSETS_WAV:$(ASSETS_DIR)/%.wav=filesystem/%.wav64)

filesystem/%.sprite: $(ASSETS_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: $(ASSETS_DIR)/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

filesystem/%.wav64: $(ASSETS_DIR)/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

# ---------------------------------------------------------------
# Targets
# ---------------------------------------------------------------
all: $(ROM_NAME).z64

$(BUILD_DIR)/$(ROM_NAME).dfs: $(ASSETS_CONV)
$(BUILD_DIR)/$(ROM_NAME).elf: $(OBJS)

$(ROM_NAME).z64: N64_ROM_TITLE = "CEngine"
$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) filesystem $(ROM_NAME).z64

# Convenience: build and run in ares
run: $(ROM_NAME).z64
	ares $(ROM_NAME).z64

-include $(DEPS)

.PHONY: all clean run