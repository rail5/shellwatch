CXX      := g++
CXXFLAGS := -std=gnu++23 -O2 -s -Wall -Wextra -Wpedantic -MMD -MP
CXXFLAGS += $(shell wx-config --cxxflags)
LDFLAGS  := $(shell wx-config --libs core,base,richtext)

BUILDDIR := bin
OBJDIR   := $(BUILDDIR)/obj
SRCDIR   := src

TARGET   := $(BUILDDIR)/shellwatch
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGET) $(OBJECTS) $(OBJS:.o=.d)

.PHONY: clean

-include $(OBJS:.o=.d)
