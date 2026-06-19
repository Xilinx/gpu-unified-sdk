#define VECS_PER_ROW (N / 4)
#define TS WG_SQUARE_SIDE
#define TSM TILE_RECT_M
#define TSN TILE_RECT_N
#define TSP TILE_RECT_P
#define FETCHES_A (TSN/TSP)
#define FETCHES_BT (TSN/TSM)
#define TILE_WIDTH_VEC (TSN/4)
#define NTILES ((N/4)/TILE_WIDTH_VEC)
#define FETCHES_A4 ((TILE_WIDTH_VEC/TSP))
#define FETCHES_BT4 ((TILE_WIDTH_VEC/TSM))

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_naive(const __global float A[M][N], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += A[i][k] * B[k][j];
    }
    C[i][j] = sum;
}

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_transposedA(const __global float AT[N][M], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += AT[k][i] * B[k][j];
    }
    C[i][j] = sum;
}

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_transposedB(__global float A[M][N], __global float BT[P][N], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += A[i][k] * BT[j][k];
    }
    C[i][j] = sum;
}

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_transposedC(const __global float A[M][N], const __global float B[N][P], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += A[i][k] * B[k][j];
    }
    CT[j][i] = sum;
}

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_transposedAC(const __global float AT[N][M], const __global float B[N][P], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += AT[k][i] * B[k][j];
    }
    CT[j][i] = sum;
}

__attribute__((reqd_work_group_size(WG_RECT_WIDTH,WG_RECT_HEIGHT,1)))
__kernel void matmul_transposedBC(__global float A[M][N], __global float BT[P][N], __global float CT[P][M]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0;
    for (int k = 0; k < N; ++k) {
        sum += A[i][k] * BT[j][k];
    }
    CT[j][i] = sum;
}

__attribute__((reqd_work_group_size(1,WG_LINEAR_SIZE,1)))
__kernel void matmul_linearwg_AT(const __global float AT[N][M], const __global float B[N][P], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; ++k) {
        sum += AT[k][i] * B[k][j];
    }
    C[i][j] = sum;
}

__attribute__((reqd_work_group_size(1,WG_LINEAR_SIZE,1)))
__kernel void matmul_linearwg_BT(__global float A[M][N], __global float BT[P][N], __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float sum = 0.0f;
    for (int k = 0; k < N; ++k) {
        sum += A[i][k] * BT[j][k];
    }
    C[i][j] = sum;
}

__kernel void matmul_linearwg_vec4(__global float4* A, __global float4* BT, __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    float4 sum = (float4)(0,0,0,0);
    for (int k = 0; k < N/4; ++k) {
        sum += A[i*(N/4)+k] * BT[j*(N/4)+k];
    }
    C[i][j] = sum.x + sum.y + sum.z + sum.w;
}

__attribute__((reqd_work_group_size(1,WG_LINEAR_SIZE,1)))
__kernel void matmul_linearwg_vec4_local(__global float4* A, __global float4* BT, __global float C[M][P]) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    int lid = get_local_id(1);
    __local float4 localA[VECS_PER_ROW];
    event_t evt = async_work_group_copy(localA, &(A[VECS_PER_ROW * i]), VECS_PER_ROW, 0);
    wait_group_events(1, &evt);
    barrier(CLK_LOCAL_MEM_FENCE);
    float4 sum = (float4)(0,0,0,0);
    for (int k = 0; k < VECS_PER_ROW; ++k) {
        sum += localA[k] * BT[j*(N/4)+k];
    }
    C[i][j] = sum.x + sum.y + sum.z + sum.w;
}

