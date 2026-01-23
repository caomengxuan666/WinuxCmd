# UPX Compression Configuration
# Try to find UPX in system path first
find_program(UPX_EXECUTABLE upx PATHS ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)
if(NOT UPX_EXECUTABLE)
    find_program(UPX_EXECUTABLE upx)
endif()

# Download UPX if not found
if(NOT UPX_EXECUTABLE)
    message(STATUS "UPX not found, attempting to download...")
    set(UPX_DOWNLOAD_URL "https://github.com/upx/upx/releases/download/v5.1.0/upx-5.1.0-win64.zip")
    set(UPX_ZIP_FILE "${CMAKE_BINARY_DIR}/upx-5.1.0-win64.zip")
    set(UPX_EXE_FILE "${CMAKE_BINARY_DIR}/upx.exe")

    # Download UPX zip file
    file(DOWNLOAD ${UPX_DOWNLOAD_URL} ${UPX_ZIP_FILE}
         TIMEOUT 30
         STATUS UPX_DOWNLOAD_STATUS
         SHOW_PROGRESS)

    list(GET UPX_DOWNLOAD_STATUS 0 UPX_DOWNLOAD_RESULT)
    if(UPX_DOWNLOAD_RESULT EQUAL 0)
        message(STATUS "UPX downloaded successfully")

        # Extract UPX executable from zip
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${UPX_ZIP_FILE} "upx-5.1.0-win64/upx.exe" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

        # Copy upx.exe to binary directory
        file(COPY "${CMAKE_BINARY_DIR}/upx-5.1.0-win64/upx.exe" DESTINATION ${CMAKE_BINARY_DIR})

        # Clean up
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/upx-5.1.0-win64")
        file(REMOVE "${UPX_ZIP_FILE}")

        set(UPX_EXECUTABLE "${UPX_EXE_FILE}")
        message(STATUS "UPX extracted to: ${UPX_EXECUTABLE}")
    else()
        message(WARNING "Failed to download UPX, skipping compression")
    endif()
endif()

if(UPX_EXECUTABLE)
    message(STATUS "UPX found at: ${UPX_EXECUTABLE}")
    message(STATUS "UPX compression will be enabled for release builds")
else()
    message(STATUS "UPX not available, skipping compression")
endif()

# Function to compress executable with UPX
function(add_upx_compression target)
    if(UPX_EXECUTABLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${UPX_EXECUTABLE} -9 "$<TARGET_FILE:${target}>"
            COMMENT "Compressing ${target} with UPX..."
            VERBATIM
        )
    endif()
endfunction()