if (WIN32)
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/windows/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/windows/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/windows/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
elseif (LINUX)
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/linux/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/linux/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/linux/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
else()
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/darwin/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/darwin/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/darwin/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
endif()

if (WIN32)
    # Only support 64-bit
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2main.lib;${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2.lib" CACHE INTERNAL "SDL2_LIBRARIES")
        set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/windows/SDL2_net.lib" CACHE INTERNAL "SDL2_NET_LIBRARIES")
        set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/windows/SDL2_image.lib" CACHE INTERNAL "SDL2_IMG_LIBRARIES")

        set(SDL2_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2.dll" CACHE INTERNAL "SDL2_DLL_PATH")
        set(SDL2_NET_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl_net/windows/SDL2_net.dll" CACHE INTERNAL "SDL2_NET_DLL_PATH")
        set(SDL2_IMG_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl_image/windows/SDL2_image.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libjpeg-9.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libpng16-16.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libtiff-5.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libwebp-7.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/zlib1.dll" CACHE INTERNAL "SDL2_IMG_DLL_PATH")
    else()
        message(ERROR "32-bit is not supported.")
    endif()
elseif (LINUX)
    set(OpenGL_GL_PREFERENCE "GLVND")

    set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/linux/libSDL2.a;" CACHE INTERNAL "SDL2_LIBRARIES")
    set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/linux/libSDL2_net.a" CACHE INTERNAL "SDL2_NET_LIBRARIES")
    set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/linux/libSDL2_image.a" CACHE INTERNAL "SDL2_IMG_LIBRARIES")
elseif (IOS)
    # we can include both the arm64 and x86_64 libs here as Xcode will choose the correct ones
    set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/ios/libSDL2.a;${LIBRARIES_ROOT_DIR}/sdl/ios/libSDL2main.a;${LIBRARIES_ROOT_DIR}/sdl/ios_simulator/libSDL2.a;${LIBRARIES_ROOT_DIR}/sdl/ios_simulator/libSDL2main.a;-lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -lobjc -Wl,-framework,CoreVideo,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal -Wl,-weak_framework,AVFoundation -Wl,-weak_framework,CoreMotion -Wl,-weak_framework,CoreGraphics -Wl,-weak_framework,OpenGLES -Wl,-weak_framework,UIKit -Wl,-weak_framework,Foundation -Wl,-weak_framework,ImageIO -Wl,-weak_framework,MobileCoreServices" CACHE INTERNAL "SDL2_LIBRARIES")
    set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/ios/libSDL2_net.a;${LIBRARIES_ROOT_DIR}/sdl_net/ios_simulator/libSDL2_net.a" CACHE INTERNAL "SDL2_NET_LIBRARIES")
    set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/ios/libSDL2_image.a;${LIBRARIES_ROOT_DIR}/sdl_image/ios_simulator/libSDL2_image.a" CACHE INTERNAL "SDL2_IMG_LIBRARIES")
else()
    # These SDL linker flags were gleaned by running `sdl2-config --static-libs` locally
    set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/darwin/libSDL2.a;-lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal" CACHE INTERNAL "SDL2_LIBRARIES")
    set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/darwin/libSDL2_net.a" CACHE INTERNAL "SDL2_NET_LIBRARIES")
    set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/darwin/libSDL2_image.a" CACHE INTERNAL "SDL2_IMG_LIBRARIES")
endif()

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
string(STRIP "${SDL2_NET_LIBRARIES}" SDL2_NET_LIBRARIES)
string(STRIP "${SDL2_IMG_LIBRARIES}" SDL2_IMG_LIBRARIES)

if (NOT LINUX)
    install(
        FILES
            ${SDL2_DLL_PATH}
            ${SDL2_NET_DLL_PATH}
            ${SDL2_IMG_DLL_PATH}
        DESTINATION
            ${INSTALL_DIRECTORY}
    )
endif()

function(copy_sdl_dependencies project_binary_dir)
    if (WIN32)
        file (
                COPY "${SDL2_DLL_PATH}"
                DESTINATION "${project_binary_dir}/bin"
        )

        file (
                COPY "${SDL2_NET_DLL_PATH}"
                DESTINATION "${project_binary_dir}/bin"
        )

        foreach(DLL_PATH ${SDL2_IMG_DLL_PATH})
            file (
                    COPY "${DLL_PATH}"
                    DESTINATION "${project_binary_dir}/bin"
            )
        endforeach()
    endif()
endfunction()
