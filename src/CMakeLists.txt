# src/CMakeLists.txt

add_library(multi_cast_core
    Log/log.cpp
    Reactor/Reactor.cpp
    Tcp/ClientSession.cpp
    Tcp/TcpManager.cpp
    Timer/TimerManager.cpp
    Multicast/MulticastManager.cpp
)

add_library(Tcp_client
    TcpClient/Tcp_client.cpp)

target_include_directories(multi_cast_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Application
        ${CMAKE_CURRENT_SOURCE_DIR}/Log
        ${CMAKE_CURRENT_SOURCE_DIR}/Reactor
        ${CMAKE_CURRENT_SOURCE_DIR}/Tcp
        ${CMAKE_CURRENT_SOURCE_DIR}/Timer
        ${CMAKE_CURRENT_SOURCE_DIR}/Multicast
)

target_include_directories(Tcp_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/TcpClient
        ${CMAKE_CURRENT_SOURCE_DIR}/Log
)

target_compile_features(multi_cast_core PUBLIC cxx_std_17)

target_link_libraries(multi_cast_core
    PUBLIC spdlog::spdlog
)

# Place all built executables in the top-level build/bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(Tcp_client
    PUBLIC spdlog::spdlog
)

add_executable(MultiCastApp
    main.cpp
)

add_executable(client TcpClient/Clientmain.cpp)

target_compile_features(MultiCastApp PRIVATE cxx_std_17)

target_link_libraries(MultiCastApp
    PRIVATE multi_cast_core
)   

target_link_libraries(client
    PRIVATE Tcp_client
    PRIVATE multi_cast_core
)

set_target_properties(MultiCastApp PROPERTIES
    OUTPUT_NAME "multicast_app"
)
