# This makefile builds the native SDL platform with the OpenGL renderer by 
# default when just calling `make`. Targets for other platforms are present, but
# not part of the default build. The renderer can be specified separately.

# All PNG, WAV and JSON files in assets/ are converted/copied and all *.c files
# in the src/ directory are compiled. There's no need to specify the list of 
# files separately.

# The tools needed to convert assets (PNG to QOI and WAV to QOA) are part of 
# sdl/sokol/wasm targets.

# Examples:

# `make sdl`                 - native SDL platform
# `make sdl RENDER=SOFTWARE` - native SDL platform with the software renderer
# `make sokol`               - native SOKOL platform
# `make sokol DEBUG=true`    - native SOKOL platform with debug symbols
# `make wasm`                - WASM (always with SOKOL platform)

# `make assets`              - only convert assets
# `make qoaconv`             - only build qoaconv (used to convert WAV to QOA)
# `make qoiconv`             - only build qoiconv (used to convert PNG to QOI)

# `make clean`               - clean binaries
# `make clean_assets`        - clean converted assets
# `make clean_all`           - clean the whole build dir (assets and binaries)


CC ?= gcc
EMCC ?= emcc
UNAME_S := $(shell uname -s)
RENDER ?= GL
USE_GLX ?= false
DEBUG ?= false
USER_CFLAGS ?=

L_FLAGS ?= -lm
C_FLAGS ?= -std=gnu99 -Wall -Wstrict-prototypes -Wno-unused-variable $(USER_CFLAGS)


GAME_SRC := $(shell find src/ -type f -name '*.c')
BUILD_DIR ?= build
BUILD_DIR_WASM ?= build/wasm
OBJ_DIR = build/obj

ASSETS_SRC_DIR ?= assets
ASSETS_BUILD_DIR = $(BUILD_DIR)/$(ASSETS_SRC_DIR)

TARGET_SDL ?= $(BUILD_DIR)/game_sdl
TARGET_SOKOL ?= $(BUILD_DIR)/game_sokol
TARGET_WASM ?= $(BUILD_DIR_WASM)/game.js

QOICONV = high_impact/tools/qoiconv
QOACONV = high_impact/tools/qoaconv



# Use sdl as the default target when just calling `make`
.DEFAULT_GOAL := sdl


ifeq ($(DEBUG), true)
	C_FLAGS := $(C_FLAGS) -g
else
	C_FLAGS := $(C_FLAGS) -O3
endif


# Rendeder ---------------------------------------------------------------------

ifeq ($(RENDER), GL)
	C_FLAGS := $(C_FLAGS) -DRENDER_GL
else ifeq ($(RENDER), SOFTWARE)
	C_FLAGS := $(C_FLAGS) -DRENDER_SOFTWARE
else
	$(error Unknown RENDER backend)
endif

ifeq ($(GL_VERSION), GLES2)
	C_FLAGS := $(C_FLAGS) -DUSE_GLES2
endif



# macOS ------------------------------------------------------------------------

ifeq ($(UNAME_S), Darwin)
	BREW_HOME := $(shell brew --prefix)
	C_FLAGS := $(C_FLAGS) -x objective-c -I/opt/homebrew/include -D_THREAD_SAFE -w
	L_FLAGS := $(L_FLAGS) -L$(BREW_HOME)/lib -framework Foundation

	ifeq ($(RENDER), GL)
		L_FLAGS := $(L_FLAGS) -GLU -framework OpenGL
	endif

	L_FLAGS_SOKOL = -framework Cocoa -framework QuartzCore -framework AudioToolbox


# Linux ------------------------------------------------------------------------

else ifeq ($(UNAME_S), Linux)
	ifeq ($(RENDER), GL)
		# Prefer modern GLVND instead of legacy X11-only GLX
		ifeq ($(USE_GLX), true)
			L_FLAGS := $(L_FLAGS) -lGL
		else
			L_FLAGS := $(L_FLAGS) -lOpenGL
		endif
	endif

	L_FLAGS_SOKOL = -lX11 -lXcursor -pthread -lXi -ldl -lasound


# Windows MSYS -----------------------------------------------------------------

else ifeq ($(shell uname -o), Msys)
	ifeq ($(RENDER), GL)
		L_FLAGS := $(L_FLAGS) -lopengl32
	endif

	L_FLAGS_SOKOL = --pthread -lkernel32 -luser32 -lshell32 -lgdi32 -lole32


# Windows NON-MSYS -------------------------------------------------------------

else ifeq ($(OS), Windows_NT)
	$(error TODO: FLAGS for windows have not been set up. Please modify this makefile and send a PR!)
else
	$(error Unknown environment)
endif



# Asset tools ------------------------------------------------------------------

