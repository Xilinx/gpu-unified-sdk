
add_executable(adas_lane_detection
	main.cpp
)

target_link_libraries(adas_lane_detection PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS adas_lane_detection
	RUNTIME DESTINATION bin/adas_lane_detection
)

install(DIRECTORY assets/
	DESTINATION bin/adas_lane_detection/assets
	FILES_MATCHING PATTERN "*"
)
