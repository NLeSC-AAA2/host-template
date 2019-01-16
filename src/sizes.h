/* These are the constants needed for the FPGA program, changing these requires
   FPGA & host recompile */

// can be 1, 4, 8 or 16
#define VECTOR_SIZE      16L
#define vec_type float4
#define COMPUTE_ROWS 8
#define COMPUTE_COLS 8
#define INTERLEAVE 16

#define INTERLEAVE_SQUARED (INTERLEAVE * INTERLEAVE)

#define BLOCK_ROWS  (INTERLEAVE * COMPUTE_ROWS)
#define BLOCK_COLS   (INTERLEAVE * COMPUTE_COLS)

#define BLOCK_WIDTH_IN_VECTORS (BLOCK_WIDTH / DOT_PROD_VECTOR_SIZE)
#define BLOCK_WIDTH_IN_COLLUMN_GROUPS INTERLEAVE // = BLOCK_WIDTH  / SYS_ARRAY_NUM_COLS

#define MUST_BE_ZERO (BLOCK_WIDTH % VECTOR_SIZE)

#if MUST_BE_ZERO != 0
#error "DOT_PROD_VECTOR_SIZE does not divide BLOCK_WIDTH"
#endif

/* These are the constants for the host program. Chaning these requires only
   a host recompile */

#define NR_BLOCKS_X 64
#define NR_BLOCKS_Y 64
#define NR_VECTORS_K (64 * 8)
#define LENGTH_K (NR_VECTORS_K * VECTOR_SIZE)


#define MAT_C_WIDTH  (NR_BLOCKS_X * BLOCK_COLS)
#define MAT_C_HEIGHT (NR_BLOCKS_Y * BLOCK_ROWS)

#define FLOPS (MAT_C_HEIGHT * MAT_C_WIDTH * MAT_A_WIDTH) * 2 // One multiply and one add

#define MAT_A_WIDTH (NR_VECTORS_K * VECTOR_SIZE)
#define MAT_A_HEIGHT MAT_C_HEIGHT

#define MAT_B_WIDTH  MAT_C_WIDTH
#define MAT_B_HEIGHT MAT_A_WIDTH

#define MAT_B_WIDTH_IN_VECTORS (NR_BLOCKS_X * BLOCK_WIDTH_IN_VECTORS)
#define MAT_B_SIZE_IN_VECTORS (MAT_B_WIDTH_IN_VECTORS * MAT_B_HEIGHT)

#define MAT_C_WIDTH_IN_COLLUMN_GROUPS (MAT_C_WIDTH / SYS_ARRAY_NUM_COLS)
