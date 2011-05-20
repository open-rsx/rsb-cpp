
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol ${CMAKE_CURRENT_SOURCE_DIR}/rsb)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES ${RSBPROTO_FILES}
                      PROTOROOT ${RSBPROTO_ROOT}
                      OUTPATH ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol)

ADD_LIBRARY(${SPREAD_PROTOCOL_NAME} STATIC ${PROTO_SOURCES})
IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    SET_TARGET_PROPERTIES(${SPREAD_PROTOCOL_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF()

# TODO why do we need to install them? they should be fully internal
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol/Notification.pb.h DESTINATION include/rsb/protocol)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol/Attachment.pb.h DESTINATION include/rsb/protocol)

CONFIGURE_FILE(rsb/rsbexports.h.in ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h DESTINATION include/rsb)
CONFIGURE_FILE(rsb/Version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp @ONLY)

SET(SOURCES rsb/CommException.cpp
            rsb/Event.cpp
            rsb/Exception.cpp
            rsb/Factory.cpp
            rsb/Handler.cpp
            rsb/Informer.cpp
            rsb/Listener.cpp
            rsb/LocalService.cpp
            rsb/Participant.cpp
            rsb/ParticipantConfig.cpp
            rsb/QualityOfServiceSpec.cpp
            rsb/QueuePushHandler.cpp
            rsb/Scope.cpp
            rsb/Service.cpp
            rsb/UnsupportedQualityOfServiceException.cpp

            rsb/converter/BoolConverter.cpp
            rsb/converter/Converter.cpp
            rsb/converter/Repository.cpp
            rsb/converter/SerializationException.cpp
            rsb/converter/StringConverter.cpp
            rsb/converter/Uint64Converter.cpp
            rsb/converter/VoidConverter.cpp
            rsb/converter/converters.cpp

            rsb/eventprocessing/Handler.cpp
            rsb/eventprocessing/EventSendingStrategy.cpp
            rsb/eventprocessing/DirectEventSendingStrategy.cpp
            rsb/eventprocessing/EventReceivingStrategy.cpp
            rsb/eventprocessing/ParallelEventReceivingStrategy.cpp
            rsb/eventprocessing/InRouteConfigurator.cpp
            rsb/eventprocessing/OutRouteConfigurator.cpp

            rsb/filter/Filter.cpp
            rsb/filter/FilterObserver.cpp
            rsb/filter/ScopeFilter.cpp

            rsb/introspection/IntrospectionConverter.cpp
            rsb/introspection/introspection.cpp

            rsb/patterns/MethodExistsException.cpp
            rsb/patterns/RemoteServer.cpp
            rsb/patterns/Server.cpp

            rsb/protocol/ProtocolException.cpp

            rsb/transport/Connector.cpp
            rsb/transport/InConnector.cpp
            rsb/transport/transports.cpp

            rsb/transport/inprocess/Bus.cpp
            rsb/transport/inprocess/InConnector.cpp
            rsb/transport/inprocess/OutConnector.cpp

            rsb/transport/spread/InConnector.cpp
            rsb/transport/spread/MembershipManager.cpp
            rsb/transport/spread/OutConnector.cpp
            rsb/transport/spread/ReceiverTask.cpp
            rsb/transport/spread/SpreadConnection.cpp
            rsb/transport/spread/SpreadConnector.cpp
            rsb/transport/spread/SpreadGroup.cpp
            rsb/transport/spread/SpreadMessage.cpp
            rsb/transport/spread/Assembly.cpp

            rsb/util/MD5.cpp

            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp)

SET(HEADERS rsb/CommException.h
            rsb/Event.h
            rsb/Exception.h
            rsb/Factory.h
            rsb/Handler.h
            rsb/Informer.h
            rsb/Listener.h
            rsb/LocalService.h
            rsb/Participant.h
            rsb/ParticipantConfig.h
            rsb/QualityOfServiceSpec.h
            rsb/QueuePushHandler.h
            rsb/Scope.h
            rsb/Service.h
            rsb/UnsupportedQualityOfServiceException.h
            rsb/Version.h

            rsb/converter/BoolConverter.h
            rsb/converter/Converter.h
            rsb/converter/ProtocolBufferConverter.h
            rsb/converter/UnambiguousConverterMap.h
            rsb/converter/Repository.h
            rsb/converter/SerializationException.h
            rsb/converter/StringConverter.h
            rsb/converter/Uint64Converter.h
            rsb/converter/VoidConverter.h
            rsb/converter/converters.h

            rsb/eventprocessing/Handler.h
            rsb/eventprocessing/EventSendingStrategy.h
            rsb/eventprocessing/DirectEventSendingStrategy.h
            rsb/eventprocessing/EventReceivingStrategy.h
            rsb/eventprocessing/ParallelEventReceivingStrategy.h
            rsb/eventprocessing/InRouteConfigurator.h
            rsb/eventprocessing/OutRouteConfigurator.h

            rsb/filter/Filter.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/FilterObserver.h
            rsb/filter/ScopeFilter.h

            rsb/introspection/IntrospectionConverter.h
            rsb/introspection/PortStateChange.h
            rsb/introspection/introspection.h

            rsb/patterns/MethodExistsException.h
            rsb/patterns/RemoteServer.h
            rsb/patterns/Server.h

            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h

            rsb/transport/Connector.h
            rsb/transport/InConnector.h
            rsb/transport/OutConnector.h
            rsb/transport/ConverterSelectingConnector.h
            rsb/transport/ConverterSelectingInConnector.h
            rsb/transport/ConverterSelectingOutConnector.h
            rsb/transport/Factory.h
            rsb/transport/transports.h

            rsb/transport/inprocess/Bus.h
            rsb/transport/inprocess/InConnector.h
            rsb/transport/inprocess/OutConnector.h

            rsb/transport/spread/InConnector.h
            rsb/transport/spread/MembershipManager.h
            rsb/transport/spread/OutConnector.h
            rsb/transport/spread/ReceiverTask.h
            rsb/transport/spread/SpreadConnection.h
            rsb/transport/spread/SpreadConnector.h
            rsb/transport/spread/SpreadGroup.h
            rsb/transport/spread/SpreadMessage.h
            rsb/transport/spread/Assembly.h

            rsb/util/MD5.h)

ADD_LIBRARY(${RSB_NAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h)
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
TARGET_LINK_LIBRARIES(${RSB_NAME} ${SPREAD_PROTOCOL_NAME}
                                  ${LIBS_LIBRARIES}
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
