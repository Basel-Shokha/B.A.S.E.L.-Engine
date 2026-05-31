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
	@echo "║  Starting engine — drop a request.json file      ║"
	@echo "║  in this folder to trigger a solver.             ║"
	@echo "║                                                  ║"
	@echo "║  Modes: f1 = EV Route                            ║"
	@echo "║         f2 = Fleet Dispatch                      ║"
	@echo "║         f3 = Swarm Control                       ║"
	@echo "╚══════════════════════════════════════════════════╝"
	@echo ""
	./$(TARGET)

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
