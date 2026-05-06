/**
 * \brief Gaussian filter kernel function.
 * \param[in] inputImage Input image data in row-major format.
 * \param[in] width Width of the image passed in as inputImage.
 * \param[out] outputImage Output image after applying 3x3 Gaussian blur.
 */
__kernel void gaussian(__global const uchar* restrict inputImage,
                       const int width,
                       __global uchar* restrict outputImage)
{
    /* Each kernel calculates 16 output pixels in the same row. */
    const int column = get_global_id(0) * 16;
    const int row = get_global_id(1) * 1;
    const int offset = row * width + column;

    /* 3x3 Gaussian kernel weights (approximated, sum to 16 for integer division) */
    const float k00 = 1.0f, k01 = 2.0f, k02 = 1.0f;
    const float k10 = 2.0f, k11 = 4.0f, k12 = 2.0f;
    const float k20 = 1.0f, k21 = 2.0f, k22 = 1.0f;

    /* Load three rows of 18 pixels (16 outputs + 1 pixel border on each side) */
    uchar16 topLoad = vload16(0, inputImage + (offset - width));
    uchar16 middleLoad = vload16(0, inputImage + offset);
    uchar16 bottomLoad = vload16(0, inputImage + (offset + width));

    /* Convert to shorts for intermediate calculations */
    short16 topData = convert_short16(topLoad);
    short16 middleData = convert_short16(middleLoad);
    short16 bottomData = convert_short16(bottomLoad);

    /* Apply Gaussian filter for 16 pixels */
    short16 result = (short16)(0);

    /* First column contributions */
    result += topData * (short)k00;
    result += middleData * (short)k10;
    result += bottomData * (short)k20;

    /* Second column contributions (shifted by 1) */
    short16 topShifted = topData.s123456789abcdef0;
    short16 middleShifted = middleData.s123456789abcdef0;
    short16 bottomShifted = bottomData.s123456789abcdef0;
    result += topShifted * (short)k01;
    result += middleShifted * (short)k11;
    result += bottomShifted * (short)k21;

    /* Third column contributions (shifted by 2) */
    topShifted = topData.s23456789abcdef00;
    middleShifted = middleData.s23456789abcdef00;
    bottomShifted = bottomData.s23456789abcdef00;
    result += topShifted * (short)k02;
    result += middleShifted * (short)k12;
    result += bottomShifted * (short)k22;

    /* Normalize by dividing by 16 (sum of kernel weights) and convert back to uchar */
    vstore16(convert_uchar16(result >> 4), 0, outputImage + offset);
}
