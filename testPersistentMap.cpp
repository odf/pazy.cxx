/* -*-c++-*- */

#include <unittest++/UnitTest++.h>

#include "PersistentMap.hpp"

using namespace odf::hash_trie;


SUITE(BitTwiddling)
{
    TEST(MaskedFunction)
    {
        CHECK_EQUAL(0x18, masked(0x12345678,  0));
        CHECK_EQUAL(0x13, masked(0x12345678,  5));
        CHECK_EQUAL(0x15, masked(0x12345678, 10));
        CHECK_EQUAL(0x08, masked(0x12345678, 15));
        CHECK_EQUAL(0x03, masked(0x12345678, 20));
    }

    TEST(BitCountFunction)
    {
        CHECK_EQUAL(13, bitCount(0x12345678));
        CHECK_EQUAL(32, bitCount(0xffffffff));
        CHECK_EQUAL( 1, bitCount(0x80000000));
        CHECK_EQUAL( 0, bitCount(0x00000000));
        CHECK_EQUAL( 2, bitCount(0x00100400));
    }

    TEST(IndexForBitFunction)
    {
        CHECK_EQUAL( 3, indexForBit(0x01010101, 0x01000000));
        CHECK_EQUAL( 2, indexForBit(0x01010101, 0x00010000));
        CHECK_EQUAL(18, indexForBit(0xffffffff, 0x00040000));
        CHECK_EQUAL(18, indexForBit(0xfffbffff, 0x00040000));
    }
}

SUITE(ArrayCopyOnWrite)
{
    int in[] = { 0, 1, 2, 3 };

    TEST(ArrayUpdateFunction)
    {
        for (int k = 0; k < 4; ++k)
        {
            const int* out = arrayUpdate(in, 4, k, 5);
            
            for (int i = 0; i < 4; ++i)
                CHECK_EQUAL(i, in[i]);

            for (int i = 0; i < 4; ++i)
                CHECK_EQUAL(((i == k) ? 5 : i), out[i]);

            delete[] out;
        }
    }

    TEST(ArrayInsertFunction)
    {
        for (int k = 0; k < 4; ++k)
        {
            const int* out = arrayInsert(in, 4, k, 5);
            
            for (int i = 0; i < 4; ++i)
                CHECK_EQUAL(i, in[i]);
            
            for (int i = 0; i < 5; ++i)
                CHECK_EQUAL(((i < k) ? i : (i == k) ? 5 : i - 1), out[i]);

            delete[] out;
        }
    }

    TEST(ArrayRemoveFunction)
    {
        for (int k = 0; k < 4; ++k)
        {
            const int* out = arrayRemove(in, 4, k);
            
            for (int i = 0; i < 4; ++i)
                CHECK_EQUAL(i, in[i]);
            
            for (int i = 0; i < 3; ++i)
                CHECK_EQUAL(((i < k) ? i : i + 1), out[i]);

            delete[] out;
        }
    }

    TEST(EmptyArray)
    {
        const int* x = arrayInsert((int *) 0, 0, 0, 1);
        CHECK_EQUAL(1, x[0]);

        delete[] x;
    }
}

