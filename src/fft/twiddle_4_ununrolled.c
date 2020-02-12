/* Generated by: ./gen_twiddle.native -n 4 -name twiddle_4 -compact -standalone */

/*
 * This function contains 22 FP additions, 12 FP multiplications,
 * (or, 16 additions, 6 multiplications, 6 fused multiply/add),
 * 13 stack variables, 0 constants, and 16 memory accesses
 */
void
twiddle_4 (R * ri, R * ii, __constant R * W, stride rs, INT mb, INT me, INT ms)
{
  {
    INT m;
    #pragma ivdep
    for (m = mb, W = W + (mb * 6); m < me;
     m = m + 1, ri = ri + ms, ii = ii + ms, W =
     W + 6, MAKE_VOLATILE_STRIDE (8, rs))
      {

    //E T1,   Tp,   T6,   To,   Tc,   Tk,   Th,   Tl;
    //E L[0], L[1], L[2], L[3], L[4], L[5], L[6], L[7];
    //E L[0], L[1], L[4], L[5], L[2], L[3], L[6], L[7];
    E L[8];

    L[0] = ri[0];
    L[1] = ii[0];

    #pragma ivdep
    for (int k=0; k<3; k++)
    {
      E T9, Tb, T8, Ta;
      T9 = ri[WS (rs, (k+1))];
      Tb = ii[WS (rs, (k+1))];
      T8 = W[k*2+0];
      Ta = W[k*2+1];
      L[2+k*2] = FMA (T8, T9, Ta * Tb);
      L[3+k*2] = FNMS (Ta, T9, T8 * Tb);
    }

    //L[4] and L[5] has been swapped with L[2] L[3]
    {
      //E T7, Ti, Tn, Tq;
      E T0, T1, T2, T3;
      T0 = L[0] + L[4]; // L[0] + L[2]
      T1 = L[2] + L[6]; // L[4] + L[6]
      ri[WS (rs, 2)] = T0 - T1;
      ri[0] = T0 + T1;

      T2 = L[3] + L[7]; //L[5] + L[7]
      T3 = L[5] + L[1]; //L[3] + L[1]
      ii[0] = T2 + T3;
      ii[WS (rs, 2)] = T3 - T2;
    }
    {
      //E Tj, Tm, Tr, Ts;
      E T0, T1, T2, T3;
      T0 = L[0] - L[4]; //L[0] - L[2]
      T1 = L[3] - L[7]; //L[5] - L[7]
      ri[WS (rs, 3)] = T0 - T1;
      ri[WS (rs, 1)] = T0 + T1;

      T2 = L[1] - L[5]; //L[1] - L[3]
      T3 = L[2] - L[6]; //L[4] - L[6]
      ii[WS (rs, 1)] = T2 - T3;
      ii[WS (rs, 3)] = T3 + T2;
    }


      }
  }
}
