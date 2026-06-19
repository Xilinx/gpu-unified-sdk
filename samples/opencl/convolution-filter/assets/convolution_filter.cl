/**
 * \brief Convolution filter kernel function with custom kernel.
 * \param[in] inputImage Input image data in row-major format.
 * \param[in] width Width of the image passed in as inputImage.
 * \param[out] outputImage Output image after applying custom convolution filter.
 * \param[in] kernelData User-defined 3x3 or 5x5 kernel as a flat float array.
 * \param[in] kernelSize Size of the kernel (3 or 5).
 */
__kernel void convolution(__global const uchar* restrict inputImage,
                         const int width,
                         __global uchar* restrict outputImage,
                         __global const float* restrict kernelData,
                         const int kernelSize)
{
    /* Each kernel calculates 16 output pixels in the same row. */
    const int column = get_global_id(0) * 16;
    const int row = get_global_id(1) * 1;
    const int offset = row * width + column;

    /* Load rows based on kernel size (3x3 or 5x5) */
    short16 result = (short16)(0);
    const int halfKernel = kernelSize / 2;

    /* Load necessary rows (3 rows for 3x3, 5 rows for 5x5) */
    uchar16 rows[5];
    for (int i = -halfKernel; i <= halfKernel; i++)
    {
        rows[i + halfKernel] = vload16(0, inputImage + (offset + i * width));
    }

    /* Convert to shorts for intermediate calculations */
    short16 data[5];
    for (int i = 0; i < kernelSize; i++)
    {
        data[i] = convert_short16(rows[i]);
    }

    /* Apply convolution */
    for (int k = 0; k < kernelSize; k++)
    {
        for (int l = 0; l < kernelSize; l++)
        {
            int kernelIndex = k * kernelSize + l;
            short16 shiftedData;
            if (l == 0)
            {
                shiftedData = data[k];
            }
            else if (l == 1)
            {
                shiftedData = data[k].s123456789abcdef0;
            }
            else if (l == 2)
            {
                shiftedData = data[k].s23456789abcdef00;
            }
            else if (l == 3)
            {
                shiftedData = data[k].s3456789abcdef000;
            }
            else // l == 4
            {
                shiftedData = data[k].s456789abcdef0000;
            }
            result += shiftedData * (short)(kernelData[kernelIndex] * 256.0f); // Scale to avoid precision loss
        }
    }

    /* Normalize and convert back to uchar (assuming kernel is normalized to sum to 1) */
    vstore16(convert_uchar16(result >> 8), 0, outputImage + offset);
}