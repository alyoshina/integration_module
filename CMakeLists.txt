cmake_minimum_required(VERSION 3.24.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(integration_module VERSION ${PROJECT_VERSION})

#configure_file(version.h.in version.h)

find_package(Qt6 REQUIRED COMPONENTS Core SerialBus Sql Test)
find_package(Qt6OpcUa)

qt_standard_project_setup()

qt_add_executable(integration_module 
    sources/main.cpp
    sources/data_utils.cpp
    sources/db_service.cpp
    sources/event_object.cpp
    sources/modbus_device.cpp
    sources/opc_device.cpp
    sources/worker_object.cpp
    headers/data_utils.h
    headers/db_service.h
    headers/event_object.h
    headers/idevice.h
    headers/modbus_device.h
    headers/opc_device.h
    headers/worker_object.h
)

target_include_directories(integration_module PRIVATE
    "${CMAKE_BINARY_DIR}"
    sources
    headers
)

target_link_libraries(integration_module PRIVATE 
	Qt6::Core
	Qt6::SerialBus
	Qt6::Sql
	Qt6::Test
    Qt6::OpcUa
)