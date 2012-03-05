/* -*-c++-*- */

#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/times.h>
#include <boost/unordered_map.hpp>

#include "PersistentMap.hpp"

using namespace odf::hash_trie;

hashType hashfun(int const val)
{
    return val;
}

typedef PersistentMap<int, int, hashfun> Map;

using boost::unordered_map;
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
    
    void start()
    {
        if (!isRunning_)
        {
            start_ = time();
            isRunning_ = true;
        }
    }
    
    void stop()
    {
        if (isRunning_)
        {
            accumulated_ += time() - start_;
            isRunning_ = false;
        }
    }
    
    void reset()
    {
    	accumulated_ = 0;
    	if (isRunning_)
        {
    		start_ = time();
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

    int const N = atoi(argv[1]);
    size_t* values = new size_t[N];

    srand(123456789);

    for (int i = 0; i < N; ++i)
    {
        values[i] = rand();
    }

    cerr << "Persistent map:" << endl;

    stopWatch.reset();
    stopWatch.start();

    Map map;

    for (int i = 0; i < N; ++i)
        map = map.insert(values[i], i);

    stopWatch.stop();
    cerr << "  Time for " << N << " insertions: "
         << stopWatch.format() << endl;

    stopWatch.reset();
    stopWatch.start();

    double sumA = 0.0;
    for (int i = N / 2; i < N; ++i)
    {
        sumA += map.getVal(values[i], 0);
    }

    stopWatch.stop();
    cerr << "  Time for " << N/2 << " queries:    "
         << stopWatch.format() << endl;

    stopWatch.reset();
    stopWatch.start();

    Map copy = map;
    for (int i = 0; i < N; i += 2)
        copy = copy.remove(values[i]);

    stopWatch.stop();
    cerr << "  Time for " << N/2 << " removals:   "
         << stopWatch.format() << endl;

    cerr << endl;


    cerr << "Boost unordered_map:" << endl;

    stopWatch.reset();
    stopWatch.start();

    unordered_map<int, int> bmap;

    for (int i = 0; i < N; ++i)
        bmap[values[i]] = i;

    stopWatch.stop();

    cerr << "  Time for " << N << " insertions: "
         << stopWatch.format() << endl;

    stopWatch.reset();
    stopWatch.start();

    double sumB = 0.0;
    for (int i = N / 2; i < N; ++i)
    {
        sumB += bmap.at(values[i]);
    }

    stopWatch.stop();
    cerr << "  Time for " << N/2 << " queries:    "
         << stopWatch.format() << endl;

    stopWatch.reset();
    stopWatch.start();

    for (int i = 0; i < N; i += 2)
        bmap.erase(values[i]);

    stopWatch.stop();
    cerr << "  Time for " << N/2 << " removals:   "
         << stopWatch.format() << endl;

    if (sumA != sumB)
        cerr << "Sums don't match!" << endl;

    return 0;
}
