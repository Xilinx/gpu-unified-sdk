
add_executable(matrix_multiply
	main.cpp
)

target_link_libraries(matrix_multiply PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS matrix_multiply
	RUNTIME DESTINATION bin/matrix_multiply
)

install(DIRECTORY assets/
	DESTINATION bin/matrix_multiply/assets
	FILES_MATCHING PATTERN "*"
)
