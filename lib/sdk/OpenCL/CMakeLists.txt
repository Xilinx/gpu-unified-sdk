
find_package(OpenCL REQUIRED)

add_library(SdkOpenCL SHARED
	CLUtils.cpp
)

target_link_libraries(SdkOpenCL
	PRIVATE
		SdkCommon

	PUBLIC
		OpenCL::OpenCL
)
