
add_executable(hello_world_c
	hello_world_c.cpp
)

install(TARGETS hello_world_c
	RUNTIME DESTINATION bin/hello_world_c
)
