cmake_minimum_required(VERSION 3.5)
project(deepano_image)


SET(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)

set(INC_DIR /usr/local/include/libusb-1.0 include)
set(LINK_DIR /usr/local/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(usb-1.0)

set(Deepano_DIR ../deepano_image/so)
set(Deepano_LIB dp_api_mdk)
link_directories(${Deepano_DIR})
link_libraries(${Deepano_LIB})

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)


include_directories(include)


add_executable(deepano_image
        include/dp_api.h
        include/dp_api_type.h
        include/dp_debug.h
        include/Fp16Convert.h
        include/model_sample.h
        include/mtcnn.h
        include/mv_types.h
        include/pre_image.h
        include/share_utils.h
        src/fp16.c
        src/Fp16Convert.c
        src/model_sample.cpp
        src/mtcnn.cpp
        src/pre_image.cpp
        src/share_utils.c
        src/test.cpp)
target_link_libraries(deepano_image ${OpenCV_LIBS})
target_link_libraries(deepano_image usb-1.0)
target_link_libraries(deepano_image Threads::Threads -lm)
target_link_libraries(deepano_image ${Deepano_LIB})
