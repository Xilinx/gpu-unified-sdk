
add_executable(convolution_filter
	main.cpp
)

target_link_libraries(convolution_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS convolution_filter
	RUNTIME DESTINATION bin/convolution_filter
)

install(DIRECTORY assets/
	DESTINATION bin/convolution_filter/assets
	FILES_MATCHING PATTERN "*"
)
