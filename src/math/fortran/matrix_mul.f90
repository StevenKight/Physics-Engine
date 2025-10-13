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

    ! Arguments passed by reference from C
    integer(c_int), intent(in) :: n, k, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(in)  :: B(*)
    real(c_double), intent(out) :: C(*)

    integer :: i, j, p
    integer :: idxA, idxB, idxC
    real(c_double) :: sum

    ! Matrix multiply: row-major order (C-style)
    do i = 0, n - 1
       do j = 0, m - 1
          sum = 0.0_c_double
          do p = 0, k - 1
             idxA = i * k + p
             idxB = p * m + j
             sum  = sum + A(idxA + 1) * B(idxB + 1)
          end do
          idxC = i * m + j
          C(idxC + 1) = sum
       end do
    end do

  end subroutine matrix_mul

end module matrix_mul_mod
