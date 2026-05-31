# ============================================================
#  B.A.S.E.L. Engine
#  Just type: make
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall
LDFLAGS  := -lpthread

TARGET := BASEL
SRCS   := main.cpp Graph.cpp MinHeap.cpp FlowNetwork.cpp MapLoader.cpp \
           EVRoutePlanner.cpp FleetDispatcher.cpp SwarmController.cpp Utilities.cpp
OBJS   := $(SRCS:.cpp=.o)

OS := $(shell uname -s 2>/dev/null || echo Windows)

# ============================================================
#  Default: build and run
# ============================================================

.PHONY: all
all: $(TARGET)
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  B.A.S.E.L. Engine ready."
	@echo "  Pipe a JSON request to ./$(TARGET)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""

# ============================================================
#  Compile
# ============================================================

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "  ✓ Build complete → ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================
#  Clean
# ============================================================

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Clean."
