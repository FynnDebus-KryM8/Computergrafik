
file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

# Windows stuff
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows specific code
    set(OperatingSystem "Windows") 

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32 -DWIN32 -D_USE_MATH_DEFINES -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DGLEW_STATIC")
endif()

add_library(utils STATIC ${HDRS} ${SRCS})
target_link_libraries(utils stb_image)
