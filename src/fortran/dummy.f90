module dummy_mod
    use iso_c_binding
    implicit none
contains
    subroutine say_hello() bind(C, name="say_hello")
        print *, "Hello from Fortran!"
    end subroutine
end module
