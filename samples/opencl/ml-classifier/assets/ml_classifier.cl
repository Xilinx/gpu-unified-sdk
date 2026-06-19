/**
 * \brief Kernel to compute partial sums of pixel intensities for feature extraction.
 * \param[in] inputImage Input image data in row-major format (luminance).
 * \param[in] width Width of the image.
 * \param[in] height Height of the image.
 * \param[out] partialSums Array to store per-workgroup sums of pixel intensities.
 */
__kernel void compute_intensity_sum(__global const uchar* restrict inputImage,
                                   const int width,
                                   const int height,
                                   __global float* restrict partialSums)
{
    const int column = get_global_id(0) * 16;
    const int row = get_global_id(1);
    const int groupId = get_group_id(1) * get_num_groups(0) + get_group_id(0);
    const int offset = row * width + column;

    float16 sum = (float16)(0.0f);

    if (column < width && row < height)
    {
        uchar16 pixels = vload16(0, inputImage + offset);
        sum = convert_float16(pixels);
    }

    // Reduce within workgroup
    __local float localSum[256];
    int lid = get_local_id(0) * get_local_size(1) + get_local_id(1);
    localSum[lid] = sum.s0 + sum.s1 + sum.s2 + sum.s3 + sum.s4 + sum.s5 + sum.s6 + sum.s7 +
                    sum.s8 + sum.s9 + sum.sa + sum.sb + sum.sc + sum.sd + sum.se + sum.sf;
    barrier(CLK_LOCAL_MEM_FENCE);

    // Perform reduction in local memory
    for (int stride = get_local_size(0) * get_local_size(1) / 2; stride > 0; stride >>= 1)
    {
        if (lid < stride)
        {
            localSum[lid] += localSum[lid + stride];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Write workgroup sum to global memory
    if (lid == 0)
    {
        partialSums[groupId] = localSum[0];
    }
}