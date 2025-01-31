add_executable(03-model WIN32 "main.cpp")
target_link_libraries(03-model PUBLIC ndq)
set_target_properties(03-model PROPERTIES FOLDER "examples")