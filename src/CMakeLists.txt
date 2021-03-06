include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/rsb/protocol
                           ${CMAKE_SOURCE_DIR}/src/rsb/protocol
                           ${CMAKE_SOURCE_DIR}/src/rsb)

set(RSBPROTO_FILES "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/collections/EventsByScopeMap.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/EventId.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/EventMetaData.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/FragmentedNotification.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/introspection/Bye.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/introspection/Hello.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/Notification.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/operatingsystem/Host.proto"
                   "${CMAKE_SOURCE_DIR}/protocol/rsb/protocol/operatingsystem/Process.proto")
protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS
                      PROTOFILES   ${RSBPROTO_FILES}
                      PROTOROOT    "${CMAKE_SOURCE_DIR}/protocol"
                      EXPORT_MACRO RSB_PROTOCOL_EXPORT
                      OUTPATH      ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "." "_" RSB_EXPORTS_NAME ${LIB_NAME})
configure_file(rsb/rsbexports.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h
               @ONLY)

# version
include(PadString)
pad_string(MAJOR_PADDED 2 "0" ${RSB_VERSION_MAJOR})
pad_string(MINOR_PADDED 2 "0" ${RSB_VERSION_MINOR})
pad_string(PATCH_PADDED 2 "0" ${RSB_VERSION_PATCH})
set(RSB_VERSION_NUMERIC "${MAJOR_PADDED}${MINOR_PADDED}${PATCH_PADDED}")
configure_file(rsb/Version.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp
               @ONLY)
configure_file(rsb/Version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.h
               @ONLY)

set(SOURCES rsb/CommException.cpp
            rsb/Event.cpp
            rsb/EventId.cpp
            rsb/Exception.cpp
            rsb/Factory.cpp
            rsb/CreateFunctions.cpp
            rsb/Handler.cpp
            rsb/Informer.cpp
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
            rsb/converter/Repository.cpp
            rsb/converter/RegexConverterPredicate.cpp
            rsb/converter/SchemaAndByteArrayConverter.cpp
            rsb/converter/SerializationException.cpp
            rsb/converter/StringConverter.cpp
            rsb/converter/TypeNameConverterPredicate.cpp
            rsb/converter/IntegerConverter.cpp
            rsb/converter/FloatingPointConverter.cpp
            rsb/converter/VoidConverter.cpp
            rsb/converter/ScopeConverter.cpp
            rsb/converter/converters.cpp

            rsb/eventprocessing/Handler.cpp
            rsb/eventprocessing/EventSendingStrategy.cpp
            rsb/eventprocessing/EventSendingStrategyFactory.cpp
            rsb/eventprocessing/DirectEventSendingStrategy.cpp
            rsb/eventprocessing/EventReceivingStrategy.cpp
            rsb/eventprocessing/EventReceivingStrategyFactory.cpp
            rsb/eventprocessing/DirectEventReceivingStrategy.cpp
            rsb/eventprocessing/ParallelEventReceivingStrategy.cpp
            rsb/eventprocessing/InRouteConfigurator.cpp
            rsb/eventprocessing/OutRouteConfigurator.cpp
            rsb/eventprocessing/strategies.cpp

            rsb/filter/Filter.cpp
            rsb/filter/FilterObserver.cpp
            rsb/filter/ScopeFilter.cpp
            rsb/filter/OriginFilter.cpp
            rsb/filter/MethodFilter.cpp
            rsb/filter/TypeFilter.cpp
            rsb/filter/CauseFilter.cpp

            rsb/patterns/MethodExistsException.cpp
            rsb/patterns/Reader.cpp
            rsb/patterns/Server.cpp
            rsb/patterns/RemoteServer.cpp
            rsb/patterns/LocalServer.cpp

            rsb/protocol/FragmentedNotification.cpp
            rsb/protocol/Notification.cpp
            rsb/protocol/ProtocolException.cpp

            rsb/transport/AsioServiceContext.cpp
            rsb/transport/Connector.cpp
            rsb/transport/Factory.cpp
            rsb/transport/OutConnector.cpp
            rsb/transport/InConnector.cpp
            rsb/transport/transports.cpp

            rsb/transport/inprocess/Bus.cpp
            rsb/transport/inprocess/InConnector.cpp
            rsb/transport/inprocess/OutConnector.cpp

            rsb/util/EventQueuePushHandler.cpp
            rsb/util/MD5.cpp
            rsb/util/QueuePushHandler.cpp

            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.cpp

            ${PROTO_SOURCES})

