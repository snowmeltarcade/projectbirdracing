function(copy_files BASE_DIR FROM FILE_EXT)
    file(GLOB files "${BASE_DIR}/${FROM}/*.${FILE_EXT}")

    foreach (file ${files})
        get_filename_component(filename ${file} NAME)

        configure_file("${FROM}${filename}" "${FROM}${filename}" COPYONLY)
    endforeach()
endfunction()

function(install_data_files FROM TO INSTALL_DIR)
    file(GLOB files "${FROM}/*.*")

    foreach (file ${files})
        get_filename_component(filename ${file} NAME)

        configure_file("${FROM}${filename}" "${TO}${filename}" COPYONLY)

        install(
            FILES
                ${FROM}${filename}
            DESTINATION
                "${INSTALL_DIR}"
        )
    endforeach()
endfunction()

function(configure_data_files)
    install_data_files(
        "${CMAKE_CURRENT_LIST_DIR}/data/graphics/"
        "${CMAKE_BINARY_DIR}/bin/data/graphics/"
        "${INSTALL_DIRECTORY}/data/graphics/"
    )

    install_data_files(
        "${CMAKE_CURRENT_LIST_DIR}/data/windowing/"
        "${CMAKE_BINARY_DIR}/bin/data/windowing/"
        "${INSTALL_DIRECTORY}/data/windowing/"
    )
endfunction()
