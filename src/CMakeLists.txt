INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_SOURCE_DIR}/src/rsb/protocol
                           ${CMAKE_SOURCE_DIR}/src/rsb)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES   ${RSBPROTO_FILES}
                      PROTOROOT    ${RSBPROTO_ROOT}
                      EXPORT_MACRO RSB_PROTOCOL_EXPORT
                      OUTPATH      ${CMAKE_CURRENT_BINARY_DIR})

CONFIGURE_FILE(rsb/rsbexports.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h
               @ONLY)

# version
INCLUDE(PadString)
PAD_STRING(MAJOR_PADDED 2 "0" ${RSB_VERSION_MAJOR})
PAD_STRING(MINOR_PADDED 2 "0" ${RSB_VERSION_MINOR})
PAD_STRING(PATCH_PADDED 2 "0" ${RSB_VERSION_PATCH})
SET(RSB_VERSION_NUMERIC "${MAJOR_PADDED}${MINOR_PADDED}${PATCH_PADDED}")
CONFIGURE_FILE(rsb/Version.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp
               @ONLY)
CONFIGURE_FILE(rsb/Version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.h
               @ONLY)

SET(SOURCES rsb/CommException.cpp
            rsb/Event.cpp
            rsb/EventId.cpp
            rsb/Exception.cpp
            rsb/Factory.cpp
            rsb/CreateFunctions.cpp
            rsb/Handler.cpp
            rsb/Informer.cpp
            rsb/Reader.cpp
            rsb/Listener.cpp
            rsb/MetaData.cpp
            rsb/Participant.cpp
            rsb/ParticipantConfig.cpp
            rsb/QualityOfServiceSpec.cpp
            rsb/Scope.cpp
            rsb/UnsupportedQualityOfServiceException.cpp

            rsb/converter/BoolConverter.cpp
            rsb/converter/ByteArrayConverter.cpp
            rsb/converter/Converter.cpp
            rsb/converter/EventsByScopeMapConverter.cpp
            rsb/converter/EventIdConverter.cpp
            rsb/converter/Int64Converter.cpp
            rsb/converter/Repository.cpp
            rsb/converter/RegexConverterPredicate.cpp
            rsb/converter/SchemaAndByteArrayConverter.cpp
            rsb/converter/SerializationException.cpp
            rsb/converter/StringConverter.cpp
            rsb/converter/TypeNameConverterPredicate.cpp
            rsb/converter/Uint32Converter.cpp
            rsb/converter/Uint64Converter.cpp
            rsb/converter/VoidConverter.cpp
            rsb/converter/converters.cpp

            rsb/eventprocessing/Handler.cpp
            rsb/eventprocessing/EventSendingStrategy.cpp
            rsb/eventprocessing/EventSendingStrategyFactory.cpp
            rsb/eventprocessing/DirectEventSendingStrategy.cpp
            rsb/eventprocessing/EventReceivingStrategy.cpp
            rsb/eventprocessing/EventReceivingStrategyFactory.cpp
            rsb/eventprocessing/PullEventReceivingStrategy.cpp
            rsb/eventprocessing/DirectEventReceivingStrategy.cpp
            rsb/eventprocessing/ParallelEventReceivingStrategy.cpp
            rsb/eventprocessing/InRouteConfigurator.cpp
            rsb/eventprocessing/PullInRouteConfigurator.cpp
            rsb/eventprocessing/PushInRouteConfigurator.cpp
            rsb/eventprocessing/OutRouteConfigurator.cpp
            rsb/eventprocessing/strategies.cpp

            rsb/filter/Filter.cpp
            rsb/filter/FilterObserver.cpp
            rsb/filter/ScopeFilter.cpp
            rsb/filter/OriginFilter.cpp
            rsb/filter/MethodFilter.cpp

            rsb/patterns/MethodExistsException.cpp
            rsb/patterns/Server.cpp
            rsb/patterns/RemoteServer.cpp
            rsb/patterns/LocalServer.cpp

            rsb/protocol/FragmentedNotification.cpp
            rsb/protocol/Notification.cpp
            rsb/protocol/ProtocolException.cpp

            rsb/transport/Connector.cpp
            rsb/transport/Factory.cpp
            rsb/transport/OutConnector.cpp
            rsb/transport/InConnector.cpp
            rsb/transport/InPullConnector.cpp
            rsb/transport/InPushConnector.cpp
            rsb/transport/transports.cpp

            rsb/transport/inprocess/Bus.cpp
            rsb/transport/inprocess/InConnector.cpp
            rsb/transport/inprocess/InPullConnector.cpp
            rsb/transport/inprocess/InPushConnector.cpp
            rsb/transport/inprocess/OutConnector.cpp

            rsb/util/EventQueuePushHandler.cpp
            rsb/util/MD5.cpp
            rsb/util/QueuePushHandler.cpp

            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp

            ${PROTO_SOURCES})

