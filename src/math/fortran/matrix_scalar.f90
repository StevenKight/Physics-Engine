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
  implicit none
contains

  !> Multiply matrix by scalar: C = A * scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(8)), row dimension n, column dimension m
  !!   scalar   - input scalar (real(8))
  !!   C(n,m)   - output matrix (real(8)), must be allocated by the caller
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_mul(A, scalar, C, n, m) bind(C, name="matrix_scalar_mul_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m)
    real(8), intent(in) :: scalar
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) * scalar
      end do
    end do
  end subroutine matrix_scalar_mul

  !> Divide matrix by scalar: C = A / scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(8))
  !!   scalar   - input scalar (real(8))
  !!   C(n,m)   - output matrix (real(8))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_div(A, scalar, C, n, m) bind(C, name="matrix_scalar_div_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m)
    real(8), intent(in) :: scalar
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) / scalar
      end do
    end do
  end subroutine matrix_scalar_div

  !> Add scalar to matrix: C = A + scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(8))
  !!   scalar   - input scalar (real(8))
  !!   C(n,m)   - output matrix (real(8))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_add(A, scalar, C, n, m) bind(C, name="matrix_scalar_add_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m)
    real(8), intent(in) :: scalar
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) + scalar
      end do
    end do
  end subroutine matrix_scalar_add

  !> Subtract scalar from matrix: C = A - scalar
  !!
  !! Parameters:
  !!   A(n,m)   - input matrix (real(8))
  !!   scalar   - input scalar (real(8))
  !!   C(n,m)   - output matrix (real(8))
  !!   n, m     - integer dimensions (rows, cols)
  subroutine matrix_scalar_sub(A, scalar, C, n, m) bind(C, name="matrix_scalar_sub_f")
    implicit none
    integer, intent(in) :: n, m
    real(8), intent(in) :: A(n, m)
    real(8), intent(in) :: scalar
    real(8), intent(out) :: C(n, m)
    integer :: i, j
    do i = 1, n
      do j = 1, m
        C(i, j) = A(i, j) - scalar
      end do
    end do
  end subroutine matrix_scalar_sub

end module matrix_scalar_mod
