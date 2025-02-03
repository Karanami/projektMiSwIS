cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDDARD 23)
set(CMAKE_C_STANDDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH "..")

project(http_tcp VERSION 1.0)

add_executable( http_tcp 
                main.c
                mongoose.c )

target_include_directories(http_tcp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

#function(copy_file FILE)
    # add_custom_command(
    #     TARGET http_tcp PRE_BUILD
    #     COMMENT "Copping ${file} to exec destination"
    #     COMMAND ${CMAKE_COMMAND} -E copy 
    #         "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
    #         "${EXECUTABLE_OUTPUT_PATH}/"
    # )
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" "${EXECUTABLE_OUTPUT_PATH}/" COPYONLY)
#endfunction()

#copy_file(index.html)
#copy_file(prog.js)
#copy_file(style.css)
