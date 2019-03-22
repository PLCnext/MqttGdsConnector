cmake_minimum_required(VERSION 3.13)

project(GdsConnector)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

################# create target #######################################################

set (WILDCARD_SOURCE *.cpp)
set (WILDCARD_HEADER *.h *.hpp *.hxx)

file(GLOB_RECURSE Headers src/${WILDCARD_HEADER} intermediate/code/${WILDCARD_HEADER})
file(GLOB_RECURSE Sources src/${WILDCARD_SOURCE} intermediate/code/${WILDCARD_SOURCE})
add_library(GdsConnector SHARED ${Headers} ${Sources})

#######################################################################################

################# project include-paths ###############################################

target_include_directories(GdsConnector
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/intermediate/code>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_PREFIX_PATH}/include>
    $<BUILD_INTERFACE:${CMAKE_PREFIX_PATH}/include/nlohmann>
    )

#######################################################################################

################# include arp cmake module path #######################################

list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH  "${MQTTCLIENTLOCATION}/lib/cmake")
list(APPEND CMAKE_MODULE_PATH  "${MQTTCLIENTLOCATION}/Release/lib/cmake")
#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################

target_link_options(GdsConnector PRIVATE LINKER:--no-undefined)

#######################################################################################

################# add link targets ####################################################

find_package(ArpDevice REQUIRED)
find_package(ArpProgramming REQUIRED)

find_library(Mqtt_Client
            NAMES
            libPxceTcs.Mqtt.Client.so
            PxceTcs.Mqtt.Client
            PATHS
            ${CMAKE_PREFIX_PATH}/lib
            ${CMAKE_PREFIX_PATH}/Release/lib
            ${CMAKE_CURRENT_SOURCE_DIR}
            NO_CMAKE_FIND_ROOT_PATH)
message(STATUS "${Mqtt_Client}")
target_link_libraries(GdsConnector PRIVATE ArpDevice ArpProgramming ${Mqtt_Client})

#######################################################################################

################# install ############################################################

string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
install(TARGETS GdsConnector
    LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)
unset(_ARP_SHORT_DEVICE_VERSION)

#######################################################################################
