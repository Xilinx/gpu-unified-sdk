#!/usr/bin/env python3

import argparse
from dataclasses import dataclass, field
from typing import Dict, List
import os
import xml.etree.ElementTree as xml


@dataclass
class Value:
	value: int
	name: str


@dataclass
class EnumApi:
	bits: List[str] = field(default_factory=list)
	values: List[Value] = field(default_factory=list)


def create_enum_apis():
	return dict(
		vulkan=EnumApi(),
		vulkansc=EnumApi(),
	)


@dataclass
class Enum:
	name: str = None
	is64: bool = False
	apis: Dict[str, EnumApi] = field(default_factory=create_enum_apis)


def valid_number(s):
	if s is None:
		return None
	if s.startswith('0x'):
		return int(s, 16)
	else:
		return int(s)


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--vkxml', required=True)
	parser.add_argument('--output-interface', required=True)
	parser.add_argument('--output-implementation', required=True)
	args = parser.parse_args()

	default_apis = create_enum_apis().keys()

	def get_apis_from_str(strings):
		if strings is None:
			return None
		apis = strings.split(',')
		if not (len(apis) - 1 in range(2)):
			raise RuntimeError(f'Invalid api strings: {strings}')
		u = set()
		for api in apis:
			if api in u:
				raise RuntimeError(f'Duplicated api: {api}')
			u.add(api)
			if not api in default_apis:
				raise RuntimeError(f'Invalid api: {api}')
		return apis

	flags_enum_for_name = dict()
	values_enum_for_name = dict()

	enum_for_entry_name_for_api = dict()
	for name in default_apis:
		enum_for_entry_name_for_api[name] = dict()

	def add_entry(enum, apis, bitpos, value, entry_name):
		if entry_name is None:
			raise RuntimeError(f'Missing entry name: {enum.name}')

		if bitpos is None and value is None:
			raise RuntimeError(f'Missing bitpos and value: {enumname}')
		if not bitpos is None and not value is None:
			raise RuntimeError(f'bitpos and value are mutually exclusive: {enum.name}')

		if enum.is64:
			max_bit = 63
		else:
			max_bit = 31

		if not bitpos is None:
			def add_bitpos(name):
				api = enum.apis[name]
				enum_for_entry_name = enum_for_entry_name_for_api[name]

				if bitpos < 0 or bitpos > max_bit:
					raise RuntimeError(f'Invalid bitpos: {bitpos} ({enum.name})')
				while bitpos >= len(api.bits):
					api.bits.append(None)

				prev_bit_entry_name = api.bits[bitpos]
				if not prev_bit_entry_name is None:
					if prev_bit_entry_name == entry_name: return
					raise RuntimeError(f'Duplicated bitpos entry: {bitpos} ({enum.name}) oldname: {prev_bit_entry_name} newname: {entry_name}')

				prev_enum = enum_for_entry_name.get(entry_name)
				if not prev_enum is None:
					raise RuntimeError(f'Duplicated bit name: {bit_name} ({name}) prev: ({prev_enum.name})')
				api.bits[bitpos] = entry_name
				enum_for_entry_name[entry_name] = enum

			for name in apis:
				add_bitpos(name)
		else:
			def add_value(name):
				api = enum.apis[name]

				if enum.is64:
					min_value = 0
					max_value = 2**63 - 1
				else:
					min_value = -2**31
					max_value = 2**31-1
				if value < min_value or value > max_value:
					raise RuntimeError(f'Enum value is out of range, entry: {entry_name} value: {value} {entry_name}')
				for v in api.values:
					if v.value == value:
						if v.name != entry_name:
							raise RuntimeError(f'Duplicated values, entry: {entry_name} value: {value} ({v.name} != {entry_name}) ({enum.name})')
				api.values.append(Value(value, entry_name))

			for name in apis:
				add_value(name)

	def add_require(require_element, feature_apis, extension_extnumber):
		for enum_element in require_element.findall('enum'):
			if not enum_element.get('alias') is None: continue

			name = enum_element.get('extends')
			if name is None: continue

			entry_name = enum_element.get('name')
			bitpos = valid_number(enum_element.get('bitpos'))

			enum_api_strings = enum_element.get('api')
			if feature_apis is None:
				if enum_api_strings is None:
					# raise RuntimeError(f'Missing require api, entry {entry_name}')
					apis = default_apis
				else:
					apis = get_apis_from_str(enum_api_strings)
			else:
				if not enum_api_strings is None:
					raise RuntimeError(f'Duplicated require api, entry: {entry_name}')
				apis = feature_apis

			if not bitpos is None:
				enum = flags_enum_for_name.get(name)
				if enum is None:
					raise RuntimeError(f'Invalid bitpos extends ({name}) feature: ({feature_element.get("comment")})')
				add_entry(enum, apis, bitpos, None, entry_name)
			else:
				enum = flags_enum_for_name.get(name)
				if enum is None:
					enum = values_enum_for_name.get(name)
				if enum is None:
					raise RuntimeError(f'Invalid enum extends, entry: {entry_name} ({name}) feature: ({feature_element.get("comment")})')

				value = valid_number(enum_element.get('value'))
				offset = valid_number(enum_element.get('offset'))

				if not value is None:
					if not offset is None:
						raise RuntimeError(f'Mutually exclusive: value and offset, entry: {entry_name} ({enum.name}) feature: ({feature_element.get("comment")})')
				else:
					if offset is None:
						raise RuntimeError(f'Requires: value or offset, entry: {entry_name} ({enum.name}) feature: ({feature_element.get("comment")})')

					enum_extnumber = valid_number(enum_element.get('extnumber'))
					if extension_extnumber is None and enum_extnumber is None:
						raise RuntimeError(f'Missing extnumber: {entry_name}')

					if not enum_extnumber is None:
						extnumber = enum_extnumber
					else:
						extnumber = extension_extnumber

					dir_str = enum_element.get('dir')
					if not dir_str is None:
						if dir_str != '-':
							raise RuntimeError(f'Invalid dir, entry: {entry_name}')
						negative = -1
					else:
						negative = 1

					extbase = (extnumber - 1) * 1000

					value = (1000000000 + extbase + offset) * negative
				add_entry(enum, apis, None, value, entry_name)

	def get_enum_apis(element):
		strings = element.get('api')
		if strings is None:
			return default_apis
		else:
			return get_apis_from_str(strings)

	with open(args.vkxml) as vkxml_file:
		root_element = xml.parse(vkxml_file).getroot()

		for enums_element in root_element.findall('enums'):
			name = enums_element.get('name')
			if name is None:
				raise RuntimeError(f'Missing enums name')

			flags_enum = flags_enum_for_name.get(name)
			values_enum = values_enum_for_name.get(name)
			if not (flags_enum is None and values_enum is None):
				raise RuntimeError(f'Duplicated enums: {name}')

			etype = enums_element.get('type')

			if etype == 'bitmask':
				bitwidth_str = enums_element.get('bitwidth')
				if bitwidth_str is None:
					is64 = False
				else:
					bitwidth = int(bitwidth_str)
					if bitwidth != 64:
						raise RuntimeError(f'Invalid bitwidth: {bitwidth_str} ({name})')
					is64 = True

				enum = Enum(name=name, is64=is64)
				flags_enum_for_name[name] = enum

				for enum_element in enums_element.findall('enum'):
					if not enum_element.get('alias') is None: continue
					bitpos = valid_number(enum_element.get('bitpos'))
					value = valid_number(enum_element.get('value'))
					entry_name = enum_element.get('name')
					add_entry(enum, get_enum_apis(enum_element), bitpos, value, entry_name)

				continue

			if etype == 'enum':
				enum = Enum(name=name)
				values_enum_for_name[name] = enum

				for enum_element in enums_element.findall('enum'):
					if not enum_element.get('alias') is None: continue
					value = valid_number(enum_element.get('value'))
					entry_name = enum_element.get('name')
					add_entry(enum, get_enum_apis(enum_element), None, value, entry_name)

				continue

			if etype == 'constants':
				continue

			raise RuntimeError(f'Invalid enum type: {etype} ({name})')

		for feature_element in root_element.findall('feature'):

			api_strings = feature_element.get('api')
			if api_strings is None:
				raise RuntimeError(f'Missing feature api, feature: {feature_element.get("comment")} ({feature_element.get("name")})')
			apis = get_apis_from_str(api_strings)
			for require_element in feature_element.findall('require'):
				add_require(require_element, apis, None)

		extnumbers = set()
		for extensions_element in root_element.findall('extensions'):
			for extension_element in extensions_element.findall('extension'):
				extnumber = valid_number(extension_element.get('number'))
				if extnumber is None or extnumber in extnumbers:
					raise RuntimeError(f'Invalid extension number: {extnumber}')
				for require_element in extension_element.findall('require'):
					add_require(require_element, None, extnumber)

	os.makedirs(os.path.dirname(args.output_interface), exist_ok=True)
	os.makedirs(os.path.dirname(args.output_implementation), exist_ok=True)

	with open(args.output_interface, 'w') as output_interface_file:
		def p(*args):
			print(*args, file=output_interface_file)

		p()
		p('// flags')
		for name in sorted(flags_enum_for_name.keys()):
			enum = flags_enum_for_name[name]
			p(f'enum class {name} : {"uint64_t" if enum.is64 else "uint32_t"} {{}};')

		p()
		p('// values')
		for name in sorted(values_enum_for_name.keys()):
			enum = values_enum_for_name[name]
			p(f'enum class {name} : {"int64_t" if enum.is64 else "int32_t"} {{}};')

	with open(args.output_implementation, 'w') as output_implementation_file:
		def p(*args):
			print(*args, file=output_implementation_file)

		p()
		p('// flags')
		for name in sorted(flags_enum_for_name.keys()):
			enum = flags_enum_for_name[name]
			api = enum.apis['vulkan']

			p()
			p(f'template <> const int FlagsTraits<enums::{enum.name}>::size = {len(api.bits)};')
			p(f'template <> const std::string_view FlagsTraits<enums::{enum.name}>::names[{len(api.bits)}] = {{')
			for biti in range(len(api.bits)):
				entry_name = api.bits[biti]
				if entry_name is None:
					p(f'\tstd::string_view(),')
				else:
					p(f'\tstd::string_view("{entry_name}"),')
			p('};')

		p()
		p('// values')
		for name in sorted(values_enum_for_name.keys()):
			enum = values_enum_for_name[name]
			api = enum.apis['vulkan']

			p()
			p(f'template <> const int ValuesTraits<enums::{enum.name}>::size = {len(api.values)};')
			p(f'template <> const ValueEntry ValuesTraits<enums::{enum.name}>::entries[{len(api.values)}] = {{')
			for v in api.values:
				p(f'\tValueEntry{{ .value = {v.value}, .name = std::string_view("{v.name}") }},')
			p('};')


if __name__ == '__main__':
	main()
