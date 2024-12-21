add_library(DirectX-Headers STATIC "source/d3dx12_property_format_table.cpp")
target_include_directories(DirectX-Headers PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(DirectX-Headers PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/directx")
set_target_properties(DirectX-Headers PROPERTIES FOLDER "libraries")