! matrix_add.f90
! Fortran implementation of matrix addition
!
! This module provides a simple element-wise matrix addition routine
! exported with C linkage using bind(C). Arrays are expected to be
! real(8) and the dimensions n (rows) and m (columns) are passed as
! integers. The routine performs no allocation and writes results into
! the provided output array C.
!
! Author: Steven Kight
! Date:   2025-10-09
!
module matrix_add_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains
  !> Element-wise addition: C = A + B
  !!
  !! Parameters:
  !!   A(n,m)    - input matrix (row-major from C)
  !!   B(n,m)    - input matrix (row-major from C)
  !!   C(n,m)    - output matrix (must be allocated by caller, row-major)
  !!   n, m      - matrix dimensions (rows, cols)
  subroutine matrix_add(A, B, C, n, m) bind(C, name="matrix_add_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in) :: A(*), B(*)
    real(c_double), intent(out) :: C(*)
    integer :: i, j, idx
    do i = 1, n
      do j = 1, m
        idx = (i - 1) * m + (j - 1) + 1
        C(idx) = A(idx) + B(idx)
      end do
    end do
  end subroutine matrix_add
end module matrix_add_mod
