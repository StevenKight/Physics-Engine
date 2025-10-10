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
contains
  !> Element-wise subtraction: C = A - B
  !!
  !! Parameters:
  !!  A(n,m) - first operand
  !!  B(n,m) - second operand
  !!  C(n,m) - result (allocated by caller)
  subroutine matrix_sub(A, B, C, n, m) bind(C, name="matrix_sub_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m), B(n, m)
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) - B(i, j)
      end do
    end do
  end subroutine matrix_sub
end module matrix_sub_mod
