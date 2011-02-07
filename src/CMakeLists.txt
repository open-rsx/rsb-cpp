
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES ${RSBPROTO_FILES}
                      PROTOROOT ${RSBPROTO_ROOT}
                      OUTPATH ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol)

CONFIGURE_FILE(rsb/rsbexports.h.in ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h DESTINATION include/rsb)

SET(SOURCES rsb/RSBException.cpp 
            rsb/RSBVersion.cpp
            rsb/Publisher.cpp
            rsb/Subscriber.cpp
            rsb/util/Configuration.cpp
            rsb/RSBEvent.cpp
            rsb/Subscription.cpp
            rsb/EventProcessor.cpp
            rsb/QualityOfServiceSpec.cpp
            rsb/QueuePushHandler.cpp
            rsb/UnsupportedQualityOfServiceException.cpp
            rsb/filter/AbstractFilter.cpp
            rsb/filter/ScopeFilter.cpp
            rsb/filter/FilterObservable.cpp
            rsb/filter/FilterObserver.cpp
            rsb/patterns/RemoteServer.cpp
            rsb/patterns/Server.cpp
            rsb/protocol/ProtocolException.cpp
            rsb/transport/Port.cpp
            rsb/transport/Router.cpp
            rsb/transport/AbstractConverter.cpp
            rsb/transport/converter/BoolConverter.cpp
            rsb/transport/converter/StringConverter.cpp
            rsb/transport/converter/Uint64Converter.cpp
            rsb/transport/converter/VoidConverter.cpp
            rsb/transport/TransportFactory.cpp
            rsb/transport/QueueAndDispatchTask.cpp
            rsb/transport/inprocess/StatusTask.cpp
            rsb/transport/inprocess/InProcessPort.cpp
            rsb/transport/spread/SpreadGroup.cpp
            rsb/transport/spread/MembershipManager.cpp
            rsb/transport/spread/SpreadPort.cpp
            rsb/transport/spread/SpreadMessage.cpp
            rsb/transport/spread/SpreadConnection.cpp
            rsb/transport/spread/ReceiverTask.cpp
            rsb/introspection/IntrospectionConverter.cpp
            ${PROTO_SOURCES}
            ${INTERNAL_PROTO_SOURCES})

SET(HEADERS rsb/RSBVersion.h
            rsb/RSBException.h 
            rsb/CommException.h
            rsb/util/Configuration.h
            rsb/util/ConfigException.h
            rsb/RSBEvent.h
            rsb/Publisher.h
            rsb/Subscriber.h
            rsb/Action.h
            rsb/Handler.h
            rsb/Subscription.h
            rsb/EventProcessor.h
            rsb/QualityOfServiceSpec.h
            rsb/QueuePushHandler.h
            rsb/UnsupportedQualityOfServiceException.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/AbstractFilter.h
            rsb/filter/ScopeFilter.h
            rsb/filter/FilterObservable.h
            rsb/filter/FilterObserver.h
            rsb/patterns/RemoteServer.h
            rsb/patterns/Server.h
            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h
            rsb/transport/Port.h
            rsb/transport/Router.h
            rsb/transport/AbstractConverter.h
            rsb/transport/converter/BoolConverter.h
            rsb/transport/converter/StringConverter.h
            rsb/transport/converter/Uint64Converter.h
            rsb/transport/converter/VoidConverter.h
            rsb/transport/inprocess/StatusTask.h
            rsb/transport/inprocess/InProcessPort.h
            rsb/transport/TransportFactory.h
            rsb/transport/QueueAndDispatchTask.h
            rsb/transport/spread/SpreadGroup.h
            rsb/transport/spread/MembershipManager.h
            rsb/transport/spread/SpreadPort.h
            rsb/transport/spread/SpreadMessage.h
            rsb/transport/spread/SpreadConnection.h
            rsb/transport/spread/ReceiverTask.h
            rsb/introspection/PortStateChange.h
            rsb/introspection/IntrospectionConverter.h)

ADD_LIBRARY(${RSB_NAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h)
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
TARGET_LINK_LIBRARIES(${RSB_NAME} ${LIBS_LIBRARIES} ${Boost_LIBRARIES} ${SPREAD_LIBRARIES} ${PROTOBUF_LIBRARIES} ${RSC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
SET_TARGET_PROPERTIES(${RSB_NAME}
                      PROPERTIES
                      VERSION ${RSB_VERSION})
# TODO why do we have to open sockets???
IF(WIN32)
	TARGET_LINK_LIBRARIES(${RSB_NAME} wsock32)
ENDIF()

INSTALL(TARGETS ${RSB_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
INSTALL_FILES_RECURSIVE(include HEADERS)

# --- cppcheck ---

GENERATE_CPPCHECK(SOURCES ${SOURCES}
                          ${HEADERS}
                          ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h
                  ENABLE_IDS style)
