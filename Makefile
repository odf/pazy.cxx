CXXWARNS = -Wall -Wextra -pedantic
CXXOPTS  = -g -O3
CXXFLAGS = $(CXXWARNS) $(CXXOPTS)
PROGRAMS = testPersistentMap timePersistentMap timePersistentSet

testPersistentMap:	testPersistentMap.o
	$(CXX) $(CXXFLAGS) testPersistentMap.o -o testPersistentMap -lUnitTest++

timePersistentMap:	timePersistentMap.o
	$(CXX) $(CXXFLAGS) timePersistentMap.o -o timePersistentMap

timePersistentSet:	timePersistentSet.o
	$(CXX) $(CXXFLAGS) $^ -o $@

all:	$(PROGRAMS)

clean:
	rm -f *.o Makefile.bak

distclean:	clean
	rm -f $(PROGRAMS)

depend:
	makedepend -Y. \
	    testPersistentMap.cpp timePersistentMap.cpp \
	    timePersistentSet.cpp

# DO NOT DELETE

testPersistentMap.o: PersistentMap.hpp hash_trie.hpp
timePersistentMap.o: PersistentMap.hpp hash_trie.hpp
timePersistentSet.o: PersistentSet.hpp hash_trie.hpp
