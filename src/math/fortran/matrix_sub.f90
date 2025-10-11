! matrix_sub.f90
! Fortran implementation of matrix subtraction
!
! This module provides a simple element-wise matrix subtraction routine
! exported with C linkage using bind(C). Arrays are expected to be
! in double precision (real(8)). The dimensions n (rows) and m (columns)
! are passed as integers. The routine performs no allocation and writes
! directly to the output array.
!
! Author: Steven Kight
! Date:   2025-10-09
!
module matrix_sub_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains
  !> Element-wise subtraction: C = A - B
  !!
  !! Parameters:
  !!  A(n,m) - first operand (row-major from C)
  !!  B(n,m) - second operand (row-major from C)
  !!  C(n,m) - result (allocated by caller, row-major)
  subroutine matrix_sub(A, B, C, n, m) bind(C, name="matrix_sub_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in) :: A(*), B(*)
    real(c_double), intent(out) :: C(*)
    integer :: i, j, idx
    do i = 1, n
      do j = 1, m
        idx = (i - 1) * m + (j - 1) + 1
        C(idx) = A(idx) - B(idx)
      end do
    end do
  end subroutine matrix_sub
end module matrix_sub_mod
