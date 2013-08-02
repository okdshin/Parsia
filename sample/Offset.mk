CXX = g++ -std=gnu++11
CXXFLAGS = -Wall -g -D OFFSET_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = Offset.o
PROGRAM = Offset.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
