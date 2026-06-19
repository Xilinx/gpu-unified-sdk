
add_executable(hello_world_c
	main.cpp
)

target_link_libraries(hello_world_c PRIVATE
	SdkCommon
)

install(TARGETS hello_world_c
	RUNTIME DESTINATION bin/hello_world_c
)
