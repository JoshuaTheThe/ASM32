override MAKEFLAGS += -rR

override OUTPUT := asm32

define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

override DEFAULT_KCC := gcc
$(eval $(call DEFAULT_VAR,KCC,$(DEFAULT_KCC)))

override DEFAULT_KLD := gcc
$(eval $(call DEFAULT_VAR,KLD,$(DEFAULT_KLD)))

override DEFAULT_KCFLAGS := -Wall -Wextra -Wpedantic -Werror -g
$(eval $(call DEFAULT_VAR,KCFLAGS,$(DEFAULT_KCFLAGS)))

override DEFAULT_KCPPFLAGS :=
$(eval $(call DEFAULT_VAR,KCPPFLAGS,$(DEFAULT_KCPPFLAGS)))

override DEFAULT_KLDFLAGS := -lm
$(eval $(call DEFAULT_VAR,KLDFLAGS,$(DEFAULT_KLDFLAGS)))

override CFLAGS := \
    -Wall \
    -Wextra \
    -m64 \
    -march=native \
    $(DEFAULT_KCFLAGS)

override CXXFLAGS := \
    -Wall \
    -Wextra \
    -std=c++20 \
    -m64 \
    -march=native \
    $(KCPPFLAGS)

override KCPPFLAGS := \
    -I ./src \
    $(KCPPFLAGS)

override KNASMFLAGS += \
    -Wall \
    -f elf64

override KLDFLAGS += \
    -m64 \
    -lgcc

override CFILES := $(shell find src -type f -name '*.c' | LC_ALL=C sort)

# FIXED: Preserve directory structure in object files
override OBJ := $(CFILES:src/%.c=obj/%.c.o)

override HEADER_DEPS := $(CFILES:src/%.c=obj/%.c.d)

.PHONY: all
all: bin/$(OUTPUT)

.PHONY: new
new: CFLAGS += -DLEGACY_CODEGEN=false
new: all

bin/$(OUTPUT): $(OBJ)
	@mkdir -p "$$(dirname $@)"
	$(KLD) $(OBJ) $(KLDFLAGS) -o $@

-include $(HEADER_DEPS)

obj/%.c.o: src/%.c
	@mkdir -p "$$(dirname $@)"
	$(KCC) $(CFLAGS) $(KCPPFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf bin obj

.PHONY: run
run: all
	./bin/$(OUTPUT)

.PHONY: debug
debug: CXXFLAGS += -O0 -ggdb3
debug: all