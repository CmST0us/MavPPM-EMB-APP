cmake_minimum_required(VERSION 3.0)

link_directories(../../Vendor/build/lib)
link_directories(../../MavPPM-EMB-PPM/build/libMavPPM_PPMEncoder/lib)

# Depend
execute_process(
        COMMAND
        ../tools/build_dep.sh
)

# Options

# Source Dir
aux_source_directory(./app mavppm_source_app)
aux_source_directory(./app/base mavppm_source_app_base)
aux_source_directory(./utils mavppm_source_utils)


# Target
add_executable(
        MavPPM
        ${mavppm_source_app}
        ${mavppm_source_app_base}
        ${mavppm_source_utils}
)

# Search
target_include_directories(
        MavPPM
        PRIVATE
        ./app
        ./utils
)

target_include_directories(
        MavPPM
        PUBLIC
        ../../MavPPM-EMB-PPM/include
        ../../Vendor/build/include
)

# Link
target_link_libraries(
        MavPPM
        PRIVATE
        MavPPM_PPMEncoder
        SocketKit
)

# Install
## Install Target
install(
        TARGETS MavPPM
        RUNTIME DESTINATION bin
)
