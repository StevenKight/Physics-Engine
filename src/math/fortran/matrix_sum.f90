! matrix_sum.f90
! Fortran implementations of matrix row and column sum operations
!
! This module provides routines for summing rows and columns of a matrix,
! exported with C linkage using bind(C). Arrays are expected to be real(8)
! and the dimensions n (rows) and m (columns) are passed as integers. The
! routines perform no allocation and write results into the provided output
! array C.
!
! Author: Steven Kight
! Date:   2026-04-10
!
module matrix_sum_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains

  !> Sum each row into a column vector: R(i) = sum_j A(i,j)
  !!
  !! Parameters:
  !!   A(n,m)  - input matrix (real(c_double)), row-major from C
  !!   R(n)    - output vector (real(c_double)), length n, allocated by caller
  !!   n, m    - matrix dimensions (rows, cols)
  subroutine matrix_row_sum(A, R, n, m) bind(C, name="matrix_row_sum_f")
    implicit none
    integer(c_int), intent(in)  :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: R(*)
    integer :: i, j
    real(c_double) :: s
    do i = 1, n
      s = 0.0d0
      do j = 1, m
        s = s + A((i - 1) * m + j)
      end do
      R(i) = s
    end do
  end subroutine matrix_row_sum

  !> Sum each column into a row vector: R(j) = sum_i A(i,j)
  !!
  !! Parameters:
  !!   A(n,m)  - input matrix (real(c_double)), row-major from C
  !!   R(m)    - output vector (real(c_double)), length m, allocated by caller
  !!   n, m    - matrix dimensions (rows, cols)
  subroutine matrix_col_sum(A, R, n, m) bind(C, name="matrix_col_sum_f")
    implicit none
    integer(c_int), intent(in)  :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: R(*)
    integer :: i, j
    real(c_double) :: s
    do j = 1, m
      s = 0.0d0
      do i = 1, n
        s = s + A((i - 1) * m + j)
      end do
      R(j) = s
    end do
  end subroutine matrix_col_sum

end module matrix_sum_mod
