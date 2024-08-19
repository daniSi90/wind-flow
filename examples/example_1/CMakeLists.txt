cmake_minimum_required(VERSION 3.10)
project(cm-example-1 VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

include(CTest)
enable_testing()

#FILE(GLOB_RECURSE LV_DRIVERS_Sources CONFIGURE_DEPENDS components/lvgl_drv/*.c)
#FILE(GLOB_RECURSE LVGL_Sources CONFIGURE_DEPENDS ../components/lvgl/*.c)
#FILE(GLOB_RECURSE UI_Sources CONFIGURE_DEPENDS ../components/ui/*.c)

add_executable(cm-example-1 main.c
                            ../../cycle_master/cycle_master.c
                            #fault_handler.c
                            # ../lwjson.c
                            # ../lwjson_debug.c
                            # ../lwjson_stream.c
                            # ../semver.c
                            #${LV_DRIVERS_Sources}
                            #${LVGL_Sources}
                            #${UI_Sources}
                            )

if (WIN32)
set(SDL2_LIB_DIR C:/MinGW/lib)
set(SDL2_INCLUDE_DIR C:/MinGW/include)
set(SDL2_LIBRARY C:/MinGW/bin)
endif (WIN32)

# SETUP INCLUDE AND LINK DIRS:

 # Add include directories
target_include_directories(cm-example-1 PRIVATE
                                        #${SDL2_INCLUDE_DIR}
                                        #${CMAKE_SOURCE_DIR}/components/lvgl_drv/include
                                        ../../cycle_master/include
                                        # ../include
                                        #../components/lvgl
                                        #../components/alledio/include
                                        #../components/ui/include
                                        #../components/ui/backend/include
                                        #../components/ui/screens/include
                                        #../components/ui/screens/danfoss/include
                                        #../components/ui/components/include
                                        # Add more include directories as needed
                                        )

link_directories(
    ${SDL2_LIB_DIR}
)

if (WIN32)
target_link_libraries(
    ${PROJECT_NAME}
        mingw32
)
else()
target_link_libraries(
    ${PROJECT_NAME}
       # mingw32
        m
)
endif (WIN32)


# Add the preprocessor definition "DEVICE_SIMULATOR"
#target_compile_definitions(cm-example-1 PRIVATE DEVICE_SIMULATOR CONFIG_VENDOR_DANFOSS)

#add_compile_definitions(LV_CONF_PATH=${CMAKE_SOURCE_DIR}/components/lvgl_drv/include/lv_conf.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
