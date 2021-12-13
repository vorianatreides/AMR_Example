EXECUTABLE = OrderOptimizer
SOURCES    = $(wildcard *.cpp)
OBJECTS    = $(patsubst %,%,${SOURCES:.cpp=.o})
CXX        = g++
CXXFLAGS   = -Wall -g -c -std=c++11 -pthread -o
LDFLAGS    = -static-libstdc++ -pthread
LDLIBS     =
#-------------------------------------------------------------------------------

#make executable
all: $(EXECUTABLE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $@ $< -MMD -MF ./$@.d

#link Objects
$(EXECUTABLE) : $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

#make clean
clean:
	rm -f ./*.o
	rm -f ./*.o.d
	rm -f $(EXECUTABLE)

.PHONY: clean

#The dependencies:
-include $(wildcard *.d)
