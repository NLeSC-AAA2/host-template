/* Generated by: ./genfft/gen_twiddle.native -n 2 -name twiddle_2 -compact -standalone -opencl */

/*
 * This function contains 6 FP additions, 4 FP multiplications,
 * (or, 4 additions, 2 multiplications, 2 fused multiply/add),
 * 9 stack variables, 0 constants, and 8 memory accesses
 */
void
twiddle_2 (R * restrict ri, R * restrict ii, __constant R * restrict W, stride rs, INT mb, INT me,
           INT ms)
{
  {
    INT m;
    #pragma ivdep
    for (m = mb; m < me; m++) {
        E T1, T8, T6, T7;
        T1 = ri[m*ms];
        T8 = ii[m*ms];
        {
          E T3, T5, T2, T4;
          T3 = ri[m*ms+WS (rs, 1)];
          T5 = ii[m*ms+WS (rs, 1)];
          T2 = W[(m*2)+0];
          T4 = W[(m*2)+1];
          T6 = FMA (T2, T3, T4 * T5);
          T7 = FNMS (T4, T3, T2 * T5);
        }
        ri[m*ms+WS (rs, 1)] = T1 - T6;
        ii[m*ms+WS (rs, 1)] = T8 - T7;
        ri[m*ms] = T1 + T6;
        ii[m*ms] = T7 + T8;
      }
  }
}


