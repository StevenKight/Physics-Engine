! matrix_power.f90
! Fortran implementation of element-wise matrix power
!
! This module provides a routine that raises each element of a matrix to a
! given power, exported with C linkage using bind(C). Arrays are expected to
! be real(8) and the dimensions n (rows) and m (columns) are passed as
! integers. The routine performs no allocation and writes results into the
! provided output array C.
!
! Author: Steven Kight
! Date:   2026-04-09
!
module matrix_power_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains
  !> Element-wise power: C = A^power
  !!
  !! Parameters:
  !!   A(n,m)  - input matrix (real(c_double)), row-major from C
  !!   power   - exponent (real(c_double))
  !!   C(n,m)  - output matrix (real(c_double)), row-major, allocated by caller
  !!   n, m    - matrix dimensions (rows, cols)
  subroutine matrix_power(A, power, C, n, m) bind(C, name="matrix_power_f")
    implicit none
    integer(c_int), intent(in)  :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(in)  :: power
    real(c_double), intent(out) :: C(*)
    integer :: i, j, idx
    do i = 1, n
      do j = 1, m
        idx = (i - 1) * m + j
        C(idx) = A(idx) ** power
      end do
    end do
  end subroutine matrix_power
end module matrix_power_mod
