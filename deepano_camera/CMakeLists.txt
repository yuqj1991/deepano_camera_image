cmake_minimum_required(VERSION 3.5)
project(deepano_camera)

SET(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-g -Wall -std=c++11)

set(INC_DIR /usr/local/include/libusb-1.0 include)
set(LINK_DIR /usr/local/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(usb-1.0)

set(Deepano_DIR so)
set(Deepano_LIB dp_api)
link_directories(${Deepano_DIR})
link_libraries(${Deepano_LIB})

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)


include_directories(include)

add_executable(deepano_camera
        include/Common.h
        include/dp_api.h
        include/dp_api_type.h
        include/dp_debug.h
        include/Fp16Convert.h
        include/interpret_output.h
        include/mv_types.h
        include/Region.h
		include/share_utils.h
		include/post_image.h
		include/model_sample.h 
		include/share.h
        src/Common.cpp
        src/fp16.c
        src/Fp16Convert.c
        src/interpret_output.cpp
        src/Region.cpp
        src/test.cpp  src/share_utils.c src/post_image.cpp src/model_sample.c src/video_result_callback.c)
        
target_link_libraries(deepano_camera ${OpenCV_LIBS})
target_link_libraries(deepano_camera usb-1.0)
target_link_libraries(deepano_camera Threads::Threads -lm)
target_link_libraries(deepano_camera ${Deepano_LIB})
