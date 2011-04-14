
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol ${CMAKE_CURRENT_SOURCE_DIR}/rsb)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES ${RSBPROTO_FILES}
                      PROTOROOT ${RSBPROTO_ROOT}
                      OUTPATH ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol)

CONFIGURE_FILE(rsb/rsbexports.h.in ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h DESTINATION include/rsb)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol/Notification.pb.h DESTINATION include/rsb/protocol)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol/Attachment.pb.h DESTINATION include/rsb/protocol)

CONFIGURE_FILE(rsb/Version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp @ONLY)

SET(SOURCES rsb/Exception.cpp
            rsb/CommException.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp
            rsb/Informer.cpp
            rsb/Listener.cpp
            rsb/util/Configuration.cpp
            rsb/util/ConfigException.cpp
            rsb/Event.cpp
            rsb/Handler.cpp
            rsb/Subscription.cpp
            rsb/EventProcessingStrategy.cpp
            rsb/QualityOfServiceSpec.cpp
            rsb/QueuePushHandler.cpp
            rsb/UnsupportedQualityOfServiceException.cpp
            rsb/Factory.cpp
            rsb/filter/Filter.cpp
            rsb/filter/ScopeFilter.cpp
            rsb/filter/FilterObservable.cpp
            rsb/filter/FilterObserver.cpp
            rsb/patterns/RemoteServer.cpp
            rsb/patterns/Server.cpp
            rsb/patterns/MethodExistsException.cpp
            rsb/protocol/ProtocolException.cpp
            rsb/transport/Connector.cpp
            rsb/transport/Router.cpp
            rsb/transport/Converter.cpp
            rsb/transport/Repository.cpp
            rsb/transport/SerializationException.cpp
            rsb/transport/converter/converters.cpp
            rsb/transport/converter/BoolConverter.cpp
            rsb/transport/converter/StringConverter.cpp
            rsb/transport/converter/Uint64Converter.cpp
            rsb/transport/converter/VoidConverter.cpp
            rsb/transport/Factory.cpp
            rsb/transport/inprocess/StatusTask.cpp
            rsb/transport/inprocess/InProcessConnector.cpp
            rsb/transport/spread/SpreadGroup.cpp
            rsb/transport/spread/MembershipManager.cpp
            rsb/transport/spread/SpreadConnector.cpp
            rsb/transport/spread/SpreadMessage.cpp
            rsb/transport/spread/SpreadConnection.cpp
            rsb/transport/spread/ReceiverTask.cpp
            rsb/introspection/IntrospectionConverter.cpp
            rsb/introspection/introspection.cpp
            ${PROTO_SOURCES}
            ${INTERNAL_PROTO_SOURCES})

SET(HEADERS rsb/Version.h
            rsb/Exception.h
            rsb/CommException.h
            rsb/util/Configuration.h
            rsb/util/ConfigException.h
            rsb/Event.h
            rsb/Informer.h
            rsb/Listener.h
            rsb/Action.h
            rsb/Handler.h
            rsb/Subscription.h
            rsb/EventProcessingStrategy.h
            rsb/QualityOfServiceSpec.h
            rsb/QueuePushHandler.h
            rsb/UnsupportedQualityOfServiceException.h
            rsb/Factory.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/Filter.h
            rsb/filter/ScopeFilter.h
            rsb/filter/FilterObservable.h
            rsb/filter/FilterObserver.h
            rsb/patterns/RemoteServer.h
            rsb/patterns/Server.h
            rsb/patterns/MethodExistsException.h
            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h
            rsb/transport/Connector.h
            rsb/transport/Router.h
            rsb/transport/Converter.h
            rsb/transport/Repository.h
            rsb/transport/SerializationException.h
            rsb/transport/converter/converters.h
            rsb/transport/converter/BoolConverter.h
            rsb/transport/converter/StringConverter.h
            rsb/transport/converter/Uint64Converter.h
            rsb/transport/converter/VoidConverter.h
            rsb/transport/converter/ProtocolBufferConverter.h
            rsb/transport/inprocess/StatusTask.h
            rsb/transport/inprocess/InProcessConnector.h
            rsb/transport/Factory.h
            rsb/transport/spread/SpreadGroup.h
            rsb/transport/spread/MembershipManager.h
            rsb/transport/spread/SpreadConnector.h
            rsb/transport/spread/SpreadMessage.h
            rsb/transport/spread/SpreadConnection.h
            rsb/transport/spread/ReceiverTask.h
            rsb/introspection/PortStateChange.h
            rsb/introspection/IntrospectionConverter.h
            rsb/introspection/introspection.h)

ADD_LIBRARY(${RSB_NAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h)
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
TARGET_LINK_LIBRARIES(${RSB_NAME} ${LIBS_LIBRARIES}
                                  ${Boost_LIBRARIES}
                                  ${SPREAD_LIBRARIES}
                                  ${PROTOBUF_LIBRARIES}
                                  ${RSC_LIBRARIES}
                                  ${CMAKE_THREAD_LIBS_INIT})
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
