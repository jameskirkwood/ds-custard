#------------------------------------------------------------------------------
# Custard Makefile
#------------------------------------------------------------------------------

TARGET    := custard
SOURCES   := source
BUILD     := build

INCLUDE   := $(foreach dir,$(SOURCES),-I$(dir)) -I$(BUILD) -Iinclude
ARCH	  := -mthumb -mthumb-interwork -march=armv5te -mtune=arm946e-s
CFLAGS    := -Wall -O2 -fomit-frame-pointer -ffast-math
CXXFLAGS  := -std=c++17

include custard.mk
LIBS      :=

#------------------------------------------------------------------------------
# files
#------------------------------------------------------------------------------

CFILES    := $(shell find $(SOURCES) -name '*.c')
CPPFILES  := $(shell find $(SOURCES) -name '*.cpp')
SFILES    := $(shell find $(SOURCES) -name '*.s')

OFILES    := $(CPPFILES:%.cpp=$(BUILD)/%.o)
OFILES    += $(CFILES:%.c=$(BUILD)/%.o)
OFILES    += $(SFILES:%.s=$(BUILD)/%.o)

DOXY_OUT  := $(build)/Doxyfile

#------------------------------------------------------------------------------
# targets
#------------------------------------------------------------------------------

lib/lib$(TARGET).a: $(OFILES)

.PHONY: docs clean

docs:
	rm -fr docs
	mkdir -p $(DOXY_OUT)
	doxygen Doxyfile
	mv $(DOXY_OUT)/html docs

clean:
	rm -fr $(BUILD) lib

#------------------------------------------------------------------------------
# dependencies
#------------------------------------------------------------------------------
-include  $(OFILES:.o=.d)
#------------------------------------------------------------------------------
