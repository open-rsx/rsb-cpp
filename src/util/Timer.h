/*
 * Timer.h
 *
 *  Created on: 28.07.2010
 *      Author: swrede
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>
#include <string>
#include <boost/shared_ptr.hpp>

namespace rsb {

namespace util {

class Timer {
public:
    Timer();
    Timer(std::string id);
    virtual ~Timer();
    void start();
    void stop();
    void reset();
    // return elapsed time in milliseconds
    double getElapsed();
    std::string getName();

private:
    std::string name;
    bool running;
    struct timeval tv_start, tv_end;
    struct timezone tz;

};

typedef boost::shared_ptr<Timer> TimerPtr;

}

}

#endif /* TIMER_H_ */
