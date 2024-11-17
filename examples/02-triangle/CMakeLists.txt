set(VERTEXSHADER_FILES "vertex.hlsl")
set(PIXELSHADER_FILES "pixel.hlsl")

add_executable(02-triangle WIN32 
	"main.cpp"
	${VERTEXSHADER_FILES}
	${PIXELSHADER_FILES}
)

target_link_libraries(02-triangle PRIVATE ndq)

set_target_properties(02-triangle PROPERTIES 
	FOLDER "examples"
	VS_DEBUGGER_WORKING_DIRECTORY  "$(OutDir)"
)

set_source_files_properties(${VERTEXSHADER_FILES} PROPERTIES 
	VS_SHADER_TYPE Vertex
	VS_SHADER_ENTRYPOINT mainVS
	VS_SHADER_MODEL 6.6
)

set_source_files_properties(${PIXELSHADER_FILES} PROPERTIES 
	VS_SHADER_TYPE Pixel
	VS_SHADER_ENTRYPOINT mainPS
	VS_SHADER_MODEL 6.6
)