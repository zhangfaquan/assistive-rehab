# Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)
# All Rights Reserved.
# Authors: Ugo Pattacini <ugo.pattacini@iit.it>

if(CER_FOUND)
  project(robotSkeletonPublisher)

  yarp_add_idl(IDL_GEN_FILES src/idl.thrift)

  set(doc_files ${PROJECT_NAME}.xml)

  source_group("IDL Files" FILES src/idl.thrift)
  source_group("DOC Files" FILES ${doc_files})

  add_executable(${PROJECT_NAME} src/main.cpp src/idl.thrift ${IDL_GEN_FILES} ${doc_files})
  target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
  target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} iKin CER::cer_kinematics AssistiveRehab)
  install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
