! matrix_hadamard.f90
! Fortran implementation of element-wise matrix multiplication (Hadamard product)
!
! Author: Steven Kight
! Date:   2026-04-10
!
module matrix_hadamard_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains
  !> Element-wise multiplication: C = A * B
  !!
  !! Parameters:
  !!   A(n,m)    - first input matrix (row-major from C)
  !!   B(n,m)    - second input matrix (row-major from C)
  !!   C(n,m)    - output matrix (must be allocated by caller, row-major)
  !!   n, m      - matrix dimensions (rows, cols)
  subroutine matrix_hadamard(A, B, C, n, m) bind(C, name="matrix_hadamard_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in) :: A(*), B(*)
    real(c_double), intent(out) :: C(*)
    integer :: i, j, idx
    do i = 1, n
      do j = 1, m
        idx = (i - 1) * m + (j - 1) + 1
        C(idx) = A(idx) * B(idx)
      end do
    end do
  end subroutine matrix_hadamard
end module matrix_hadamard_mod
