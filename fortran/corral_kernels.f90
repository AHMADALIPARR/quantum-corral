! corral_kernels.f90
! Quantum Corral density kernel (Fortran 90 / ISO C binding)
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under the Boost Software License 1.0 (BSL-1.0)
! and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
! with Competing Use restriction waived ("No competing clause").
! See LICENSE and LICENSE.FSL in the repository root.
!
! Provides C-interoperable density computation for real/imaginary
! field pairs on an nx-by-ny grid.

module corral_kernels
  use, intrinsic :: iso_c_binding
  implicit none
contains

  subroutine corral_density(re, im, rho, nx, ny) &
      bind(C, name="corral_density")

    integer(c_int), value :: nx, ny
    real(c_double), intent(in) :: re(nx,ny)
    real(c_double), intent(in) :: im(nx,ny)
    real(c_double), intent(out) :: rho(nx,ny)

    integer :: x, y

    do y = 1, ny
      do x = 1, nx
        rho(x,y) = re(x,y)**2 + im(x,y)**2
      end do
    end do

  end subroutine

end module
