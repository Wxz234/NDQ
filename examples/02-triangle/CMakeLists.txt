set_source_files_properties(vertex.hlsl pixel.hlsl PROPERTIES VS_TOOL_OVERRIDE "None")

add_executable(02-triangle WIN32 "main.cpp")
target_sources(02-triangle PRIVATE 	vertex.hlsl pixel.hlsl)

target_link_libraries(02-triangle PRIVATE ndq)
set_target_properties(02-triangle PROPERTIES 
    FOLDER "examples"
    VS_DEBUGGER_WORKING_DIRECTORY  $<TARGET_FILE_DIR:02-triangle>
)

add_custom_command(
    TARGET 02-triangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/vertex.hlsl $<TARGET_FILE_DIR:02-triangle>/vertex.hlsl
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/pixel.hlsl $<TARGET_FILE_DIR:02-triangle>/pixel.hlsl
)