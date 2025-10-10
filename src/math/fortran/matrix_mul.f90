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
contains
  !> Matrix multiplication: C = A * B
  !!
  !! Parameters:
  !!   A(n,k) - left-hand matrix
  !!   B(k,m) - right-hand matrix
  !!   C(n,m) - output matrix (allocated by caller)
  !!   n, k, m - dimensions (rowsA, sharedDim, colsB)
  subroutine matrix_mul(A, B, C, n, k, m) bind(C, name="matrix_mul_f")
    implicit none
    integer, intent(in) :: n, k, m
    real(8), intent(in) :: A(n, k), B(k, m)
    real(8), intent(out) :: C(n, m)
    integer :: i, j, l
    C = 0.0d0
    do i = 1, n
      do j = 1, m
        do l = 1, k
          C(i, j) = C(i, j) + A(i, l) * B(l, j)
        end do
      end do
    end do
  end subroutine matrix_mul
end module matrix_mul_mod
