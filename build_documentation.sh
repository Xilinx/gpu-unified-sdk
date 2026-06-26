#!/bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


# Documentation generation script for the AMD GPU Unified SDK.
# Generates HTML API documentation (OpenGL ES, OpenCL, Vulkan libraries and
# samples) from source using Doxygen.
#
# Run this from the SDK root directory (where the Doxyfile lives):
#     ./build_documentation.sh
#
# Output is written to docs/html/index.html.

set -e  # Exit on any error

# Run from the directory containing this script (the SDK root), so the relative
# paths in the Doxyfile (lib/sdk, samples, README.md, doc_imgs) resolve.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}  AMD GPU Unified SDK Documentation Generation${NC}"
echo -e "${BLUE}==================================================${NC}"

# Check if doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo -e "${RED}Error: Doxygen is not installed${NC}"
    echo -e "${YELLOW}Please install doxygen:${NC}"
    echo "  Ubuntu/Debian: sudo apt-get install doxygen"
    echo "  CentOS/RHEL:   sudo yum install doxygen"
    echo "  Arch Linux:    sudo pacman -S doxygen"
    echo "  macOS:         brew install doxygen"
    exit 1
fi

# Display Doxygen version
echo -e "Found ${GREEN}$(doxygen --version)${NC} (doxygen)"

# Check if we're in the right directory
if [ ! -f "Doxyfile" ]; then
    echo -e "${RED}Error: Doxyfile not found${NC}"
    echo "Please run this script from the GPU Unified SDK root directory"
    exit 1
fi

echo -e "${YELLOW}Checking documentation inputs...${NC}"

# Sanity-check the source trees the Doxyfile documents.
for d in lib/sdk samples; do
    if [ -d "$d" ]; then
        echo -e "  ${GREEN}✓${NC} $d"
    else
        echo -e "  ${YELLOW}⚠${NC} $d not found (will be skipped)"
    fi
done
if [ -f "README.md" ]; then
    echo -e "  ${GREEN}✓${NC} README.md (used as the main page)"
else
    echo -e "  ${YELLOW}⚠${NC} README.md not found (no main page)"
fi

echo ""
echo -e "${YELLOW}Generating documentation...${NC}"

# Create the output directory if it doesn't exist (Doxyfile OUTPUT_DIRECTORY=docs,
# HTML_OUTPUT=html).
mkdir -p docs/html

# Run doxygen
if doxygen Doxyfile; then
    echo ""
    echo -e "${GREEN}==================================================${NC}"
    echo -e "${GREEN}  Documentation generated successfully!${NC}"
    echo -e "${GREEN}==================================================${NC}"
    echo ""
    echo -e "Documentation available at:"
    echo -e "  ${BLUE}file://$(pwd)/docs/html/index.html${NC}"
    echo ""
    echo -e "To view the documentation:"
    echo -e "  ${YELLOW}xdg-open docs/html/index.html${NC}     # Linux"
    echo -e "  ${YELLOW}open docs/html/index.html${NC}         # macOS"
    echo ""
    echo -e "Or serve it locally:"
    echo -e "  ${YELLOW}cd docs/html && python3 -m http.server 8000${NC}"
    echo ""

    # Count generated pages
    if [ -d "docs/html" ]; then
        page_count=$(find docs/html -name "*.html" | wc -l)
        echo -e "Generated ${GREEN}${page_count}${NC} HTML pages"
        if command -v du &> /dev/null; then
            docs_size=$(du -sh docs 2>/dev/null | cut -f1)
            echo -e "Documentation size: ${GREEN}${docs_size}${NC}"
        fi
    fi
else
    echo ""
    echo -e "${RED}==================================================${NC}"
    echo -e "${RED}  Documentation generation failed!${NC}"
    echo -e "${RED}==================================================${NC}"
    echo ""
    echo -e "Common issues:"
    echo -e "  • Check that all .dox files have valid syntax"
    echo -e "  • Verify image files referenced from README.md / .dox exist in doc_imgs/"
    echo -e "  • Ensure all @ref references point to valid targets"
    echo ""
    exit 1
fi

echo ""
echo -e "${BLUE}Documentation generation complete!${NC}"