SUITE(PersistentMap)
{
#define CHECK_MISSING(key, map) CHECK_EQUAL(None, (map).get(key).get())

    const void* None = 0;

    SUITE(EightBitHash)
    {
        hashType hashfun(int const val)
        {
            return val % 256;
        }

        typedef PersistentMap<int, int, hashfun> Map;

        TEST(SimpleTest)
        {
            Map map =
                Map().insert(1, 2).insert(3, 4).insert(259, 5).insert(1027, 6);

            CHECK_EQUAL(2, *map.get(   1));
            CHECK_EQUAL(4, *map.get(   3));
            CHECK_EQUAL(5, *map.get( 259));
            CHECK_EQUAL(6, *map.get(1027));

            CHECK_MISSING(123, map);
        }

        TEST(TwoInOneOut)
        {
            Map map = Map().insert('A', 1).insert('B', 2).remove('A');

            CHECK_EQUAL(1, map.size());
            CHECK_EQUAL(2, *map.get('B'));
            CHECK_MISSING('A', map);
        }

        TEST(EmptyMap)
        {
            Map map;

            CHECK_EQUAL(0, map.size());
            CHECK_MISSING(0, map);
            CHECK_EQUAL(0, map.remove(0).size());
            CHECK_EQUAL("PersistentMap({})", map.asString());
        }

        TEST(SingletonMap)
        {
            Map map = Map().insert('A', 1);

            CHECK_EQUAL(1, map.size());
            CHECK_EQUAL(1, *map.get('A'));
            CHECK_MISSING('B', map);
            CHECK_EQUAL(0, map.remove('A').remove('A').size());
            CHECK_EQUAL("PersistentMap(65 -> 1)", map.asString());
        }

        TEST(SingletonMapUpdate)
        {
            Map map = Map().insert('A', 1).insert('A', 65);

            CHECK_EQUAL(1, map.size());
            CHECK_EQUAL(65, *map.get('A'));
            CHECK_MISSING('B', map);
        }

        SUITE(TwoItemsLevelOneCollision)
        {
            int const key_a = 1;
            int const key_b = 33;
            int const key_c = 5;

            Map map = Map().insert(key_a, 'a').insert(key_b, 'b');

            TEST(Basic)
            {
                CHECK_EQUAL(2, map.size());
                CHECK_EQUAL('a', *map.get(key_a));
                CHECK_EQUAL('a', *map.get(key_a));
                CHECK_MISSING(key_c, map);
                CHECK_EQUAL("PersistentMap({1: {0: 1 -> 97, 1: 33 -> 98}})",
                            map.asString());

                CHECK_EQUAL(1, map.remove(key_a).size());
                CHECK_EQUAL(0, map.remove(key_a).remove(key_b).size());
            }

            TEST(RemoveNonMember)
            {
                Map mod = map.remove(key_c);

                CHECK_EQUAL(2, mod.size());
                CHECK_EQUAL('a', *mod.get(key_a));
                CHECK_EQUAL('a', *mod.get(key_a));
                CHECK_MISSING(key_c, mod);
            }
        }

        SUITE(ThreeItemsFullCollision)
        {
            int const key_a = 257;
            int const key_b = 513;
            int const key_c = 769;
            int const key_d = 33;

            Map map =
                Map().insert(key_a, 'a').insert(key_b, 'b').insert(key_c, 'c');

            TEST(Basic)
            {
                CHECK_EQUAL("PersistentMap(<257 -> 97 | 513 -> 98 | 769 -> 99>)",
                            map.asString());
                CHECK_EQUAL(3, map.size());
                CHECK_EQUAL('a', *map.get(key_a));
                CHECK_EQUAL('b', *map.get(key_b));
                CHECK_EQUAL('c', *map.get(key_c));
            }

            TEST(RemoveOneItem)
            {
                Map mod = map.remove(key_a);

                CHECK_EQUAL(2, mod.size());
                CHECK_EQUAL('b', *mod.get(key_b));
                CHECK_EQUAL('c', *mod.get(key_c));
            }

            TEST(AddOneItem)
            {
                Map mod = map.insert(key_d, 'd');

                CHECK_EQUAL(4, mod.size());
                CHECK_EQUAL('a', *mod.get(key_a));
                CHECK_EQUAL('b', *mod.get(key_b));
                CHECK_EQUAL('c', *mod.get(key_c));
                CHECK_EQUAL('d', *mod.get(key_d));
            }
        }

        TEST(WildMix)
        {
            Map map;
            int* keys = new int[17];
            for (int i = 0; i < 17; ++i)
            {
                keys[i] = i * 5 + 7;
                map = map.insert(keys[i], 10 * keys[i]);
            }

            CHECK_EQUAL(17, map.size());
            for (int i = 0; i < 17; ++i)
                CHECK_EQUAL(10 * keys[i], *map.get(keys[i]));

            delete[] keys;
        }

        SUITE(HundredsOfEntries)
        {
            struct Fixture
            {
                Fixture()
                    : N(301)
                {
                    for (int i = 0; i < N; ++i)
                        baseMap = baseMap.insert(i, 10 * i);
                }

                int const N;
                Map baseMap;
            };
 
            TEST_FIXTURE(Fixture, Basic)
            {
                Map map = baseMap;

                CHECK_EQUAL(N, map.size());
                for (int i = 0; i < N; ++i)
                    CHECK_EQUAL(10 * i, *map.get(i));
                CHECK_MISSING(N+1, map);
            }

            TEST_FIXTURE(Fixture, RemoveSome)
            {
                Map map = baseMap;

                int const M = 101;
                for (int i = 0; i < M; ++i)
                    map = map.remove(i);

                CHECK_EQUAL(N - M, map.size());
                for (int i = 0; i < M; ++i)
                    CHECK_MISSING(i, map);
                for (int i = M; i < N; ++i)
                    CHECK_EQUAL(10 * i, *map.get(i));
            }

            TEST_FIXTURE(Fixture, RemoveSomeNonMembers)
            {
                Map map = baseMap;

                int const A = 1000;
                int const B = 1101;
                for (int i = A; i < B; ++i)
                    map = map.remove(i);

                CHECK_EQUAL(N, map.size());
                for (int i = 0; i < N; ++i)
                    CHECK_EQUAL(10 * i, *map.get(i));
                for (int i = A; i < B; ++i)
                    CHECK_MISSING(i, map);
            }

            TEST_FIXTURE(Fixture, RemoveAll)
            {
                Map map = baseMap;
                for (int i = 0; i < N; ++i)
                    map = map.remove(i);

                CHECK_EQUAL(0, map.size());
                for (int i = 0; i < N; ++i)
                    CHECK_MISSING(i, map);
            }

            TEST_FIXTURE(Fixture, UpdateSome)
            {
                Map map = baseMap;

                int const M = 101;
                for (int i = 0; i < M; ++i)
                    map = map.insert(i, 7 * i);

                CHECK_EQUAL(N, map.size());
                for (int i = 0; i < M; ++i)
                    CHECK_EQUAL(7 * i, *map.get(i));
                for (int i = M; i < N; ++i)
                    CHECK_EQUAL(10 * i, *map.get(i));
            }

            TEST_FIXTURE(Fixture, ReInsertSome)
            {
                Map map = baseMap;

                int const M = 101;
                for (int i = 0; i < M; ++i)
                    map = map.insert(i, 10 * i);

                CHECK_EQUAL(N, map.size());
                for (int i = 0; i < N; ++i)
                    CHECK_EQUAL(10 * i, *map.get(i));
            }
        }

        SUITE(ThousandsOfEntries)
        {
            int const N = 10000;

            TEST(InsertN)
            {
                Map map;
                for (int i = 0; i < N; ++i)
                {
                    map = map.insert(i, i);
                    CHECK_EQUAL(i + 1, map.size());
                }
                for (int i = 0; i < N; ++i)
                {
                    CHECK_EQUAL(i, *map.get(i));
                }
            }
        }        
    }

    SUITE(LongHash)
    {
        hashType hashfun(int const val)
        {
            return (hashType) val;
        }

        typedef PersistentMap<int, int, hashfun> Map;

        TEST(LowerBitCollisions)
        {
            int keys[] = {
                0x1fffffff,
                0x3fffffff,
                0x5fff0fff,
                0x7fff0fff,
                0x9fffffff,
                0xbfffffff,
                0xdfff0fff,
                0xffff0fff
            };

            int N = sizeof(keys) / sizeof(int);
            Map map;

            for (int i = 0; i < N; ++i)
                map = map.insert(keys[i], keys[i] >> 4);
            CHECK_EQUAL(N, map.size());
            for (int i = 0; i < N; ++i)
                CHECK_EQUAL(keys[i] >> 4, *map.get(keys[i]));

            for (int i = 1; i < N; ++i)
                map = map.remove(keys[i]);
            CHECK_EQUAL(1, map.size());
            CHECK_EQUAL(keys[0] >> 4, *map.get(keys[0]));
            CHECK_MISSING(keys[1], map);

            map = map.remove(keys[0]);
            CHECK_EQUAL(0, map.size());
            CHECK_MISSING(keys[0], map);
        }

        TEST(HigherBitCollisions)
        {
            int keys[] = {
                0x7ffffff1,
                0x7ffffff3,
                0x7fff0ff5,
                0x7fff0ff7
            };

            int N = sizeof(keys) / sizeof(int);
            Map map;

            for (int i = 0; i < N; ++i)
                map = map.insert(keys[i], keys[i] >> 4);
            CHECK_EQUAL(N, map.size());
            for (int i = 0; i < N; ++i)
                CHECK_EQUAL(keys[i] >> 4, *map.get(keys[i]));
            
            for (int i = 1; i < N; ++i)
                map = map.remove(keys[i]);
            CHECK_EQUAL(1, map.size());
            CHECK_EQUAL(keys[0] >> 4, *map.get(keys[0]));
            CHECK_MISSING(keys[1], map);

            map = map.remove(keys[0]);
            CHECK_EQUAL(0, map.size());
            CHECK_MISSING(keys[0], map);
        }
    }
}

int main()
{
    return UnitTest::RunAllTests();
}