set(HEADERS rsb/CommException.h
            rsb/Event.h
            rsb/EventCollections.h
            rsb/EventId.h
            rsb/Exception.h
            rsb/Factory.h
            rsb/CreateFunctions.h
            rsb/Handler.h
            rsb/Informer.h
            rsb/Listener.h
            rsb/MetaData.h
            rsb/Participant.h
            rsb/ParticipantConfig.h
            rsb/QualityOfServiceSpec.h
            rsb/Scope.h
            rsb/UnsupportedQualityOfServiceException.h

            rsb/converter/BoolConverter.h
            rsb/converter/ByteArrayConverter.h
            rsb/converter/Converter.h
            rsb/converter/ConverterSelectionStrategy.h
            rsb/converter/EventsByScopeMapConverter.h
            rsb/converter/EventIdConverter.h
            rsb/converter/PredicateConverterList.h
            rsb/converter/ProtocolBufferConverter.h
            rsb/converter/UnambiguousConverterMap.h
            rsb/converter/RegexConverterPredicate.h
            rsb/converter/Repository.h
            rsb/converter/SchemaAndByteArrayConverter.h
            rsb/converter/SerializationException.h
            rsb/converter/StringConverter.h
            rsb/converter/TypeNameConverterPredicate.h
            rsb/converter/IntegerConverter.h
            rsb/converter/FloatingPointConverter.h
            rsb/converter/VoidConverter.h
            rsb/converter/ScopeConverter.h
            rsb/converter/RosettaConverter.h
            rsb/converter/converters.h

            rsb/eventprocessing/Handler.h
            rsb/eventprocessing/EventSendingStrategy.h
            rsb/eventprocessing/EventSendingStrategyFactory.h
            rsb/eventprocessing/DirectEventSendingStrategy.h
            rsb/eventprocessing/EventReceivingStrategy.h
            rsb/eventprocessing/EventReceivingStrategyFactory.h
            rsb/eventprocessing/DirectEventReceivingStrategy.h
            rsb/eventprocessing/ParallelEventReceivingStrategy.h
            rsb/eventprocessing/InRouteConfigurator.h
            rsb/eventprocessing/OutRouteConfigurator.h
            rsb/eventprocessing/strategies.h
            rsb/eventprocessing/ScopeDispatcher.h

            rsb/filter/Filter.h
            rsb/filter/FilterActionTypes.h
            rsb/filter/FilterObserver.h
            rsb/filter/ScopeFilter.h
            rsb/filter/OriginFilter.h
            rsb/filter/MethodFilter.h
            rsb/filter/TypeFilter.h
            rsb/filter/CauseFilter.h

            rsb/patterns/MethodExistsException.h
            rsb/patterns/Reader.h
            rsb/patterns/Server.h
            rsb/patterns/RemoteServer.h
            rsb/patterns/LocalServer.h

            rsb/protocol/FragmentedNotification.h
            rsb/protocol/Notification.h
            rsb/protocol/ProtocolException.h

            rsb/transport/AsioServiceContext.h
            rsb/transport/Connector.h
            rsb/transport/InConnector.h
            rsb/transport/OutConnector.h
            rsb/transport/ConverterSelectingConnector.h
            rsb/transport/Factory.h
            rsb/transport/transports.h

            rsb/transport/inprocess/Bus.h
            rsb/transport/inprocess/InConnector.h
            rsb/transport/inprocess/OutConnector.h

            rsb/util/EventQueuePushHandler.h
            rsb/util/MD5.h
            rsb/util/QueuePushHandler.h

            ${CMAKE_CURRENT_BINARY_DIR}/rsb/Version.h
            ${CMAKE_CURRENT_BINARY_DIR}/rsb/rsbexports.h

            ${PROTO_HEADERS})

if(WITH_SOCKET_TRANSPORT)
    list(APPEND SOURCES rsb/transport/socket/Types.cpp
                        rsb/transport/socket/BusConnection.cpp
                        rsb/transport/socket/Bus.cpp
                        rsb/transport/socket/BusImpl.cpp
                        rsb/transport/socket/BusServer.cpp
                        rsb/transport/socket/BusServerImpl.cpp
                        rsb/transport/socket/Factory.cpp
                        rsb/transport/socket/ConnectorBase.cpp
                        rsb/transport/socket/InConnector.cpp
                        rsb/transport/socket/LifecycledBusServer.cpp
                        rsb/transport/socket/OutConnector.cpp
                        rsb/transport/socket/Serialization.cpp)
    list(APPEND HEADERS rsb/transport/socket/Types.h
                        rsb/transport/socket/BusConnection.h
                        rsb/transport/socket/Bus.h
                        rsb/transport/socket/BusImpl.h
                        rsb/transport/socket/BusServer.h
                        rsb/transport/socket/BusServerImpl.h
                        rsb/transport/socket/Factory.h
                        rsb/transport/socket/ConnectorBase.h
                        rsb/transport/socket/InConnector.h
                        rsb/transport/socket/LifecycledBusServer.h
                        rsb/transport/socket/OutConnector.h
                        rsb/transport/socket/Serialization.h)
endif()

add_library(${LIB_NAME} SHARED ${SOURCES} ${HEADERS} ${PROTO_HEADERS})
target_link_libraries(${LIB_NAME} ${PROTOBUF_LIBRARIES}
                                  ${RSC_LIBRARIES}
                                  ${Boost_LIBRARIES}
                                  ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(${LIB_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})

# export the protocol classes on windows
if(WIN32)
    set_target_properties(${LIB_NAME}
                          PROPERTIES
                          COMPILE_DEFINITIONS "${PROTOCOL_EXPORT}")
endif()

# TODO why do we have to open sockets???
if(WIN32)
    target_link_libraries(${LIB_NAME} wsock32)
endif()

install(TARGETS ${LIB_NAME}
        EXPORT RSBDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install_files_recursive("include/${INSTALL_PATH_PREFIX}" HEADERS)

include(InstallDebugSymbols)
install_debug_symbols(TARGETS ${LIB_NAME})

add_subdirectory(rsb/introspection)
