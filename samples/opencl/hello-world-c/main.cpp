
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
