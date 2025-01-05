#
# DeployQt.cmake
#
# Purpose:
#   - Provide a function to run windeployqt on a given target (Windows only).
#   - This must be included in your CMakeLists AFTER find_package(Qt6 ...)
#     and after add_executable(...) for the target you want to deploy.
#

function(deploy_qt_for targetName)
    if (WIN32)
        # Qt6_DIR might look like: C:/Qt/6.8.1/msvc2022_64/lib/cmake/Qt6
        # We want to peel off the last two directory levels to get the Qt prefix:
        get_filename_component(_qt6_prefix_path "${Qt6_DIR}" DIRECTORY)        
        get_filename_component(_qt6_prefix_path "${_qt6_prefix_path}" DIRECTORY)
        get_filename_component(_qt6_prefix_path "${_qt6_prefix_path}" DIRECTORY)
        message(STATUS "_qt6_prefix_path is: ${_qt6_prefix_path}")

        set(_windeployqt_exe "${_qt6_prefix_path}/bin/windeployqt.exe")

        add_custom_command(
            TARGET "${targetName}"
            POST_BUILD
            COMMAND "${_windeployqt_exe}"
                    # If we’re building Debug, pass --debug
                    # If Release, pass --release
                    $<$<CONFIG:Debug>:--debug>
                    $<$<CONFIG:Release>:--release>
                    --dir "$<TARGET_FILE_DIR:${targetName}>"
                    "$<TARGET_FILE:${targetName}>"
            COMMENT "Running windeployqt to copy Qt DLLs for target: ${targetName}"
        )
    endif()
endfunction()
