
#include <vulkan-vendor-ids/VulkanVendorIds.hpp>

#include <array>




namespace vk {


#ifdef VULKAN_VENDOR_IDS_GEN
#include <vulkan-vendor-ids/gen/vendorids.inl>
#else
static constexpr int kKhronosVendorCount = 0;
static constexpr int kPciVendorCount = 0;
#define VULKAN_VENDOR_IDS_KHRONOS_VENDORS
#define VULKAN_VENDOR_IDS_PCI_VENDORS
#endif


struct Vendor {
	uint32_t id;
	std::string_view name;
};


static const std::array<Vendor, kKhronosVendorCount> kKhronosVendors = {
	VULKAN_VENDOR_IDS_KHRONOS_VENDORS
};

static const std::array<Vendor, kPciVendorCount> kPciVendors = {
	VULKAN_VENDOR_IDS_PCI_VENDORS
};


std::string_view findVendorNameForId(const uint32_t id) noexcept
{
	if ((id >> 16) == 1) { // detect khronos vendor
		for (const Vendor & vendor : kKhronosVendors) {
			if (vendor.id == id) return vendor.name;
		}
		return {};
	}
	for (const Vendor & vendor : kPciVendors) {
		if (vendor.id == id) return vendor.name;
	}
	return {};
}


} // vk
