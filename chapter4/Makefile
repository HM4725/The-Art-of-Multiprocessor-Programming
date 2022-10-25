# project name
TARGET   = run

CC       = g++
CFLAGS   = -std=c++14 -Wall -I./include

LINKER   = g++
LFLAGS   = -Wall -I./include -lm -pthread

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

MD       = mkdir -p
RM       = rm -f


$(BINDIR)/$(TARGET): clean $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(MD) $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(RM) $(OBJECTS)
	@$(RM) $(BINDIR)/$(TARGET)
	@echo "Cleanup complete!"
