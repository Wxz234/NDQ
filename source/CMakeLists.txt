file(GLOB_RECURSE NDQ_HEADER "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE NDQ_SOURCE "${PROJECT_SOURCE_DIR}/source/*.cpp")

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${NDQ_HEADER})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${NDQ_SOURCE})

add_library(ndq STATIC
    ${NDQ_HEADER}
    ${NDQ_SOURCE}
)
target_include_directories(ndq PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(ndq PUBLIC d3d12.lib dxgi.lib d3dcompiler.lib dxguid.lib)
set_target_properties(ndq PROPERTIES 
    FOLDER "libraries"
    #COMMON_LANGUAGE_RUNTIME ""
)
