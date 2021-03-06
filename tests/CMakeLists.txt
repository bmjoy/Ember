# Copyright (c) 2014 - 2018 Ember
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(EXECUTABLE_NAME unit_tests)

set(EXECUTABLE_SRC
    srp6.cpp
    BufferChain.cpp
    Buffer.cpp
    BinaryStream.cpp
    GruntHandler.cpp
    GruntProtocol.cpp
    LoginHandler.cpp
    Patcher.cpp
    IPBan.cpp
    )

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRC})
target_link_libraries(${EXECUTABLE_NAME} gtest gtest_main liblogin shared spark srp6 ${BOTAN_LIBRARY} ${Boost_LIBRARIES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ../src)
add_test(NAME unit_tests COMMAND unit_tests)
INSTALL(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})