cmake_minimum_required(VERSION 3.10)
project(cli_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurazione OS
if(WIN32)
    add_definitions(-D_WIN32)
elseif(UNIX)
    add_definitions(-D__linux__)
    set(OS_LIBS pthread)
endif()

# Directory di inclusione
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/lib/thirdParty/nlohmann
    ${PROJECT_SOURCE_DIR}/lib/libhs_datalog
)

# Sorgenti
set(SOURCE_FILES 
    src/main.cpp
    src/SystemUtils.cpp
    src/SensorDevice.cpp
    src/DataWriter.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Configurazione Libreria HS_DataLog (Logica invariata ma pulita)
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(HS_LIB_PATH "${PROJECT_SOURCE_DIR}/lib/libhs_datalog/64bit")
    else()
        set(HS_LIB_PATH "${PROJECT_SOURCE_DIR}/lib/libhs_datalog/32bit")
    endif()
    set(HS_LIB "${HS_LIB_PATH}/libhs_datalog_v2.dll")
    
    target_link_libraries(${PROJECT_NAME} ${HS_LIB})
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${HS_LIB}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

elseif(UNIX)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(HS_LIB_PATH "${PROJECT_SOURCE_DIR}/lib/libhs_datalog/raspberryPi_64bit")
        else()
            set(HS_LIB_PATH "${PROJECT_SOURCE_DIR}/lib/libhs_datalog/raspberryPi_32bit")
        endif()
    else()
        set(HS_LIB_PATH "${PROJECT_SOURCE_DIR}/lib/libhs_datalog/linux")
    endif()
    
    set(HS_LIB "${HS_LIB_PATH}/libhs_datalog_v2.so")
    
    target_link_libraries(${PROJECT_NAME} ${HS_LIB} ${OS_LIBS})
    
    # RPATH per runtime linking
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "${HS_LIB_PATH}"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()