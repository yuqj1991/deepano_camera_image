cmake_minimum_required(VERSION 3.5)
project(Deepano_SDK_AND_EXAMPLES)

SET(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-g -Wall -std=c++11)

set(INC_DIR /usr/local/include/libusb-1.0 include)
set(LINK_DIR /usr/local/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(usb-1.0)

set(Deepano_DIR ../Deepano_SDK_AND_EXAMPLES/so)
set(Deepano_LIB dp_api)
link_directories(${Deepano_DIR})
link_libraries(${Deepano_LIB})

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)


include_directories(include)

add_executable(Deepano_SDK_AND_EXAMPLES
        include/Common.h
        include/dp_api.h
        include/dp_api_type.h
        include/dp_debug.h
        include/Fp16Convert.h
        include/interpret_output.h
        include/mv_types.h
        include/Region.h
        src/Common.cpp
        src/fp16.c
        src/Fp16Convert.c
        src/interpret_output.cpp
        src/Region.cpp
        src/test.cpp include/share_utils.h src/share_utils.c src/post_image.cpp include/post_image.h src/model_sample.c include/model_sample.h include/share.h src/video_result_callback.c)
        
target_link_libraries(Deepano_SDK_AND_EXAMPLES ${OpenCV_LIBS})
target_link_libraries(Deepano_SDK_AND_EXAMPLES usb-1.0)
target_link_libraries(Deepano_SDK_AND_EXAMPLES Threads::Threads -lm)
target_link_libraries(Deepano_SDK_AND_EXAMPLES ${Deepano_LIB})
