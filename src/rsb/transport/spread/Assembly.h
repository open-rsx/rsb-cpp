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
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * Instances of this class store fragments of partially received,
 * fragmented notifications for later assembly.
 *
 * @author swrede
 */
class RSB_EXPORT Assembly {
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
class RSB_EXPORT AssemblyPool {
public:

    /**
     * Creates a new pool with specified settings. Pruning will not immediately
     * start with these settings. It has to be enabled explicitly using the
     * appropriate method calls.
     *
     * @param ageMs defines the max. allowed age of pooled fragments before they
     *              are pruned (s) > 0
     * @param pruningIntervalMs the interval to use for checking the age (ms) > 0
     * @throw std::domain_error 0 given for ageMs or pruningIntervalMs
     */
    explicit AssemblyPool(const unsigned int &ageS = 20,
            const unsigned int &pruningIntervalMs = 4000);

    ~AssemblyPool();

    /**
     * Tells whether the pool is currently pruning fragments or not. This method
     * is thread-safe.
     *
     * @return @c true if the pool is currently pruning, else @c false
     */
    bool isPruning() const;

    /**
     * Changes the pruning settings (enables or disables pruning) and waits
     * until the new settings are applied. This method is thread-safe.
     *
     * @param prune if @c true, start pruning if it is not yet running, if
     *        @c false, disable pruning if active
     */
    void setPruning(const bool &prune);

    /**
     * Adds a new notification to the pool and tries to join it with already
     * pooled parts. If a complete event notification is available after this
     * message, the joined body is returned and the all parts are removed
     * from the pool.
     *
     * @param notification notification to add to the pool
     * @return if a joined message is ready, the contents are returned, else a
     *         0 pointer
     */
    boost::shared_ptr<std::string> add(
            rsb::protocol::NotificationPtr notification);

private:
    typedef std::map<std::string, boost::shared_ptr<Assembly> > Pool;

    class PruningTask: public rsc::threading::PeriodicTask {
    public:

                PruningTask(Pool &pool, boost::recursive_mutex &poolMutex,
                        const unsigned int &ageS,
                        const unsigned int &pruningIntervalMs);

        void execute();
    private:
        rsc::logging::LoggerPtr logger;
        Pool &pool;
        boost::recursive_mutex &poolMutex;
        unsigned int maxAge;
    };

    rsc::logging::LoggerPtr logger;
    Pool pool;
    boost::recursive_mutex poolMutex;

    const unsigned int pruningAgeS;
    const unsigned int pruningIntervalMs;

    rsc::threading::ThreadedTaskExecutor executor;
    mutable boost::recursive_mutex pruningMutex;
    rsc::threading::TaskPtr pruningTask;
};

typedef boost::shared_ptr<AssemblyPool> AssemblyPoolPtr;

}
}
