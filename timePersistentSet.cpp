/* -*-c++-*- */

#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/times.h>
#include <boost/unordered_set.hpp>

#include "PersistentSet.hpp"

using namespace odf::hash_trie;

hashType hashfun(int const val)
{
    return val;
}

typedef PersistentSet<int, hashfun> Set;

using boost::unordered_set;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::exception;

class Stopwatch
{
private:
    bool const useCpuTime_;

    long accumulated_;
    long start_;
    bool isRunning_;

    tms mutable tmsCurrent_;

    long time() const
    {
        clock_t res = times(&tmsCurrent_);
        if (res < 0)
            throw "Cannot determine user time.";

    	if (useCpuTime_)
            return tmsCurrent_.tms_utime;
        else
            return res;
    }

public:
    Stopwatch(bool useCpuTime = true)
        : useCpuTime_(useCpuTime),
          accumulated_(0),
          start_(0),
          isRunning_(false)
    {
    }
    
    string mode() const
    {
    	if (useCpuTime_)
            return "CPU";
        else
            return "Real";
    }
    
    void resume()
    {
        if (!isRunning_)
        {
            isRunning_ = true;
            start_ = time();
        }
    }

    void start()
    {
        accumulated_ = 0;
        isRunning_ = true;
        start_ = time();
    }
    
    void stop()
    {
        if (isRunning_)
        {
            accumulated_ += time() - start_;
            isRunning_ = false;
        }
    }
    
    /**
     * Reports the elapsed time on this timer in milliseconds.
     */
    long elapsed() const
    {
        static long clktck = 0;

        if (clktck == 0 and (clktck = sysconf(_SC_CLK_TCK)) <= 0)
            throw "Cannot determine system clock rate";

    	return (accumulated_ + (isRunning_ ? time() - start_ : 0))
            * 1000 / clktck;
    }
    
    string format() const
    {
        return format(elapsed());
    }
    
    static string format(long const milliseconds)
    {
        std::stringstream ss;
    	ss << milliseconds / 10 / 100.0 << " seconds";
        return ss.str();
    }
};


int main(int argc, char** argv)
{
    Stopwatch stopWatch;

    if (argc < 2)
    {
        cerr << "Missing argument: number of items to insert." << endl;
        return 1;
    }

    int const N = atoi(argv[1]);
    size_t* values = new size_t[N + N / 2];

    srand(123456789);

    for (int i = 0; i < N + N / 2; ++i)
    {
        values[i] = rand();
    }

    cerr << "Persistent set:" << endl;

    stopWatch.start();

    Set set;

    for (int i = 0; i < N; ++i)
        set = set.insert(values[i]);

    cerr << "  Time for " << N << " insertions: "
         << stopWatch.format() << endl;

    stopWatch.start();

    int countA = 0;
    for (int i = N / 2; i < N + N / 2; ++i)
    {
        countA += set.contains(values[i]);
    }

    cerr << "  Time for " << N << " queries:    "
         << stopWatch.format() << endl;

    stopWatch.start();

    Set copy = set;
    for (int i = 0; i < N; i += 2)
        copy = copy.remove(values[i]);

    cerr << "  Time for " << N / 2 << " removals:   "
         << stopWatch.format() << endl;

    cerr << endl;

    cerr << "Boost unordered_set:" << endl;

    stopWatch.start();

    unordered_set<int> bset;

    for (int i = 0; i < N; ++i)
        bset.insert(values[i]);


    cerr << "  Time for " << N << " insertions: "
         << stopWatch.format() << endl;

    stopWatch.start();

    int countB = 0;
    for (int i = N / 2; i < N + N / 2; ++i)
    {
        countB += (bset.count(values[i]) > 0);
    }

    cerr << "  Time for " << N << " queries:    "
         << stopWatch.format() << endl;

    stopWatch.start();

    for (int i = 0; i < N; i += 2)
        bset.erase(values[i]);

    cerr << "  Time for " << N / 2 << " removals:   "
         << stopWatch.format() << endl;

    if (countA != countB )
        cerr << "Counts don't match: "
             << countA << " for PersistentSet, "
             << countB << " for unordered_set."
             << endl;

    return 0;
}
