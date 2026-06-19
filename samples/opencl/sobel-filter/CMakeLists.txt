
add_executable(sobel_filter
	main.cpp
)

target_link_libraries(sobel_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS sobel_filter
	RUNTIME DESTINATION bin/sobel_filter
)

install(DIRECTORY assets/
	DESTINATION bin/sobel_filter/assets
	FILES_MATCHING PATTERN "*"
)