qoiconv: $(QOICONV)
$(QOICONV): high_impact/libs/qoiconv.c high_impact/libs/stb_image.h high_impact/libs/stb_image_write.h high_impact/libs/qoi.h
	@mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) -std=c99 -O3 high_impact/libs/qoiconv.c -o $(QOICONV)

qoaconv: $(QOACONV)
$(QOACONV): high_impact/libs/qoaconv.c
	@mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) -std=c99 -O3 high_impact/libs/qoaconv.c -o $(QOACONV) -lm



# Assets  ----------------------------------------------------------------------

ASSETS_PNG := $(shell find $(ASSETS_SRC_DIR) -type f -name '*.png')
ASSETS_WAV := $(shell find $(ASSETS_SRC_DIR) -type f -name '*.wav')
ASSETS_JSON := $(shell find $(ASSETS_SRC_DIR) -type f -name '*.json')

ASSETS_ENC_QOI := $(ASSETS_PNG:assets/%.png=$(ASSETS_BUILD_DIR)/%.qoi)
ASSETS_ENC_QOA := $(ASSETS_WAV:assets/%.wav=$(ASSETS_BUILD_DIR)/%.qoa)
ASSETS_CP_JSON := $(ASSETS_JSON:assets/%.json=$(ASSETS_BUILD_DIR)/%.json)

assets: qoiconv qoaconv assets_build
assets_build: $(ASSETS_ENC_QOI) $(ASSETS_ENC_QOA) $(ASSETS_CP_JSON)

build/assets/%.qoi: assets/%.png
	@mkdir -p $(@D)
	$(QOICONV) $< $@

build/assets/%.qoa: assets/%.wav
	@mkdir -p $(@D)
	$(QOACONV) $< $@

build/assets/%.json: assets/%.json
	@mkdir -p $(@D)
	cp $< $@



# Platform: Native/SDL ---------------------------------------------------------

SDL_C_FLAGS = $(shell sdl2-config --cflags)
SDL_L_FLAGS = $(shell sdl2-config --libs)
SDL_COMMON_OBJ = $(patsubst %.c, $(OBJ_DIR)/sdl/%.o, $(GAME_SRC))
SDL_COMMON_DEPS = $(patsubst %.c, $(OBJ_DIR)/sdl/%.d, $(GAME_SRC))

sdl: assets sdl_build
sdl_build: $(SDL_COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $(TARGET_SDL) $(L_FLAGS) $(SDL_L_FLAGS)

$(OBJ_DIR)/sdl/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(SDL_C_FLAGS) -DPLATFORM_SDL -MMD -MP -c $< -o $@

-include $(SDL_COMMON_DEPS)



# Platform: Native/SOKOL -------------------------------------------------------

SOKOL_COMMON_OBJ = $(patsubst %.c, $(OBJ_DIR)/sokol/%.o, $(GAME_SRC))
SOKOL_COMMON_DEPS = $(patsubst %.c, $(OBJ_DIR)/sokol/%.d, $(GAME_SRC))

sokol: assets sokol_build
sokol_build: $(SOKOL_COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $(TARGET_SOKOL) $(L_FLAGS) $(L_FLAGS_SOKOL)

$(OBJ_DIR)/sokol/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -DPLATFORM_SOKOL -MMD -MP -c $< -o $@

-include $(SOKOL_COMMON_DEPS)



# Platform: WASM/Sokol ---------------------------------------------------------

COMMON_OBJ_WASM = $(patsubst %.c, $(OBJ_DIR)/wasm/%.o, $(GAME_SRC))
COMMON_DEPS_WASM = $(patsubst %.c, $(OBJ_DIR)/wasm/%.d, $(GAME_SRC))

wasm: assets wasm_build
wasm_build: $(COMMON_OBJ_WASM)
	mkdir -p $(BUILD_DIR_WASM)
	cp src/wasm_index.html $(BUILD_DIR_WASM)/game.html
	$(EMCC) $^ -o $(TARGET_WASM) -lGL \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s ENVIRONMENT=web \
		-s MIN_WEBGL_VERSION=2 \
		-s MAX_WEBGL_VERSION=2 \
		-sEXPORTED_FUNCTIONS=_main,_input_set_button_state \
		--preload-file $(ASSETS_BUILD_DIR)@$(ASSETS_SRC_DIR)

$(OBJ_DIR)/wasm/%.o: %.c
	mkdir -p $(dir $@)
	$(EMCC) $(C_FLAGS) -DPLATFORM_SOKOL -MMD -MP -c $< -o $@

-include $(COMMON_DEPS_WASM)



# Cleaing ----------------------------------------------------------------------

.PHONY: clean
clean:
	$(RM) -rf $(OBJ_DIR) $(BUILD_DIR_WASM) $(TARGET_SDL) $(TARGET_SOKOL)

.PHONY: clean_assets
clean_assets:
	$(RM) -rf $(ASSETS_BUILD_DIR)

clean_all: clean_assets clean

