// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

// Host-provided macros:
// - M: Rows of A and C
// - N: Columns of A, rows of B
// - P: Columns of B and C
// - WG_RECT_WIDTH, WG_RECT_HEIGHT: Workgroup dimensions for rectangular kernels
// - WG_LINEAR_SIZE: Workgroup size for linear kernels
// - WG_SQUARE_SIDE: Workgroup size for square tiled kernels
// - TILE_SQUARE: Tile size for square tiling
// - TILE_RECT_M, TILE_RECT_N, TILE_RECT_P: Tile sizes for rectangular tiling

// Matrix dimensions:
// - A: M � N
// - B: N � P
// - C: M � P
// - AT: N � M (transposed A)
// - BT: P � N (transposed B)
// - CT: P � M (transposed C)

// -----------------------------
// Basic Multiplication Kernels
// -----------------------------

// Task: Compute C = A * B, each work-item computes one output element
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_naive(const __global float A[M][N], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0); // Row: 0 to M-1
    int j = get_global_id(1); // Col: 0 to P-1
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += A[i][k] * B[k][j];
    }
    C[i][j] = sum;
}

// Task: Compute C = AT * B, A is transposed
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_transposedA(const __global float AT[N][M], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += AT[k][i] * B[k][j];
    }
    C[i][j] = sum;
}

// Task: Compute C = A * BT, B is transposed
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_transposedB(__global float A[M][N], __global float BT[P][N], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += A[i][k] * BT[j][k];
    }
    C[i][j] = sum;
}

// Task: Compute CT = A * B, output C is transposed
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_transposedC(const __global float A[M][N], const __global float B[N][P], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += A[i][k] * B[k][j];
    }
    CT[j][i] = sum;
}

// Task: Compute CT = AT * B, A and C are transposed
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_transposedAC(const __global float AT[N][M], const __global float B[N][P], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += AT[k][i] * B[k][j];
    }
    CT[j][i] = sum;
}

// Task: Compute CT = A * BT, B and C are transposed
__attribute__((reqd_work_group_size(WG_RECT_WIDTH, WG_RECT_HEIGHT, 1)))
__kernel void matmul_transposedBC(__global float A[M][N], __global float BT[P][N], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += A[i][k] * BT[j][k];
    }
    CT[j][i] = sum;
}

// -----------------------------
// Linear Workgroup Kernels
// -----------------------------

// Task: Compute C = AT * B with 1D workgroup (1 � WG_LINEAR_SIZE)
__attribute__((reqd_work_group_size(1, WG_LINEAR_SIZE, 1)))
__kernel void matmul_linearwg_AT(const __global float AT[N][M], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; k++) {
        sum += AT[k][i] * B[k][j];
    }
    C[i][j] = sum;
}

// Task: Compute C = A * BT with 1D workgroup, fixed for verification
__attribute__((reqd_work_group_size(1, WG_LINEAR_SIZE, 1)))
__kernel void matmul_linearwg_BT(__global float A[M][N], __global float BT[P][N], __global float C[M][P]) {
    int i = get_global_id(0); // Row: 0 to M-1
    int j = get_global_id(1); // Col: 0 to P-1
    float sum = 0.0f;
    if (i < M && j < P) { // Bounds check
        for (int k = 0; k < N; k++) {
            sum = fma(A[i][k], BT[j][k], sum);
        }
        C[i][j] = sum;
    }
}

// Task: Compute C = A * BT with float4 inputs, 1D workgroup
__kernel void matmul_linearwg_vec4(__global float4* A, __global float4* BT, __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    if (i < M && j < P) {
        float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for (int k = 0; k < N / 4; k++) {
            acc += A[i * (N / 4) + k] * BT[j * (N / 4) + k];
        }
        sum = acc.x + acc.y + acc.z + acc.w;
        C[i][j] = sum;
    }
}

// Task: Compute C = A * BT with float4 inputs, preload A row to local memory
#define VECS_PER_ROW (N / 4)
__attribute__((reqd_work_group_size(1, WG_LINEAR_SIZE, 1)))
__kernel void matmul_linearwg_vec4_local(__global float4* A, __global float4* BT, __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    int lid = get_local_id(1);
    __local float4 localA[VECS_PER_ROW];
    if (i < M && j < P) {
        event_t evt = async_work_group_copy(localA, &A[i * VECS_PER_ROW], VECS_PER_ROW, 0);
        wait_group_events(1, &evt);
        barrier(CLK_LOCAL_MEM_FENCE);
        float sum = 0.0f;
        float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for (int k = 0; k < VECS_PER_ROW; k++) {
            acc += localA[k] * BT[j * VECS_PER_ROW + k];
        }
        sum = acc.x + acc.y + acc.z + acc.w;
        C[i][j] = sum;
    }
}

