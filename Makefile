CXXWARNS = -Wall -Wextra -pedantic
CXXOPTS  = -g -O3
CXXFLAGS = $(CXXWARNS) $(CXXOPTS)
PROGRAMS = testPersistentMap timePersistentMap timePersistentSet testList

all:	$(PROGRAMS)

testPersistentMap:	test/testPersistentMap.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lUnitTest++

timePersistentMap:	test/timePersistentMap.o
	$(CXX) $(CXXFLAGS) $^ -o $@

timePersistentSet:	test/timePersistentSet.o
	$(CXX) $(CXXFLAGS) $^ -o $@

testList:		test/testList.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lgmp -lm -lUnitTest++

clean:
	rm -f *.o test/*.o Makefile.bak

distclean:	clean
	rm -f $(PROGRAMS)

depend:
	makedepend -Y. \
	    test/testPersistentMap.cpp test/timePersistentMap.cpp \
	    test/timePersistentSet.cpp test/testList.cpp

# DO NOT DELETE

test/testPersistentMap.o: PersistentMap.hpp hash_trie.hpp
test/timePersistentMap.o: PersistentMap.hpp hash_trie.hpp
test/timePersistentSet.o: PersistentSet.hpp hash_trie.hpp
test/testList.o: Integer.h shared_array.hpp List.hpp Thunk.hpp nullstream.hpp
test/testList.o: fun.hpp list_fun.hpp
