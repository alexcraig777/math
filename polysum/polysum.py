""" Sum polynomials """

import numpy as np

def make_A(n):
    """ Generate the matrix `A` such that `A u = x` """
    A = np.zeros((n + 1, n + 1))
    A[0, 0] = 1
    for k in range(1, n + 1):
        A[:, k] = (A[:, k - 1] * (k - 1) / k
                   + np.roll(A[:, k - 1], 1) / k)
    return A


def polysum(poly):
    """ Computes the sum from k=0 to n of `P(k)`

    `poly` is the list of coefficients of `P`, starting with the
    constant term """
    x = np.array(poly + [0])
    A = make_A(len(poly))
    u = np.linalg.solve(A, x)
    v = np.roll(u, 1)
    y = np.dot(A, v)
    return list(y)


def print_poly(poly):
    """ Print out a polynomial in nice format """
    for i, c in enumerate(poly):
        c = round(c, 4)
        if not c:
            continue
        if i == 0:
            power = ""
        elif i == 1:
            power = "x"
        else:
            power = f"x^{i}"
        print(f" {c}*{power}", end = " ")
    print()
