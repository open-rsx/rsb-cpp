#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/PeriodicTask.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include "../../protocol/Notification.h"

namespace rsb {
namespace spread {

/**
 * Instances of this class store fragments of partially received,
 * fragmented notifications for later assembly.
 *
 * @author swrede
 */
class Assembly {
public:

    Assembly(rsb::protocol::NotificationPtr n);
    ~Assembly();

    std::string getData(const unsigned int &i) const;

    std::string *getCompleteData() const;

    unsigned int add(rsb::protocol::NotificationPtr n);

    bool isComplete() const;

    /**
     * Age of the assembly as seconds. The age is the elaped time since this
     * instance was created.
     *
     * @return age in seconds
     */
    unsigned int age() const;

private:
    rsc::logging::LoggerPtr logger;
    unsigned int receivedParts;
    std::vector<rsb::protocol::NotificationPtr> store;
    boost::posix_time::ptime birthTime;
};

typedef boost::shared_ptr<Assembly> AssemblyPtr;

/**
 * Instances of this class maintain a pool of ongoing @ref Assembly
 * s. In addition to adding arriving notification fragments to these,
 * the ages of assemblies are monitor and old assemblies are pruned.
 *
 * @author jmoringe
 */
class AssemblyPool {
public:
    AssemblyPool();

    ~AssemblyPool();

    boost::shared_ptr<std::string> add(rsb::protocol::NotificationPtr notification);

private:
    typedef std::map<std::string, boost::shared_ptr<Assembly> > Pool;

    class PruningTask : public rsc::threading::PeriodicTask {
    public:
        PruningTask(Pool &pool, boost::recursive_mutex &poolMutex);

        void execute();
    private:
        rsc::logging::LoggerPtr logger;
        Pool &pool;
        boost::recursive_mutex &poolMutex;
    };

    rsc::logging::LoggerPtr logger;
    Pool pool;
    boost::recursive_mutex poolMutex;

    rsc::threading::ThreadedTaskExecutor executor;
    rsc::threading::TaskPtr pruningTask;
};

typedef boost::shared_ptr<AssemblyPool> AssemblyPoolPtr;

}
}
