/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <vector>


/**
 * \brief Basic integer array addition implemented in C/C++.
 * \details A sample which shows how to add two integer arrays and store the result in a third array.
 *          No OpenCL code is used in this sample, only standard C/C++. The code executes only on the CPU.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const int arraySize = 1000000;

	std::vector<int> inputA(arraySize);
	std::vector<int> inputB(arraySize);
	std::vector<int> output(arraySize);

	for (int i = 0; i < arraySize; i++) {
		inputA[i] = i;
		inputB[i] = i;
	}

	for (int i = 0; i < arraySize; i++) {
		output[i] = inputA[i] + inputB[i];
	}

	return 0;
}
