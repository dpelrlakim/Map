CXX = g++-5
CXXFLAGS = -std=c++14 -Wall -MMD
EXEC = runSuite
OBJECTS = runSuite.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
