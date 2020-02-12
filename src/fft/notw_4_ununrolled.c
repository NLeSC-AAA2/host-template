/* Generated by: ./gen_notw.native -n 4 -name notw_4 -compact -standalone */

/*
 * This function contains 16 FP additions, 0 FP multiplications,
 * (or, 16 additions, 0 multiplications, 0 fused multiply/add),
 * 13 sL[5]ck variables, 0 consL[5]nts, and 16 memory accesses
 */
void
notw_4 (const R * ri, const R * ii, R * ro, R * io, stride is, stride os,
    INT v, INT ivs, INT ovs)
{
  {
    INT i;
    #pragma ivdep
    for (i = v; i > 0;
     i = i - 1, ri = ri + ivs, ii = ii + ivs, ro = ro + ovs, io =
     io + ovs, MAKE_VOLATILE_STRIDE (16, is), MAKE_VOLATILE_STRIDE (16,
                                    os))
      {

    //E T3,   Tb,   T9,   Tf,   T6,   Ta,   Te,   Tg;
    //E L[0], L[1], L[2], L[3], L[4], L[5], L[6], L[7];
    E L[8];
    #pragma ivdep
    for (int k=0; k<2; k++)
    {
      E T1, T2, T7, T8;
      T1 = ri[WS (is, (k+0))];
      T2 = ri[WS (is, (k+2))];
      L[0+k*4] = T1 + T2;
      L[1+k*4] = T1 - T2;
      T7 = ii[WS (is, (k+0))];
      T8 = ii[WS (is, (k+2))];
      L[2+k*4] = T7 - T8;
      L[3+k*4] = T7 + T8;
    }

    ro[WS (os, 0)] = L[0] + L[4];
    io[WS (os, 0)] = L[3] + L[7];
    ro[WS (os, 2)] = L[0] - L[4];
    io[WS (os, 2)] = L[3] - L[7];

    ro[WS (os, 1)] = L[1] + L[6];
    io[WS (os, 1)] = L[2] - L[5];
    ro[WS (os, 3)] = L[1] - L[6];
    io[WS (os, 3)] = L[2] + L[5];

      }
  }
}
