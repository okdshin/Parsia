CXX = g++ -std=gnu++11
CXXFLAGS = -Wall -g -D SYNTAXRULE_UNIT_TEST
INCLUDES = 
LIBS = -lboost_regex
OBJS = SyntaxRule.o
PROGRAM = SyntaxRule.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
