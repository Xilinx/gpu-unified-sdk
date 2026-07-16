#!/usr/bin/env python3

import argparse
from dataclasses import dataclass
import os
import xml.etree.ElementTree as xml


@dataclass
class KhronosVendor:
	id: int
	key: str
	name: str


@dataclass
class PciVendor:
	id: int
	name: str


def escape(name):
	return name.replace('"', r'\"')


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--pciids', required=True)
	parser.add_argument('--vkxml', required=True)
	parser.add_argument('--output', required=True)
	args = parser.parse_args()

	def error(msg):
		raise RuntimeError(f'pci.ids: {msg}')

	khronos_vendors = dict()
	pci_vendors = dict()

	with open(args.vkxml) as vkxml_file:
		root = xml.parse(vkxml_file).getroot()
		for enums in root:
			if enums.tag == 'enums' and enums.get('name') == 'VkVendorId':
				for enum in enums:
					if enum.tag == 'enum':
						idstr = enum.get('value')
						id = int(idstr, 16)
						key = enum.get('name')
						name = enum.get('comment')
						if not khronos_vendors.get(id) is None:
							raise RuntimeError(f'Duplicated Khronos id: {idstr}')
						khronos_vendors[id] = KhronosVendor(id=id, key=key, name=name)

	with open(args.pciids) as pciids_file:
		for line in pciids_file:
			line = line.rstrip()
			if len(line) == 0: continue
			if line.startswith('#'): continue
			if line.startswith('\t'): continue
			if line.startswith('C '): continue
			tokens = line.split('  ', maxsplit=1)
			if len(tokens) != 2:
				error(f'Invalid line: {line}')
			(idstr, name) = tokens
			id = int(idstr, 16)
			if f'{id:>04x}' != idstr:
				raise RuntimeError(f'Invalid PCI id: {idstr}')
			if not pci_vendors.get(id) is None:
				error(f'Duplicated PCI id: {idstr}')
			pci_vendors[id] = PciVendor(id=id, name = name)

	os.makedirs(os.path.dirname(args.output), exist_ok=True)

	with open(args.output, 'w') as output_file:
		def p(*args):
			print(*args, file=output_file)

		longest_khronos_name = 0
		longest_khronos_key = 0
		for vendor in khronos_vendors.values():
			longest_khronos_name = max(longest_khronos_name, len(vendor.name))
			longest_khronos_key = max(longest_khronos_key, len(vendor.key))

		p(f'static constexpr int kKhronosVendorCount = {len(khronos_vendors)};')
		p(f'static constexpr int kPciVendorCount = {len(pci_vendors)};')

		p('#define VULKAN_VENDOR_IDS_KHRONOS_VENDORS \\')
		for id in sorted(khronos_vendors.keys()):
			vendor = khronos_vendors[id]
			align_name = longest_khronos_name - len(vendor.name)
			align_key = longest_khronos_key - len(vendor.key)
			p(f'\tVendor{{ .id = 0x{vendor.id:>08x}, .name = std::string_view("{escape(vendor.name)}") }}, {" " * align_name}/*{vendor.key}*/ {" " * align_key}\\')
		p()
		p('#define VULKAN_VENDOR_IDS_PCI_VENDORS \\')
		for id in sorted(pci_vendors.keys()):
			vendor = pci_vendors[id]
			p(f'\tVendor{{ .id = 0x{vendor.id:>04x}, .name = std::string_view("{escape(vendor.name)}") }}, \\')
		p()


if __name__ == '__main__':
	main()