SET(HEADERS rsb/CommException.h
            rsb/Event.h
            rsb/EventCollections.h
            rsb/EventId.h
            rsb/EventQueuePushHandler.h
            rsb/Exception.h
            rsb/Factory.h
            rsb/CreateFunctions.h
            rsb/Handler.h
            rsb/Informer.h
            rsb/Reader.h
            rsb/Listener.h
            rsb/MetaData.h
            rsb/Participant.h
            rsb/ParticipantConfig.h
            rsb/QualityOfServiceSpec.h
            rsb/QueuePushHandler.h
            rsb/Scope.h
            rsb/UnsupportedQualityOfServiceException.h

            rsb/converter/BoolConverter.h
            rsb/converter/ByteArrayConverter.h
            rsb/converter/Converter.h
            rsb/converter/ConverterSelectionStrategy.h
            rsb/converter/EventsByScopeMapConverter.h
            rsb/converter/EventIdConverter.h
            rsb/converter/Int64Converter.h
            rsb/converter/PredicateConverterList.h
            rsb/converter/ProtocolBufferConverter.h
            rsb/converter/UnambiguousConverterMap.h
            rsb/converter/RegexConverterPredicate.h
            rsb/converter/Repository.h
            rsb/converter/SchemaAndByteArrayConverter.h
            rsb/converter/SerializationException.h
            rsb/converter/StringConverter.h
            rsb/converter/TypeNameConverterPredicate.h
            rsb/converter/Uint32Converter.h
            rsb/converter/Uint64Converter.h
            rsb/converter/VoidConverter.h
            rsb/converter/RosettaConverter.h
            rsb/converter/converters.h

            rsb/eventprocessing/Handler.h
            rsb/eventprocessing/EventSendingStrategy.h
            rsb/eventprocessing/EventSendingStrategyFactory.h
            rsb/eventprocessing/DirectEventSendingStrategy.h
            rsb/eventprocessing/EventReceivingStrategy.h
            rsb/eventprocessing/EventReceivingStrategyFactory.h
            rsb/eventprocessing/PullEventReceivingStrategy.h
            rsb/eventprocessing/PushEventReceivingStrategy.h
            rsb/eventprocessing/DirectEventReceivingStrategy.h
            rsb/eventprocessing/ParallelEventReceivingStrategy.h
            rsb/eventprocessing/InRouteConfigurator.h
            rsb/eventprocessing/PullInRouteConfigurator.h
            rsb/eventprocessing/PushInRouteConfigurator.h
            rsb/eventprocessing/OutRouteConfigurator.h
            rsb/eventprocessing/strategies.h

            rsb/filter/Filter.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/FilterObserver.h
            rsb/filter/ScopeFilter.h
            rsb/filter/OriginFilter.h
            rsb/filter/MethodFilter.h

            rsb/patterns/MethodExistsException.h
            rsb/patterns/Server.h
            rsb/patterns/RemoteServer.h
            rsb/patterns/LocalServer.h

            rsb/protocol/FragmentedNotification.h
            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h

            rsb/transport/Connector.h
            rsb/transport/InConnector.h
            rsb/transport/InPullConnector.h
            rsb/transport/InPushConnector.h
            rsb/transport/OutConnector.h
            rsb/transport/ConverterSelectingConnector.h
            rsb/transport/Factory.h
            rsb/transport/transports.h

            rsb/transport/inprocess/Bus.h
            rsb/transport/inprocess/InConnector.h
            rsb/transport/inprocess/InPullConnector.h
            rsb/transport/inprocess/InPushConnector.h
            rsb/transport/inprocess/OutConnector.h

            rsb/util/EventQueuePushHandler.h
            rsb/util/MD5.h
            rsb/util/QueuePushHandler.h

            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.h
            ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h

            ${PROTO_HEADERS})

IF(WITH_SOCKET_TRANSPORT)
    LIST(APPEND SOURCES rsb/transport/socket/Types.cpp
                        rsb/transport/socket/BusConnection.cpp
                        rsb/transport/socket/Bus.cpp
                        rsb/transport/socket/BusServer.cpp
                        rsb/transport/socket/Factory.cpp
                        rsb/transport/socket/ConnectorBase.cpp
                        rsb/transport/socket/InConnector.cpp
                        rsb/transport/socket/InPushConnector.cpp
                        rsb/transport/socket/InPullConnector.cpp
                        rsb/transport/socket/OutConnector.cpp
                        rsb/transport/socket/Serialization.cpp)
    LIST(APPEND HEADERS rsb/transport/socket/Types.h
                        rsb/transport/socket/BusConnection.h
                        rsb/transport/socket/Bus.h
                        rsb/transport/socket/BusServer.h
                        rsb/transport/socket/Factory.h
                        rsb/transport/socket/ConnectorBase.h
                        rsb/transport/socket/InConnector.h
                        rsb/transport/socket/InPushConnector.h
                        rsb/transport/socket/InPullConnector.h
                        rsb/transport/socket/OutConnector.h
                        rsb/transport/socket/Serialization.h)
ENDIF()

IF(WIN32)
    ADD_DEFINITIONS("/DRSB_PROTOCOL_EXPORT=__declspec(dllexport)")
ELSE()
    ADD_DEFINITIONS("-DRSB_PROTOCOL_EXPORT=")
ENDIF()

ADD_LIBRARY(${LIB_NAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${PROTOBUF_LIBRARIES}
                                  ${RSC_LIBRARIES}
                                  ${Boost_LIBRARIES}
                                  ${CMAKE_THREAD_LIBS_INIT})
SET_TARGET_PROPERTIES(${LIB_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})
# TODO why do we have to open sockets???
IF(WIN32)
    TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF()

INSTALL(TARGETS ${LIB_NAME}
        EXPORT RSBDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
INSTALL_FILES_RECURSIVE("include/${INSTALL_PATH_PREFIX}" HEADERS)
