
add_executable(fir_filter
	main.cpp
)

target_link_libraries(fir_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS fir_filter
	RUNTIME DESTINATION bin/fir_filter
)

install(DIRECTORY assets/
	DESTINATION bin/fir_filter/assets
	FILES_MATCHING PATTERN "*"
)
