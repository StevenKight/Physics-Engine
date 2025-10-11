! matrix_mul.f90
! Fortran implementation of matrix multiplication
!
! This module provides an implementation of matrix multiplication.
! The matrices are assumed to be in double precision (real(8)).
! The subroutine is exported with C linkage using bind(C) so
! it can be called from C/C++ code.
!
! Author: Steven Kight
! Date:   2025-10-09
!
module matrix_mul_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains
  !> Matrix multiplication: C = A * B
  !!
  !! Parameters:
  !!   A(n,k) - left-hand matrix (row-major from C)
  !!   B(k,m) - right-hand matrix (row-major from C)
  !!   C(n,m) - output matrix (row-major)
  !!   n, k, m - dimensions (rowsA, sharedDim, colsB)
  subroutine matrix_mul(A, B, C, n, k, m) bind(C, name="matrix_mul_f")
    implicit none
    integer(c_int), intent(in) :: n, k, m
    real(c_double), intent(in) :: A(*), B(*)
    real(c_double), intent(out) :: C(*)
    integer :: i, j, l, c_idx, a_idx, b_idx
    ! Initialize output (remember swapped dims)
    do i = 1, n*m
      C(i) = 0.0d0
    end do
    ! Perform multiplication
    do i = 1, n
      do j = 1, m
        do l = 1, k
          c_idx = (j - 1) * n + (i - 1) + 1
          a_idx = (l - 1) * n + (i - 1) + 1
          b_idx = (j - 1) * k + (l - 1) + 1
          C(c_idx) = C(c_idx) + A(a_idx) * B(b_idx)
        end do
      end do
    end do
  end subroutine matrix_mul
end module matrix_mul_mod
