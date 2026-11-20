! evolution.f90
! Quantum Corral time-evolution / propagator stub
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under the Boost Software License 1.0 (BSL-1.0)
! and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
! with Competing Use restriction waived ("No competing clause").
! See LICENSE and LICENSE.FSL in the repository root.

module corral_evolution
  implicit none
contains
  ! Stub: unitary or approximate time-step evolution of the wavefunction.
  subroutine evolve_step(psi, dt, nx, ny)
    integer, intent(in) :: nx, ny
    complex(kind=8), intent(inout) :: psi(nx,ny)
    real(kind=8), intent(in) :: dt
    ! placeholder — no-op
  end subroutine
end module