// -----------------------------
// Tiled Multiplication Kernels
// -----------------------------

// Task: Square tiled multiplication with local memory for A and BT
#define TS WG_SQUARE_SIDE
__kernel void matmul_tile_square(const __global float A[M][N], const __global float BT[P][N], __global float C[M][P]) {
    int row = get_local_id(0); // Local row: 0 to TS-1
    int col = get_local_id(1); // Local col: 0 to TS-1
    int globalRow = get_global_id(0); // Global row: 0 to M-1
    int globalCol = get_global_id(1); // Global col: 0 to P-1
    __local float Asub[TS][TS];
    __local float Bsub[TS][TS];
    float acc = 0.0f;
    if (globalRow < M && globalCol < P) {
        for (int t = 0; t < N / TS; t++) {
            int tileCol = t * TS + col;
            int tileRow = t * TS + row;
            Asub[row][col] = (tileCol < N) ? A[globalRow][tileCol] : 0.0f;
            Bsub[col][row] = (tileRow < N) ? BT[globalCol][tileRow] : 0.0f;
            barrier(CLK_LOCAL_MEM_FENCE);
            for (int k = 0; k < TS; k++) {
                acc = fma(Asub[row][k], Bsub[col][k], acc);
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        C[globalRow][globalCol] = acc;
    }
}

// Task: Rectangular tiled multiplication with local memory
#define TSM TILE_RECT_M
#define TSN TILE_RECT_N
#define TSP TILE_RECT_P
#define FETCHES_A (TSN / TSP)
#define FETCHES_BT (TSN / TSM)
__attribute__((reqd_work_group_size(TSM, TSP, 1)))
__kernel void matmul_tile_rect(const __global float A[M][N], const __global float BT[P][N], __global float C[M][P]) {
    int row = get_local_id(0);
    int col = get_local_id(1);
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    __local float Asub[TSM][TSN];
    __local float BTsub[TSP][TSN];
    float acc = 0.0f;
    if (globalRow < M && globalCol < P) {
        for (int t = 0; t < N / TSN; t++) {
            for (int m = 0; m < FETCHES_A; m++) {
                int idx = t * TSN + col + m * TSP;
                Asub[row][col + m * TSP] = (idx < N) ? A[globalRow][idx] : 0.0f;
            }
            for (int p = 0; p < FETCHES_BT; p++) {
                int idx = t * TSN + row + p * TSM;
                BTsub[col][row + p * TSM] = (idx < N) ? BT[globalCol][idx] : 0.0f;
            }
            barrier(CLK_LOCAL_MEM_FENCE);
            for (int k = 0; k < TSN; k++) {
                acc = fma(Asub[row][k], BTsub[col][k], acc);
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        C[globalRow][globalCol] = acc;
    }
}

// Task: Rectangular tiled multiplication with float4 inputs
#define TILE_WIDTH_VEC (TSN / 4)
#define NTILES ((N / 4) / TILE_WIDTH_VEC)
__kernel void matmul_tile_rect_vec4(const __global float4 A4[M][N/4], const __global float4 BT4[P][N/4], __global float C[M][P]) {
    int row = get_local_id(0);
    int col = get_local_id(1);
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    __local float4 Asub[TSM][TILE_WIDTH_VEC];
    __local float4 BTsub[TSP][TILE_WIDTH_VEC];
    float acc = 0.0f;
    if (globalRow < M && globalCol < P) {
        for (int t = 0; t < NTILES; t++) {
            for (int m = 0; m < TILE_WIDTH_VEC / TSP; m++) {
                int idx = t * TILE_WIDTH_VEC + col + m * TSP;
                Asub[row][col + m * TSP] = (idx < N / 4) ? A4[globalRow][idx] : (float4)(0.0f);
            }
            for (int p = 0; p < TILE_WIDTH_VEC / TSM; p++) {
                int idx = t * TILE_WIDTH_VEC + row + p * TSM;
                BTsub[col][row + p * TSM] = (idx < N / 4) ? BT4[globalCol][idx] : (float4)(0.0f);
            }
            barrier(CLK_LOCAL_MEM_FENCE);
            for (int k = 0; k < TILE_WIDTH_VEC; k++) {
                float4 prod = Asub[row][k] * BTsub[col][k];
                acc += prod.x + prod.y + prod.z + prod.w;
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        C[globalRow][globalCol] = acc;
    }
}
