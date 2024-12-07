cmake_minimum_required(VERSION 3.28)
project(mqtt_multi_thread)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/ken/Qt/6.7.2/gcc_64")

find_package(Qt6 COMPONENTS
        Core
        Mqtt
        Network
        REQUIRED)

add_executable(mqtt_multi_thread
        main.cpp

        MqttReadThread.cpp
        MqttWriteThread.cpp
        MqttThreadConnector.cpp
        MqttThreadPool.cpp
)

target_link_libraries(mqtt_multi_thread
        Qt::Core
        Qt::Mqtt
        Qt::Network
)

