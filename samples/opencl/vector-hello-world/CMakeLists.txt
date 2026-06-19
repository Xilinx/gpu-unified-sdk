
add_executable(vector_hello_world
	main.cpp
)

target_link_libraries(vector_hello_world PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS vector_hello_world
	RUNTIME DESTINATION bin/vector_hello_world
)

install(DIRECTORY assets/
	DESTINATION bin/vector_hello_world/assets
	FILES_MATCHING PATTERN "*"
)
