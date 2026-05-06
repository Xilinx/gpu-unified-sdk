
add_executable(gaussian_filter
	main.cpp
)

target_link_libraries(gaussian_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS gaussian_filter
	RUNTIME DESTINATION bin/gaussian_filter
)

install(DIRECTORY assets/
	DESTINATION bin/gaussian_filter/assets
	FILES_MATCHING PATTERN "*"
)
