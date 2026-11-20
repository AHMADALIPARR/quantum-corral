! fortran/laplacian.f90 - Fortran 2018 5-point 2D Laplacian Kernel
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
! See LICENSE and LICENSE.FSL.

module corral_laplacian
  use, intrinsic :: iso_c_binding
  implicit none
contains
  subroutine corral_laplacian_2d(re_in, im_in, lap_re, lap_im, nx, ny, dx, dy) &
      bind(C, name="corral_laplacian_2d")
    integer(c_int), value :: nx, ny
    real(c_double), value :: dx, dy
    real(c_double), intent(in) :: re_in(nx, ny), im_in(nx, ny)
    real(c_double), intent(out) :: lap_re(nx, ny), lap_im(nx, ny)
    integer :: x, y
    real(c_double) :: idx2, idy2

    idx2 = 1.0_c_double / (dx * dx)
    idy2 = 1.0_c_double / (dy * dy)
    lap_re = 0.0_c_double
    lap_im = 0.0_c_double

    do y = 2, ny - 1
      do x = 2, nx - 1
        lap_re(x, y) = (re_in(x+1, y) + re_in(x-1, y) - 2.0_c_double * re_in(x, y)) * idx2 + &
                       (re_in(x, y+1) + re_in(x, y-1) - 2.0_c_double * re_in(x, y)) * idy2
        lap_im(x, y) = (im_in(x+1, y) + im_in(x-1, y) - 2.0_c_double * im_in(x, y)) * idx2 + &
                       (im_in(x, y+1) + im_in(x, y-1) - 2.0_c_double * im_in(x, y)) * idy2
      end do
    end do
  end subroutine corral_laplacian_2d
end module corral_laplacian
