cmake_minimum_required(VERSION 3.28)
project(ibmscppbackend)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(CMAKE_PREFIX_PATH "/home/leo/Qt/Tools/CMake")
set(CMAKE_PREFIX_PATH "/home/ken/Qt/6.7.2/gcc_64")

INCLUDE_DIRECTORIES("/usr/local/include/net-snmp")
LINK_DIRECTORIES("/usr/local/lib")

find_package(Qt6 COMPONENTS
        Core
        Mqtt
        Network
        REQUIRED)

add_executable(ibmscppbackend
        main.cpp

        MqttReadThread.cpp
        MqttWriteThread.cpp
#        RestRequest.cpp
        MqttThreadConnector.cpp
        MqttThreadPool.cpp
        SnmpGet.cpp
        SnmpThreadConnector.cpp
#        SnmpReadThread.cpp
#        SnmpWriteThread.cpp
)

target_link_libraries(ibmscppbackend
        Qt::Core
        Qt::Mqtt
        Qt::Network
        netsnmp
)

