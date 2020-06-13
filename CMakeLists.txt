CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(kvdb-cli)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Single thread mode /ML and /MLd have be deprecated from VS 2003.
option(BoostOption_USE_MULTITHREADED "Enable Boost's library use multithread" ON)

if (WIN32)
    option(BoostOption_USE_STATIC_LIBS "Enable Boost use static library" ON)
    option(BoostOption_USE_STATIC_RUNTIME "Enable Boost use static runtime library" OFF)
else()
    option(BoostOption_USE_STATIC_LIBS "Enable Boost use static library" OFF)
    option(BoostOption_USE_STATIC_RUNTIME "Enable Boost use static runtime library" OFF)
endif()

set(Boost_USE_STATIC_LIBS        BoostOption_USE_STATIC_LIBS)      # only find static libs
set(Boost_USE_MULTITHREADED      BoostOption_USE_MULTITHREADED)
set(Boost_USE_STATIC_RUNTIME     BoostOption_USE_STATIC_RUNTIME)

find_package(Boost 1.55 COMPONENTS coroutine context system thread chrono program_options REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(EXTRA_LIBS ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

find_package (Threads)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message("----------------------------------")

message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("  PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("  PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("----------------------------------")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif()

if (NOT MSVC)
    ## -Wall -Werror -Wextra -Wno-format -Wno-unused-function
    ##set(CMAKE_CXX_FLAGS_DEFAULT "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4a -msse4.1 -msse4.2 -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-unused-variable -Wno-unknown-pragmas -fPIC -U__STRICT_ANSI__")
    set(CMAKE_CXX_FLAGS_DEFAULT "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall -Wno-unused-function -Wno-deprecated-declarations -Wno-unused-variable -Wno-unknown-pragmas -fPIC -U__STRICT_ANSI__")
    ## add_compile_options(-D__SSE3__ -D__SSE4A__ -D__SSE4_1__ -D__SSE4_2__)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEFAULT} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEFAULT} -g -pg -D_DEBUG")
endif()

if (WIN32)
    add_compile_options("-D_WIN32_WINNT=0x0601")
    add_compile_options("-D_CRT_SECURE_NO_WARNINGS")
    set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32 mswsock)
    if (Boost_USE_STATIC_RUNTIME)
        #
        # Replace all "/MD" flags to "/MT"
        # See: http://stackoverflow.com/questions/14172856/cmake-compile-with-mt-instead-of-md
        #
        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            )
        foreach(CompilerFlag ${CompilerFlags})
            if (${CompilerFlag} MATCHES "/MD")
                string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            endif()
        endforeach()
    endif()
endif()

if (UNIX)
    set(EXTRA_LIBS ${EXTRA_LIBS} pthread)
else()
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

include_directories(include)
include_directories(src)
include_directories(src/main)
include_directories(src/test)
include_directories(src/main/kvdb-cli)

set(SOURCE_FILES
	src/main/kvdb-cli/client/KvdbClientApp.cpp
	src/main/kvdb-cli/kvdb-cli.cpp
    )

# add_custom_target(debug
#     COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#     COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
# )

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)

add_executable(kvdb-cli ${SOURCE_FILES})
target_link_libraries(kvdb-cli ${EXTRA_LIBS})

project(kvdb-server)

include_directories(include)
include_directories(src)
include_directories(src/main)
include_directories(src/test)
include_directories(src/main/kvdb-server)

set(SOURCE_FILES
	src/main/kvdb-cli/server/Connection.cpp
	src/main/kvdb-cli/server/ConnectionManager.cpp
	src/main/kvdb-cli/server/KvdbConnection.cpp
	src/main/kvdb-cli/server/RequestHandler.cpp
	src/main/kvdb-cli/server/Response.cpp
	src/main/kvdb-cli/kvdb-server.cpp
    )

# add_custom_target(debug
#     COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#     COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
# )

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Switch CMAKE_BUILD_TYPE to Release"
)

add_executable(kvdb-server ${SOURCE_FILES})
target_link_libraries(kvdb-server ${EXTRA_LIBS})
