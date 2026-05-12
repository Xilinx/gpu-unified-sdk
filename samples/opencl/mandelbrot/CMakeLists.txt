
add_executable(mandelbrot
	main.cpp
)

target_link_libraries(mandelbrot PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS mandelbrot
	RUNTIME DESTINATION bin/mandelbrot
)

install(DIRECTORY assets/
	DESTINATION bin/mandelbrot/assets
	FILES_MATCHING PATTERN "*"
)
