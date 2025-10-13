! matrix_scalar.f90
! Fortran implementation of matrix-scalar operations
!
! This module implements element-wise scalar operations on matrices using
! double precision (real(8)). All subroutines are exported with
! bind(C) so they can be called from C/C++ or other languages that support
! the C ABI. The routines perform no dynamic allocation and expect the
! caller to provide appropriately sized arrays.
!
! Author: Steven Kight
! Date:   2025-10-09
!
module matrix_scalar_mod
  use iso_c_binding, only: c_double, c_int
  implicit none
contains

  !> Multiply matrix by scalar: C = A * scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(c_double)), row-major from C
  !!   scalar   - input scalar (real(c_double))
  !!   C(n,m)   - output matrix (real(c_double)), row-major
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_mul(A, scalar, C, n, m) bind(C, name="matrix_scalar_mul_f")
    implicit none
    integer(c_int), intent(in) :: n, m           ! passed by reference from C
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: C(*)
    real(c_double), intent(in)  :: scalar
    integer :: i, j, idx

    do i = 1, n
       do j = 1, m
          idx = (i - 1) * m + j
          C(idx) = A(idx) * scalar
       end do
    end do
  end subroutine matrix_scalar_mul


  !> Divide matrix by scalar: C = A / scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(c_double))
  !!   scalar   - input scalar (real(c_double))
  !!   C(n,m)   - output matrix (real(c_double))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_div(A, scalar, C, n, m) bind(C, name="matrix_scalar_div_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: C(*)
    real(c_double), intent(in)  :: scalar
    integer :: i, j, idx

    do i = 1, n
       do j = 1, m
          idx = (i - 1) * m + j
          C(idx) = A(idx) / scalar
       end do
    end do
  end subroutine matrix_scalar_div


  !> Add scalar to matrix: C = A + scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(c_double))
  !!   scalar   - input scalar (real(c_double))
  !!   C(n,m)   - output matrix (real(c_double))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_add(A, scalar, C, n, m) bind(C, name="matrix_scalar_add_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: C(*)
    real(c_double), intent(in)  :: scalar
    integer :: i, j, idx

    do i = 1, n
       do j = 1, m
          idx = (i - 1) * m + j
          C(idx) = A(idx) + scalar
       end do
    end do
  end subroutine matrix_scalar_add


  !> Subtract scalar from matrix: C = A - scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(c_double))
  !!   scalar   - input scalar (real(c_double))
  !!   C(n,m)   - output matrix (real(c_double))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_sub(A, scalar, C, n, m) bind(C, name="matrix_scalar_sub_f")
    implicit none
    integer(c_int), intent(in) :: n, m
    real(c_double), intent(in)  :: A(*)
    real(c_double), intent(out) :: C(*)
    real(c_double), intent(in)  :: scalar
    integer :: i, j, idx

    do i = 1, n
       do j = 1, m
          idx = (i - 1) * m + j
          C(idx) = A(idx) - scalar
       end do
    end do
  end subroutine matrix_scalar_sub

end module matrix_scalar_mod
