CXXWARNS = -Wall -Wextra -pedantic
CXXOPTS  = -g -O3
CXXFLAGS = $(CXXWARNS) $(CXXOPTS)
PROGRAMS = testPersistentMap timePersistentMap timePersistentSet testList

all:	$(PROGRAMS)

testPersistentMap:	testPersistentMap.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lUnitTest++

timePersistentMap:	timePersistentMap.o
	$(CXX) $(CXXFLAGS) $^ -o $@

timePersistentSet:	timePersistentSet.o
	$(CXX) $(CXXFLAGS) $^ -o $@

testList:	testList.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lgmp -lm -lUnitTest++

clean:
	rm -f *.o Makefile.bak

distclean:	clean
	rm -f $(PROGRAMS)

depend:
	makedepend -Y. \
	    testPersistentMap.cpp timePersistentMap.cpp \
	    timePersistentSet.cpp testList.cpp

# DO NOT DELETE

testPersistentMap.o: PersistentMap.hpp hash_trie.hpp
timePersistentMap.o: PersistentMap.hpp hash_trie.hpp
timePersistentSet.o: PersistentSet.hpp hash_trie.hpp
testList.o: Integer.h shared_array.hpp List.hpp Thunk.hpp nullstream.hpp
testList.o: fun.hpp list_fun.hpp
