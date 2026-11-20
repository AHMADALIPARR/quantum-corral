! laplacian.f90
! Quantum Corral Laplacian / kinetic-energy operator stub
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under the Boost Software License 1.0 (BSL-1.0)
! and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
! with Competing Use restriction waived ("No competing clause").
! See LICENSE and LICENSE.FSL in the repository root.

module corral_laplacian
  implicit none
contains
  ! Stub: discrete Laplacian on a polar or Cartesian grid
  ! to be specialized for circular corral geometry.
  subroutine apply_laplacian(psi, out, nx, ny)
    integer, intent(in) :: nx, ny
    complex(kind=8), intent(in) :: psi(nx,ny)
    complex(kind=8), intent(out) :: out(nx,ny)
    out = (0.0d0, 0.0d0)  ! placeholder
  end subroutine
end module
