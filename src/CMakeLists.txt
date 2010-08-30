
INCLUDE_DIRECTORIES(. BEFORE)

SET(SOURCES rsb/RSBException.cpp 
            rsb/RSBVersion.cpp
            rsb/Publisher.cpp
            rsb/Subscriber.cpp
            rsb/util/UUID.cpp
            rsb/util/Configuration.cpp
            rsb/util/Timer.cpp
            rsb/util/Task.cpp
            rsb/util/PeriodicTask.cpp
            rsb/util/TaskExecutor.cpp
            rsb/RSBEvent.cpp
            rsb/Subscription.cpp
            rsb/MatchAndExecute.cpp
            rsb/EventProcessor.cpp
            rsb/filter/AbstractFilter.cpp
            rsb/filter/ScopeFilter.cpp
            rsb/filter/FilterObservable.cpp
            rsb/filter/FilterObserver.cpp
            rsb/protocol/ProtocolException.cpp
            rsb/transport/Port.cpp
            rsb/transport/Router.cpp
            rsb/transport/AbstractConverter.cpp
            rsb/transport/TransportFactory.cpp
            rsb/transport/QueueAndDispatchTask.cpp
            rsb/transport/converter/StringConverter.cpp
            rsb/transport/inprocess/StatusTask.cpp
            rsb/transport/inprocess/InProcessPort.cpp
            rsb/transport/spread/SpreadGroup.cpp
            rsb/transport/spread/MembershipManager.cpp
            rsb/transport/spread/SpreadPort.cpp
            rsb/transport/spread/SpreadMessage.cpp
            rsb/transport/spread/SpreadConnection.cpp
            rsb/transport/spread/ReceiverTask.cpp
            rsb/introspection/IntrospectionConverter.cpp
            ../test/InformerTask.cpp
            ../test/BTL2StringConverter.cpp
            ${PROTO_SOURCES})

SET(HEADERS rsb/RSBVersion.h
            rsb/RSBException.h 
            rsb/CommException.h
            rsb/util/UUID.h
            rsb/util/Configuration.h
            rsb/util/ConfigException.h
            rsb/util/Timer.h
            rsb/util/Task.h
            rsb/util/PeriodicTask.h
            rsb/util/TaskExecutor.h
            rsb/RSBEvent.h
            rsb/Publisher.h
            rsb/Subscriber.h
            rsb/Action.h
            rsb/Handler.h
            rsb/Subscription.h
            rsb/MatchAndExecute.h
            rsb/EventProcessor.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/AbstractFilter.h
            rsb/filter/ScopeFilter.h
            rsb/filter/FilterObservable.h
            rsb/filter/FilterObserver.h
            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h
            rsb/transport/Port.h
            rsb/transport/Router.h
            rsb/transport/converter/StringConverter.h
            rsb/transport/inprocess/StatusTask.h
            rsb/transport/inprocess/InProcessPort.h
            rsb/transport/TransportFactory.h
            rsb/transport/AbstractConverter.h
            rsb/transport/QueueAndDispatchTask.h
            rsb/transport/spread/SpreadGroup.h
            rsb/transport/spread/MembershipManager.h
            rsb/transport/spread/SpreadPort.h
            rsb/transport/spread/SpreadMessage.h
            rsb/transport/spread/SpreadConnection.h
            rsb/transport/spread/ReceiverTask.h
            rsb/introspection/PortStateChange.h
            rsb/introspection/IntrospectionConverter.h)

ADD_LIBRARY(${LIBNAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS})
TARGET_LINK_LIBRARIES(${LIBNAME} ${LIBS_LIBRARIES} ${Boost_LIBRARIES} ${SPREAD_LIBRARIES} ${PROTOBUF_LIBRARIES})
INSTALL(TARGETS ${LIBNAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
INSTALL_FILES_RECURSIVE(include HEADERS)
