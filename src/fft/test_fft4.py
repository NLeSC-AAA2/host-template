#!/usr/bin/env python


import numpy as np
from kernel_tuner import run_kernel



def test_fft_4():
    x = np.random.normal(size=(1024, 4, 2)).astype(np.float32)
    y = np.zeros_like(x)
    for cycle in range(4):
        y_ref = np.fft.fft(np.roll(x[...,0]+1j*x[...,1], -cycle, axis=1))
        _, _, y = run_kernel("test_fft_4", "fft1024_mc_fma.cl", 1, [np.int32(cycle), x, y], {"TESTING": 1, "block_size_x": 1024})
        y_Z = np.roll(y[...,0]+1j*y[...,1], -cycle, axis=1)

        print(y_Z)
        print(y_ref)

        assert abs(y_Z - y_ref).max() < 1e-4


def test_fft_1024():
    x = np.random.normal(size=(1024, 2)).astype(np.float32)
    y = np.zeros_like(x)
    _, y = run_kernel("fft_1024", "fft1024_mc_fma.cl", 1, [x, y], {"TESTING": 1, "block_size_x": 1024})
    y_Z = y[...,0]+1j*y[...,1]
    y_ref = np.fft.fft(x[...,0]+1j*x[...,1])

    print(y_Z)
    print(y_ref)

    assert abs(y_Z - y_ref).max() < 1e-3



if __name__ == "__main__":
    np.set_printoptions(edgeitems=20)

    test_fft_1024()


