
#include "GBMUtils.hpp"

#include <Common/Log.hpp>




namespace Gbm {




Device Device::get(const int fd)
{
	::gbm_device * const handle = ::gbm_create_device(fd);
	CHECK(handle)
	return Device(handle);
}


Device::~Device() noexcept
{
	if (handle_) {
		::gbm_device_destroy(handle_);
	}
}




Surface Surface::create(const Device & device, const Size & size, const uint32_t format,
		const uint64_t * const modifiers, const int modifierCount, const uint32_t flags)
{
	::gbm_surface * const handle = ::gbm_surface_create_with_modifiers2(device,
			size.width, size.height, format, modifiers, modifierCount, flags);
	CHECK(handle)
	return Surface(handle);
}


Surface::~Surface() noexcept
{
	if (handle_) {
		::gbm_surface_destroy(handle_);
	}
}




} // Gbm
