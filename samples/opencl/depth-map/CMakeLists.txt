
add_executable(obstacle_detection
	main.cpp
)

target_link_libraries(obstacle_detection PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS obstacle_detection
	RUNTIME DESTINATION bin/obstacle_detection
)

install(DIRECTORY assets/
	DESTINATION bin/obstacle_detection/assets
	FILES_MATCHING PATTERN "*"
)
