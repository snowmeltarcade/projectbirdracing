function(copy_files BASE_DIR FROM FILE_EXT)
    file(GLOB files "${BASE_DIR}/${FROM}/*.${FILE_EXT}")

    foreach (file ${files})
        get_filename_component(filename ${file} NAME)

        configure_file("${FROM}${filename}" "${FROM}${filename}" COPYONLY)
    endforeach()
endfunction()
