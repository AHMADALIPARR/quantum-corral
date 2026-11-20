! fortran/evolution.f90 - Fortran 2018 RK4 TDSE Time-Evolution Step
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
! See LICENSE and LICENSE.FSL.

module corral_evolution
  use, intrinsic :: iso_c_binding
  use corral_laplacian
  implicit none
contains
  subroutine corral_rk4_step(re, im, pot_v, nx, ny, dx, dy, dt) &
      bind(C, name="corral_rk4_step")
    integer(c_int), value :: nx, ny
    real(c_double), value :: dx, dy, dt
    real(c_double), intent(inout) :: re(nx, ny), im(nx, ny)
    real(c_double), intent(in) :: pot_v(nx, ny)

    real(c_double), allocatable :: k1r(:,:), k1i(:,:), k2r(:,:), k2i(:,:)
    real(c_double), allocatable :: k3r(:,:), k3i(:,:), k4r(:,:), k4i(:,:)
    real(c_double), allocatable :: tr(:,:), ti(:,:)

    allocate(k1r(nx,ny), k1i(nx,ny), k2r(nx,ny), k2i(nx,ny))
    allocate(k3r(nx,ny), k3i(nx,ny), k4r(nx,ny), k4i(nx,ny))
    allocate(tr(nx,ny), ti(nx,ny))

    call rhs_eval(re, im, pot_v, k1r, k1i, nx, ny, dx, dy)
    tr = re + 0.5_c_double * dt * k1r
    ti = im + 0.5_c_double * dt * k1i
    call rhs_eval(tr, ti, pot_v, k2r, k2i, nx, ny, dx, dy)

    tr = re + 0.5_c_double * dt * k2r
    ti = im + 0.5_c_double * dt * k2i
    call rhs_eval(tr, ti, pot_v, k3r, k3i, nx, ny, dx, dy)

    tr = re + dt * k3r
    ti = im + dt * k3i
    call rhs_eval(tr, ti, pot_v, k4r, k4i, nx, ny, dx, dy)

    re = re + (dt / 6.0_c_double) * (k1r + 2.0_c_double*k2r + 2.0_c_double*k3r + k4r)
    im = im + (dt / 6.0_c_double) * (k1i + 2.0_c_double*k2i + 2.0_c_double*k3i + k4i)

    deallocate(k1r, k1i, k2r, k2i, k3r, k3i, k4r, k4i, tr, ti)
  end subroutine corral_rk4_step

  subroutine rhs_eval(re, im, pot_v, dre, dim, nx, ny, dx, dy)
    integer(c_int), intent(in) :: nx, ny
    real(c_double), intent(in) :: dx, dy
    real(c_double), intent(in) :: re(nx,ny), im(nx,ny), pot_v(nx,ny)
    real(c_double), intent(out) :: dre(nx,ny), dim(nx,ny)
    real(c_double) :: lre(nx,ny), lim(nx,ny)
    integer :: x, y
    real(c_double) :: h_re, h_im

    call corral_laplacian_2d(re, im, lre, lim, nx, ny, dx, dy)
    do y = 1, ny
      do x = 1, nx
        h_re = -0.5_c_double * lre(x,y) + pot_v(x,y) * re(x,y)
        h_im = -0.5_c_double * lim(x,y) + pot_v(x,y) * im(x,y)
        dre(x,y) =  h_im
        dim(x,y) = -h_re
      end do
    end do
  end subroutine rhs_eval
end module corral_evolution
