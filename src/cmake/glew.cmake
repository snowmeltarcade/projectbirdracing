if (WIN32)
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/windows/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/glew/windows/lib/glew32.lib" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "${LIBRARIES_ROOT_DIR}/glew/windows/bin/glew32.dll" CACHE INTERNAL "GLEW_DLL_PATH")
elseif (LINUX)
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/linux/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/glew/linux/lib/libGLEWd.a" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "" CACHE INTERNAL "GLEW_DLL_PATH")
else()
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/darwin/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.a" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.dylib;${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.2.2.dylib;${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.2.2.0.dylib" CACHE INTERNAL "GLEW_DLL_PATH")
endif()

if (NOT LINUX)
    install(
        FILES
            ${GLEW_DLL_PATH}
        DESTINATION
            ${INSTALL_DIRECTORY}
    )

    if (WIN32)
        file(
            COPY "${GLEW_DLL_PATH}"
            DESTINATION "${PROJECT_BINARY_DIR}/bin"
        )
    endif()
endif()
