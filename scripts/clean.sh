#!/usr/bin/env bash
#
# clean.sh - remove temporary build artifacts from the SDK tree.
#
# The SDK can be built in-source (CMake/Ninja generate files next to the
# sources) or out-of-source. This script removes the well-known build artifacts
# so the working tree is left with source files only. It NEVER touches tracked
# source, shaders, assets or the hand-written CMakeLists.txt files.
#
# Usage:
#   scripts/clean.sh [-n|--dry-run] [-q|--quiet] [-h|--help] [DIR ...]
#
#   -n, --dry-run   Show what would be removed, but delete nothing.
#   -q, --quiet     Only print a summary.
#   -h, --help      Show this help.
#   DIR ...         Extra directories to also clean (e.g. an out-of-source
#                   build dir). Defaults to the SDK root.

set -euo pipefail

# Resolve the SDK root (the parent of this scripts/ directory) regardless of the
# caller's working directory.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

DRY_RUN=0
QUIET=0
declare -a TARGETS=()

usage() {
	sed -n '2,20p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

while [ $# -gt 0 ]; do
	case "$1" in
		-n|--dry-run) DRY_RUN=1 ;;
		-q|--quiet)   QUIET=1 ;;
		-h|--help)    usage; exit 0 ;;
		-*)
			echo "clean.sh: unknown option '$1'" >&2
			usage >&2
			exit 2
			;;
		*) TARGETS+=("$1") ;;
	esac
	shift
done

# Default to cleaning the SDK root if no directories were given.
if [ "${#TARGETS[@]}" -eq 0 ]; then
	TARGETS=("$SDK_ROOT")
fi

# Artifact patterns.
#  - Directories removed recursively (-type d, by name).
ARTIFACT_DIRS=(
	"CMakeFiles"            # CMake per-target build dir
	"_deps"                 # CMake FetchContent downloads
	"Testing"               # CTest output
	"run"                   # per-sample staged binary + assets output dir
	"docs"                  # generated Doxygen HTML output (build_documentation.sh)
)
#  - Files removed by exact name.
ARTIFACT_FILES=(
	"CMakeCache.txt"
	"cmake_install.cmake"
	"CTestTestfile.cmake"
	"build.ninja"
	"rules.ninja"
	".ninja_deps"
	".ninja_log"
	"compile_commands.json"
	"Makefile.cmake"
	"install_manifest.txt"
)
#  - Files removed by glob (object files, archives, generated wayland protocol).
#    NOTE: deliberately NOT matching "*.obj" - that is a 3D model asset format
#    used by the samples (e.g. models/audi.obj), not a compiler object file.
#    This toolchain emits ".o" object files.
ARTIFACT_GLOBS=(
	"*.o"
	"*.a"
	"*.so"
	"*.so.*"
	"*.d"
	"xdg-shell-client-protocol.h"
	"xdg-shell-protocol.c"
)

removed_count=0

note() { [ "$QUIET" -eq 1 ] || echo "$@"; }

remove_path() {
	local p="$1"
	if [ "$DRY_RUN" -eq 1 ]; then
		note "would remove: $p"
	else
		rm -rf -- "$p"
		note "removed: $p"
	fi
	removed_count=$((removed_count + 1))
}

clean_dir() {
	local root="$1"
	if [ ! -d "$root" ]; then
		echo "clean.sh: skipping non-existent dir: $root" >&2
		return
	fi
	note "Cleaning: $root"

	local name
	for name in "${ARTIFACT_DIRS[@]}"; do
		while IFS= read -r -d '' p; do
			remove_path "$p"
		done < <(find "$root" -type d -name "$name" -prune -print0 2>/dev/null)
	done

	for name in "${ARTIFACT_FILES[@]}"; do
		while IFS= read -r -d '' p; do
			remove_path "$p"
		done < <(find "$root" -type f -name "$name" -print0 2>/dev/null)
	done

	local glob
	for glob in "${ARTIFACT_GLOBS[@]}"; do
		while IFS= read -r -d '' p; do
			remove_path "$p"
		done < <(find "$root" -type f -name "$glob" -print0 2>/dev/null)
	done
}

for t in "${TARGETS[@]}"; do
	clean_dir "$t"
done

if [ "$DRY_RUN" -eq 1 ]; then
	echo "Dry run complete: $removed_count item(s) would be removed."
else
	echo "Clean complete: $removed_count item(s) removed."
fi
