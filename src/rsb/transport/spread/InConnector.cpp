/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "InConnector.h"

#include <rsc/threading/ThreadedTaskExecutor.h>

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;
using namespace rsc::threading;

namespace rsb {
namespace spread {

InConnector::InConnector(const string &host, unsigned int port) :
    logger(Logger::getLogger("rsb.spread.InConnector")),
    active(false),
    connector(new SpreadConnector(host, port)) {
    this->exec = TaskExecutorPtr(new ThreadedTaskExecutor);
    // TODO check if it makes sense and is possible to provide a weak_ptr to the ctr of StatusTask
    //st = boost::shared_ptr<StatusTask>(new StatusTask(this));
    this->rec = boost::shared_ptr<ReceiverTask>(
        new ReceiverTask(this->connector->getConnection(),
                         this->connector->getConverters(),
                         this->observer));
}

rsb::transport::InConnector* InConnector::create(const Properties& args) {
    static LoggerPtr logger = Logger::getLogger("rsb.spread.InConnector");
    RSCDEBUG(logger, "creating InConnector with properties " << args);

    return new InConnector(args.get<string>      ("host", defaultHost()),
                           args.get<unsigned int>("port", defaultPort()));
}

InConnector::~InConnector() {
    if (this->active) {
        deactivate();
    }
}

void InConnector::activate() {
    this->connector->activate();

    // (re-)start threads
    this->exec->schedule(rec);
    //this->exec->schedule(st);
    this->active = true;
}

void InConnector::deactivate() {
    this->rec->cancel();
    // killing spread connection, exception thrown to rec thread which
    // should be handled specifically as the cancel flag was set
    this->connector->deactivate();
    this->rec->waitDone();
    this->active = false;
}

void InConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &specs) {
    this->connector->setQualityOfServiceSpecs(specs);
}

void InConnector::setObserver(HandlerPtr observer) {
    this->observer = observer;
    this->rec->setHandler(observer);
}

void InConnector::notify(rsb::filter::ScopeFilter* f,
                         const rsb::filter::FilterAction::Types &at) {
    // join or leave groups
    // TODO evaluate success
    RSCDEBUG(logger, "notify(rsb::filter::ScopeFilter*, ...) entered");
    switch (at) {
    case rsb::filter::FilterAction::ADD:
        RSCINFO(logger, "ScopeFilter scope is " << f->getScope()
                << " ,now going to join Spread group")
            ;
        this->connector->join(f->getScope().toString());
        break;
    case rsb::filter::FilterAction::REMOVE:
        RSCINFO(logger, "ScopeFilter scope is " << f->getScope()
                << " ,now going to leave Spread group")
            ;
        this->connector->leave(f->getScope().toString());
        break;
    default:
        RSCWARN(logger,
                "ScopeFilter Action not supported by this Connector implementation")
            ;
        break;
    }
}

}
}
