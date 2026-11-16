BUILD_DIR = build
SRC_DIR   = src
ROM_NAME  = cengine

# Build configuration: 'make debug' or 'make DEBUG=1'
DEBUG ?= 0
ifeq ($(DEBUG),1)
BUILD_DIR := build_debug
ROM_NAME  := cengine_debug
endif

N64_INST ?= $(HOME)/.local/n64_toolchain
export N64_INST
export PATH := $(N64_INST)/bin:$(PATH)

include $(N64_INST)/include/n64.mk
include $(N64_INST)/include/t3d.mk

ifeq ($(DEBUG),1)
N64_CFLAGS += -DDEBUG -g -O0
else
N64_CFLAGS += -O2
endif

# Source files: every .c under src/
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJS      := $(SRC_FILES:%.c=$(BUILD_DIR)/%.o)
DEPS      := $(OBJS:.o=.d)

INC_DIRS := $(sort $(dir $(shell find $(SRC_DIR) -name '*.h')))
N64_CFLAGS += $(addprefix -I,$(INC_DIRS))
N64_CFLAGS += -Wno-missing-braces

# Assets
ASSETS_DIR := src/game/assets
ASSETS_PNG := $(wildcard $(ASSETS_DIR)/*.png)
ASSETS_GLB := $(wildcard $(ASSETS_DIR)/models/*.glb)
ASSETS_WAV := $(wildcard $(ASSETS_DIR)/audio/*.wav)

ASSETS_CONV := $(ASSETS_PNG:$(ASSETS_DIR)/%.png=filesystem/%.sprite) \
               $(ASSETS_GLB:$(ASSETS_DIR)/%.glb=filesystem/%.t3dm) \
               $(ASSETS_WAV:$(ASSETS_DIR)/%.wav=filesystem/%.wav64)

filesystem/%.sprite: $(ASSETS_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

filesystem/%.t3dm: $(ASSETS_DIR)/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@ --asset-path=$(ASSETS_DIR) --verbose
	$(N64_BINDIR)/mkasset -c 1 -o $(dir $@) $@

filesystem/%.wav64: $(ASSETS_DIR)/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

# Targets
all: $(ROM_NAME).z64
	@python3 tools/n64crc.py $(ROM_NAME).z64

# '| filesystem' to ensure the filesystem directory exists before mkdfs runs
$(BUILD_DIR)/$(ROM_NAME).dfs: $(ASSETS_CONV) | filesystem
$(BUILD_DIR)/$(ROM_NAME).elf: $(OBJS)

$(ROM_NAME).z64: N64_ROM_TITLE = "CEngine"
$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).elf $(BUILD_DIR)/$(ROM_NAME).dfs

filesystem:
	@mkdir -p filesystem

# Target for debug builds
debug:
	@$(MAKE) DEBUG=1

clean:
	rm -rf $(BUILD_DIR) $(BUILD_DIR)_debug filesystem $(ROM_NAME).z64 $(ROM_NAME)_debug.z64

run_debug: $(ROM_NAME)_debug.z64
	ares $(ROM_NAME)_debug.z64

run: $(ROM_NAME).z64
	ares $(ROM_NAME).z64

-include $(DEPS)

.PHONY: all clean run run_debug debug
