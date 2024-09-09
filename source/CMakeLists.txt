set(NDQ_HEADER_PATH
    "${PROJECT_SOURCE_DIR}/include/ndq/window.h"
    "${PROJECT_SOURCE_DIR}/include/ndq/rhi/device.h"
)
set(NDQ_SOURCE_PATH
    "${PROJECT_SOURCE_DIR}/source/window.cpp"
    "${PROJECT_SOURCE_DIR}/source/rhi/device.cpp"
)

add_library(ndq STATIC 
    ${NDQ_HEADER_PATH}
    ${NDQ_SOURCE_PATH}
)
target_include_directories(ndq PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(ndq PUBLIC d3d12.lib dxgi.lib)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${NDQ_HEADER_PATH})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${NDQ_SOURCE_PATH})

set_target_properties(ndq PROPERTIES FOLDER "libraries")