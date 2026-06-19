
add_executable(sobel_filter_no_vectors
	main.cpp
)

target_link_libraries(sobel_filter_no_vectors PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS sobel_filter_no_vectors
	RUNTIME DESTINATION bin/sobel_filter_no_vectors
)

install(DIRECTORY assets/
	DESTINATION bin/sobel_filter_no_vectors/assets
	FILES_MATCHING PATTERN "*"
)
