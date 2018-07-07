#------------------------------------------------------------------------------
# Custard Makefile
#------------------------------------------------------------------------------

TARGET    := custard
BUILD     := build

INCLUDE   := -I$(BUILD) -Iinclude
ARCH	  := -mthumb -mthumb-interwork -march=armv5te -mtune=arm946e-s
CFLAGS    := -Wall -O2 -fomit-frame-pointer -ffast-math
CXXFLAGS  := -std=c++17

include custard.mk
LIBS      :=

#------------------------------------------------------------------------------
# files
#------------------------------------------------------------------------------

CFILES    := $(shell find include -name '*.c')
CPPFILES  := $(shell find include -name '*.cpp')
SFILES    := $(shell find include -name '*.s')

OFILES    := $(CPPFILES:%.cpp=$(BUILD)/%.o)
OFILES    += $(CFILES:%.c=$(BUILD)/%.o)
OFILES    += $(SFILES:%.s=$(BUILD)/%.o)

#------------------------------------------------------------------------------
# targets
#------------------------------------------------------------------------------

lib/lib$(TARGET).a: $(OFILES)

.PHONY: docs clean

docs:
	rm -fr docs
	doxygen doxygen/Doxyfile
	mv doxygen/html docs

clean:
	rm -fr $(BUILD) lib

#------------------------------------------------------------------------------
# dependencies
#------------------------------------------------------------------------------
-include  $(OFILES:.o=.d)
#------------------------------------------------------------------------------
