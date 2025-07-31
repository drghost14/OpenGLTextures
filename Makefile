# ──────────────────────────────────────────────────────────────────────────────
# Project      : OpenGL 3D Learning with GLM (Static ENet & Raylib)
# Makefile     : Version 1.6
# Author       : Muhammad Hassnain Khichi (aka drghost)
# Description  : Statically links ENet (libenet.a) and Raylib (libraylib.a)
# ──────────────────────────────────────────────────────────────────────────────

#────────────────────────────────────────
# Compiler / Linker Settings
#────────────────────────────────────────
CXX       := g++
CXXFLAGS  := -m64 -O2 -Wall -std=c++23 -Iinclude

#────────────────────────────────────────
# Librarian & Static Flags
#────────────────────────────────────────
# -static            -> fully static binary
# -static-libstdc++  -> libstdc++ statically
# -static-libgcc     -> libgcc statically
# -Bstatic/-Bdynamic -> switch for .a archives
LDFLAGS := \
	-m64 \
	-static \
	-static-libstdc++ \
	-static-libgcc \
	-Llib \
	-Wl,-Bstatic \
		-l:libenet.a \
		-l:libraylib.a \
	-Wl,-Bdynamic \
		-lgdi32 \
		-lwinmm \
		-lopengl32 \
		-lws2_32 \
	-Wl,-subsystem,console


#────────────────────────────────────────
# Parallelism
#────────────────────────────────────────
CORES     := $(shell nproc 2>/dev/null || echo 4)
JOBS      ?= $(CORES)

#────────────────────────────────────────
# Sources & Objects
#────────────────────────────────────────
SRCS      := $(wildcard \
               src/*.cpp \
               src/*/*.cpp \
               src/*/*/*.cpp \
               src/*/*/*/*.cpp \
               src/*/*/*/*/*.cpp)
OBJS      := $(SRCS:.cpp=.o)

#────────────────────────────────────────
# Executable
#────────────────────────────────────────
TARGET    := main.exe

#────────────────────────────────────────
# Phony Targets
#────────────────────────────────────────
.PHONY: all run pr pr_run strip clean clear

#────────────────────────────────────────
# Default: build + strip (.o removal)
#────────────────────────────────────────
all: $(TARGET) strip

#────────────────────────────────────────
# Run without rebuilding
#────────────────────────────────────────
run: $(TARGET)
	@echo "→ Running $(TARGET)…"
	@./$(TARGET)

#────────────────────────────────────────
# Parallel build (then strip)
#────────────────────────────────────────
pr:
	@echo "→ Building with $(JOBS) jobs…"
	@$(MAKE) -j$(JOBS) all

#────────────────────────────────────────
# Parallel build + run
#────────────────────────────────────────
pr_run: pr
	@echo "→ Launching $(TARGET)…"
	@./$(TARGET)

#────────────────────────────────────────
# Link Rule
#────────────────────────────────────────
$(TARGET): $(OBJS)
	@echo "→ Linking $@ statically…"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

#────────────────────────────────────────
# Compile step
#────────────────────────────────────────
%.o: %.cpp
	@echo "→ Compiling $<…"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#────────────────────────────────────────
# Strip object files only
#────────────────────────────────────────
strip:
	@echo "→ Removing object files…"
	@rm -f $(OBJS)

#────────────────────────────────────────
# Full clean (objects + executable)
#────────────────────────────────────────
clean:
	@echo "→ Cleaning all build artifacts…"
	@rm -f $(OBJS) $(TARGET)

#────────────────────────────────────────
# Manual .o file cleanup only
#────────────────────────────────────────
clear:
	@echo "→ Manually removing only object files…"
	@rm -f $(OBJS)

#────────────────────────────────────────
# Package: build + strip symbols + copy to dist/
#────────────────────────────────────────
package: all
	@echo "→ Preparing package…"
	@mkdir -p dist
	@echo "→ Stripping debugging symbols from $(TARGET)…"
	@strip $(TARGET)
	@cp $(TARGET) dist/
	@echo "→ Package created in dist/"

#────────────────────────────────────────
# Package + Run
#────────────────────────────────────────
package_run: package
	@echo "→ Running packaged executable…"
	@./dist/$(TARGET)

#────────────────────────────────────────
# Delete the dist/ folder (packaged files)
#────────────────────────────────────────
package_clear:
	@echo "→ Removing package folder dist/…"
	@rm -rf dist
