
add_executable(ml_classifier
	main.cpp
)

target_link_libraries(ml_classifier PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS ml_classifier
	RUNTIME DESTINATION bin/ml_classifier
)

install(DIRECTORY assets/
	DESTINATION bin/ml_classifier/assets
	FILES_MATCHING PATTERN "*"
)