__kernel void matmul_tile_square(const __global float A[M][N], const __global float BT[P][N], __global float C[M][P]) {
    int row = get_local_id(0);
    int col = get_local_id(1);
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    __local float Asub[TS][TS];
    __local float Bsub[TS][TS];
    float acc = 0.0f;
    int numTiles = N/TS;
    for (int t = 0; t < numTiles; t++) {
        int tiledRow = TS*t + row;
        int tiledCol = TS*t + col;
        Asub[row][col] = A[globalRow][tiledCol];
        Bsub[row][col] = BT[globalCol][tiledRow];
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int k = 0; k < TS; k++) {
            acc += Asub[row][k] * Bsub[k][col];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    C[globalRow][globalCol] = acc;
}

__attribute__((reqd_work_group_size(TSM,TSP,1)))
__kernel void matmul_tile_rect(const __global float A[M][N], const __global float BT[P][N], __global float C[M][P]) {
    int row = get_local_id(0);
    int col = get_local_id(1);
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    __local float Asub[TSM][TSN];
    __local float BTsub[TSP][TSN];
    float acc = 0.0f;
    int numTiles = N/TSN;
    for (int t = 0; t < numTiles; t++) {
        int tiledColFetch = TSN*t + col;
        int tiledRowFetch = TSN*t + row;
        for (int m = 0; m < FETCHES_A; ++m) {
            Asub[row][col + m * TSP] = A[globalRow][tiledColFetch + m * TSP];
        }
        for (int p = 0; p < FETCHES_BT; ++p) {
            BTsub[col][row + p * TSM] = BT[globalCol][tiledRowFetch + p * TSM];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int k = 0; k < TSN; ++k) {
            acc += Asub[row][k] * BTsub[col][k];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    C[globalRow][globalCol] = acc;
}

__kernel void matmul_tile_rect_vec4(const __global float4 A4[M][N/4], const __global float4 BT4[P][N/4], __global float C[M][P]) {
    int row = get_local_id(0);
    int col = get_local_id(1);
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    __local float4 Asub[TSM][TILE_WIDTH_VEC];
    __local float4 BTsub[TSP][TILE_WIDTH_VEC];
    float4 acc = (float4)(0.0f,0.0f,0.0f,0.0f);
    for (int t = 0; t < NTILES; t++) {
        int tiledColFetch = TILE_WIDTH_VEC*t + col;
        int tiledRowFetch = TILE_WIDTH_VEC*t + row;
        for (int m = 0; m < FETCHES_A4; ++m) {
            Asub[row][col + m * TSP] = A4[globalRow][tiledColFetch + m * TSP];
        }
        for (int p = 0; p < FETCHES_BT4; ++p) {
            BTsub[col][row + p * TSM] = BT4[globalCol][tiledRowFetch + p * TSM];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int k = 0; k < TILE_WIDTH_VEC; k++) {
            acc += (Asub[row][k] * BTsub[col][k]);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    C[globalRow][globalCol] = acc.x + acc.y + acc.z + acc.w;
}

__attribute__((reqd_work_group_size(TSM,TSP,1)))
__kernel void matmul_optimized(const __global float4 A4[M][N/4], const __global float4 BT4[P][N/4], __global float C[M][P], 
                              const int m, const int n, const int p) {
    const int row = get_local_id(0);
    const int col = get_local_id(1);
    const int globalRow = get_global_id(0);
    const int globalCol = get_global_id(1);

    __local float4 Asub[TSM][TILE_WIDTH_VEC];
    __local float4 BTsub[TSP][TILE_WIDTH_VEC];

    float4 acc = (float4)(0.0f);

    for (int t = 0; t < NTILES; t++) {
        int tiledColFetch = TILE_WIDTH_VEC * t + col;
        int tiledRowFetch = TILE_WIDTH_VEC * t + row;

        // Direct indexing for coalesced access
        for (int i = 0; i < FETCHES_A4; ++i) {
            Asub[row][col + i * TSP] = A4[globalRow][tiledColFetch + i * TSP];
        }
        for (int i = 0; i < FETCHES_BT4; ++i) {
            BTsub[col][row + i * TSM] = BT4[globalCol][tiledRowFetch + i * TSM];
        }

        barrier(CLK_LOCAL_MEM_FENCE);

        // Fully unroll the inner loop
        #pragma unroll
        for (int k = 0; k < TILE_WIDTH_VEC; k++) {
            acc += Asub[row][k] * BTsub[col][k];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (globalRow < m && globalCol < p) {
        C[globalRow][globalCol] = acc.x + acc.y + acc.z + acc.w;
    }
}


