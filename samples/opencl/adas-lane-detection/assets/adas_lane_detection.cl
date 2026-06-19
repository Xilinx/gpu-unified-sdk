/**
 * \brief Kernel to perform Sobel edge detection for lane visualization.
 * \param[in] input Input image data (luminance).
 * \param[in] width Width of the image.
 * \param[in] height Height of the image.
 * \param[out] output Edge-detected image (uchar, 0-255).
 */
__kernel void sobel_edge_detection(__global const uchar* restrict input,
                                  const int width,
                                  const int height,
                                  __global uchar* restrict output)
{
    const int column = get_global_id(0); // One pixel per work-item
    const int row = get_global_id(1);
    const int offset = row * width + column;
    const int halfKernel = 1; // 3x3 Sobel kernel

    if (row < height && column < width)
    {
        float gx = 0.0f, gy = 0.0f;
        const float threshold = 25.0f; // Slightly lower threshold

        // Only process if within kernel bounds
        if (row >= halfKernel && row < height - halfKernel && column >= halfKernel && column < width - halfKernel)
        {
            // Standard Sobel kernels with weights
            for (int ky = -halfKernel; ky <= halfKernel; ky++)
            {
                for (int kx = -halfKernel; kx <= halfKernel; kx++)
                {
                    int idx = (row + ky) * width + (column + kx);
                    // Clamp indices to avoid out-of-bounds access
                    int clampedRow = max(0, min(height - 1, row + ky));
                    int clampedCol = max(0, min(width - 1, column + kx));
                    int clampedIdx = clampedRow * width + clampedCol;
                    float pixel = (float)input[clampedIdx];
                    // Horizontal Sobel: [-1, 0, 1; -2, 0, 2; -1, 0, 1]
                    gx += pixel * (ky == -1 ? (kx == -1 ? -1.0f : kx == 0 ? 0.0f : 1.0f) :
                                  ky == 0  ? (kx == -1 ? -2.0f : kx == 0 ? 0.0f : 2.0f) :
                                             (kx == -1 ? -1.0f : kx == 0 ? 0.0f : 1.0f));
                    // Vertical Sobel: [-1, -2, -1; 0, 0, 0; 1, 2, 1]
                    gy += pixel * (kx == -1 ? (ky == -1 ? -1.0f : ky == 0 ? 0.0f : 1.0f) :
                                  kx == 0  ? (ky == -1 ? -2.0f : ky == 0 ? 0.0f : 2.0f) :
                                             (ky == -1 ? -1.0f : ky == 0 ? 0.0f : 1.0f));
                }
            }
            float magnitude = sqrt(gx * gx + gy * gy);
            output[offset] = (magnitude > threshold) ? (uchar)min(magnitude, 255.0f) : 0; // Direct store
        }
        else
        {
            output[offset] = 0; // Set boundary pixels to black
        }
    }
}