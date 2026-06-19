
add_executable(hello_world_opencl
	main.cpp
)

target_link_libraries(hello_world_opencl PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS hello_world_opencl
	RUNTIME DESTINATION bin/hello_world_opencl
)

install(DIRECTORY assets/
	DESTINATION bin/hello_world_opencl/assets
	FILES_MATCHING PATTERN "*"
)
