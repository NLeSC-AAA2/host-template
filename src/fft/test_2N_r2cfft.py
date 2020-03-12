#!/usr/bin/env python

import numpy as np

from kernel_tuner import run_kernel


def fix_2n(c, N):
    Xa_r, Xa_i, Xb_r, Xb_i = np.zeros((4, N//2), dtype=np.float32)

    for i in range(1,N//2):
        Xa_r[i] = (c[N-i].real + c[i].real) / 2
        Xa_i[i] = (c[i].imag - c[N-i].imag) / 2
        Xb_r[i] = (c[N-i].imag + c[i].imag) / 2
        Xb_i[i] = (c[N-i].real - c[i].real) / 2

    Xa_r[0] = c[0].real
    Xb_r[0] = c[0].imag

    return Xa_r, Xa_i, Xb_r, Xb_i

def test_2N_r2cfft():
    N = 1024

    signal1, signal2 = np.random.normal(size=(2, N)).astype(np.float32)
    x = np.c_[signal1,signal2]
    y = np.zeros_like(x)
    _, y = run_kernel("fft_1024", "fft1024_mc_fma.cl", 1, [x, y], {"TESTING": 1, "block_size_x": 1024})

    c = y[...,0]+1j*y[...,1]
    Xa_r, Xa_i, Xb_r, Xb_i = fix_2n(c, N)
    signal1_ans = Xa_r+1j*Xa_i
    signal2_ans = Xb_r+1j*Xb_i

    signal1_ref = np.fft.rfft(signal1)[:-1]
    signal2_ref = np.fft.rfft(signal2)[:-1]

    print(signal1_ans)
    print(signal1_ref)

    assert abs(signal1_ans - signal1_ref).max() < 1e-3

    print(signal2_ans)
    print(signal2_ref)

    assert abs(signal2_ans - signal2_ref).max() < 1e-3


def test_2n():
    N = 1024

    signal1, signal2 = np.random.normal(size=(2, N)).astype(np.float32)
    x = signal1 + 1j * signal2

    y = np.fft.fft(x).astype(np.complex64)

    Xa, Xb = np.zeros((2, N), dtype=np.float32)
    _, Xa, Xb = run_kernel("test_fix_2n", "fft1024_mc_fma.cl", 1, [y, Xa, Xb], {"TESTING": 1, "block_size_x": 1})

    signal1_ref = np.fft.rfft(signal1)[:-1]
    signal2_ref = np.fft.rfft(signal2)[:-1]

    print(Xa)
    print(signal1_ref)

    assert abs(Xa.view(np.complex64) - signal1_ref).max() < 1e-3

    print(Xb)
    print(signal2_ref)

    assert abs(Xb.view(np.complex64) - signal2_ref).max() < 1e-3

