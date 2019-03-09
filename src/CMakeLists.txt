cmake_minimum_required(VERSION 3.0)

link_directories(../../Vendor/build/lib)
link_directories(../../MavPPM-EMB-PPM/build/libMavPPM_PPMEncoder/lib)

# Options
option(MAVPPM_CROSS_COMPILE "Cross Compile For MavPPM" OFF)
if(NOT DEFINED BUILD_CROSS_COMPILE)
    set(BUILD_CROSS_COMPILE OFF)
endif()


# Source Dir
aux_source_directory(./app mavppm_source_app)
aux_source_directory(./app/base mavppm_source_app_base)
aux_source_directory(./app/mavlink mavppm_source_mavlink)
aux_source_directory(./app/message_handler mavppm_source_message_handler)
aux_source_directory(./utils mavppm_source_utils)

# Target
add_executable(
        MavPPM
        ${mavppm_source_app}
        ${mavppm_source_app_base}
        ${mavppm_source_message_handler}
        ${mavppm_source_utils}
        ${mavppm_source_mavlink}
)

# Search
target_include_directories(
        MavPPM
        PRIVATE
        ./app
        ./app/base
        ./app/mavlink
        ./app/message_handler
        ./utils
)

target_include_directories(
        MavPPM
        PUBLIC
        ../../MavPPM-EMB-PPM/include
        ../../MavPPM-mavlink/mavlink/include/v1
        ../../MavPPM-mavlink/mavlink/include/v1/standard
        ../../Vendor/build/include
)

# Link
target_link_libraries(
        MavPPM
        PRIVATE
        MavPPM_PPMEncoder
        SocketKit
        pthread
        libplist.a
)

if(BUILD_CROSS_COMPILE STREQUAL "ON")
    target_link_libraries(
            MavPPM
            PRIVATE
            wiringPi
    )
endif()

# Install
## Install Target
install(
        TARGETS MavPPM
        RUNTIME DESTINATION bin
)
