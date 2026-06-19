
add_executable(sgemm
	main.cpp
)

target_link_libraries(sgemm PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS sgemm
	RUNTIME DESTINATION bin/sgemm
)

install(DIRECTORY assets/
	DESTINATION bin/sgemm/assets
	FILES_MATCHING PATTERN "*"
)
