# ============================================================
#  B.A.S.E.L. Engine
#  Just type: make
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -w
LDFLAGS  := -lpthread

TARGET := BASEL
SRCS   := main.cpp Graph.cpp MinHeap.cpp FlowNetwork.cpp MapLoader.cpp \
           EVRoutePlanner.cpp FleetDispatcher.cpp SwarmController.cpp Utilities.cpp
OBJS   := $(SRCS:.cpp=.o)

# ============================================================
#  Default: build and run
# ============================================================

.PHONY: all
all: $(TARGET)
	@echo ""
	@echo "╔══════════════════════════════════════════════════╗"
	@echo "║          B.A.S.E.L. Engine is ready.             ║"
	@echo "║                                                  ║"
	@echo "║  Feed it a JSON request:                         ║"
	@echo "║  echo '{...}' | ./BASEL                          ║"
	@echo "║  cat request.json | ./BASEL                      ║"
	@echo "╚══════════════════════════════════════════════════╝"
	@echo ""

# ============================================================
#  Compile
# ============================================================

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ============================================================
#  Clean
# ============================================================

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Clean."
