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
contains
  !> Element-wise addition: C = A + B
  !!
  !! Parameters:
  !!   A(n,m)    - input matrix
  !!   B(n,m)    - input matrix
  !!   C(n,m)    - output matrix (must be allocated by caller)
  !!   n, m      - matrix dimensions (rows, cols)
  subroutine matrix_add(A, B, C, n, m) bind(C, name="matrix_add_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m), B(n, m)
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) + B(i, j)
      end do
    end do
  end subroutine matrix_add
end module matrix_add_mod
