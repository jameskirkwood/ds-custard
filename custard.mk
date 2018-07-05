#------------------------------------------------------------------------------
# This file extends the NDS build system included with devkitARM. To use this
# extension in your project, include this file in your makefile with the line
# below, and follow the instructions to define any required variables.
#
# include $(CUSTARD)/custard.mk
#------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment.")
endif
#------------------------------------------------------------------------------
include $(DEVKITARM)/ds_rules
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Optional variables used by devkitARM to build the ROM
#------------------------------------------------------------------------------
# GAME_TITLE, GAME_SUBTITLE1, GAME_SUBTITLE2 : ROM embedded metadata
# NITRO_FILES : Directory to be embedded in the ROM's NitroFS file system
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# You must set the variable BUILD to determine the directory where intermediate
# output files will be placed BEFORE including this file. The directory will
# be created automatically.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Optional variables to be set BEFORE including this file
#------------------------------------------------------------------------------
# Smart defaults are given to these variables (or appended/prepended if already
# set) by the inclusion of this file, but may be overridden afterwards if need
# be. Custard is not included by default.
#------------------------------------------------------------------------------
# LIBDIRS  : List of library directories (each must contain include and lib)
# INCLUDE  : -Idir for any additional dirs `dir` to search for headers
# ARCH     : Architecture flags to use when compiling or linking any sources
# ASFLAGS  : Flags to use when compiling assembly sources
# CFLAGS   : Flags to use when compiling C and C++ sources
# CXXFLAGS : Additional flags to use when compiling C++ sources only
# LDFLAGS  : Flags to use when linking object files
# LIBS     : -l linker flags for each library used
# LIBPATHS : Additional -L include path flags
#------------------------------------------------------------------------------

LIBDIRS   += $(LIBNDS) $(PORTLIBS)
INCLUDE   += $(foreach dir,$(LIBDIRS),-I$(dir)/include)
ifeq ($(strip $(ARCH)),)
  ARCH = -marm -mthumb-interwork -march=armv5te -mtune=arm946e-s
endif
ASFLAGS   += $(ARCH)
CFLAGS    += $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS  += $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS   += -specs=ds_arm9.specs $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS      += -lnds9
LIBPATHS  += $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

LD := $(CXX)

define bin2h
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > $@
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> $@
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> $@
endef

#------------------------------------------------------------------------------
# build rules
#------------------------------------------------------------------------------

%.a:
	@mkdir -p $(dir $@)
	rm -f $@
	$(AR) -rc $@ $^

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -MMD -MP -MF $(BUILD)/$*.d $(CXXFLAGS) -c $< -o $@ $(ERROR_FILTER)

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -MMD -MP -MF $(BUILD)/$*.d -x assembler-with-cpp $(ASFLAGS) -c $< -o $@ $(ERROR_FILTER)

$(BUILD)/%.wav.o: %.wav
	@mkdir -p $(dir $@)
	bin2s $< | $(AS) -o $@

$(BUILD)/%_wav.h: %.wav
	@mkdir -p $(dir $@)
	$(bin2h)

$(BUILD)/%.s $(BUILD)/%.h: %.png %.grit
	@mkdir -p $(dir $@)
	grit $< -fts -o$(BUILD)/$*
