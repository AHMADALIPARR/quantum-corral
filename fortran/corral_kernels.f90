! fortran/corral_kernels.f90 - Fortran 2018 ISO_C_BINDING Probability Density Kernel
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
! See LICENSE and LICENSE.FSL.

module corral_kernels
  use, intrinsic :: iso_c_binding
  implicit none
contains
  subroutine corral_density(re, im, rho, nx, ny) &
      bind(C, name="corral_density")
    integer(c_int), value :: nx, ny
    real(c_double), intent(in) :: re(nx, ny)
    real(c_double), intent(in) :: im(nx, ny)
    real(c_double), intent(out) :: rho(nx, ny)
    integer :: x, y

    do y = 1, ny
      do x = 1, nx
        rho(x, y) = re(x, y)**2 + im(x, y)**2
      end do
    end do
  end subroutine corral_density
end module corral_kernels
