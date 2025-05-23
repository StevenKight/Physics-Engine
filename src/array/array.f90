module utils_operations
    use, intrinsic :: iso_c_binding, only: c_int, c_float
    implicit none
    contains

    subroutine multipy(a, b, result) bind(c, name="multiply_fortran")
        real(c_float), value :: a, b
        real(c_float) :: result

        result = a * b
    end subroutine multipy

end module utils_operations