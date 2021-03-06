# Copyright (C) 2016 Istituto Italiano di Tecnologia iCub Facility & RBCS Department
# Authors: Valentina Vasco
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set(LIBRARY_TARGET_NAME gazebo_assistiverehab_rpc_tuginterface)

yarp_idl_to_dir(INPUT_FILES tuginterface_rpc.thrift
                OUTPUT_DIR ${PROJECT_SOURCE_DIR}/build
                SOURCES_VAR TIRPC_SRC
                HEADERS_VAR TIRPC_HEADERS
                INCLUDE_DIRS_VAR TIRPC_INCLUDES)

add_library(${LIBRARY_TARGET_NAME} SHARED ${TIRPC_SRC} ${TIRPC_HEADERS})
target_link_libraries(${LIBRARY_TARGET_NAME} ${YARP_LIBRARIES})

set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES PUBLIC_HEADER "${TIRPC_HEADERS}")

target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC
                           "$<BUILD_INTERFACE:${TIRPC_INCLUDES}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/gazebo>")

set_property(TARGET ${LIBRARY_TARGET_NAME} PROPERTY PUBLIC_HEADER "${TIRPC_HEADERS}")

install(TARGETS ${LIBRARY_TARGET_NAME}
        EXPORT gazebo_assistiverehab_tuginterface
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gazebo)

set_property(GLOBAL APPEND PROPERTY ${gazebo_assistiverehab_tuginterface}_TARGETS ${LIBRARY_TARGET_NAME})
