!===============================================================================
! corral_sim.f90 — Electron Quantum Corral Numerical Kernel (Fortran 2008)
!
! Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
! Licensed under the Boost Software License 1.0 (BSL-1.0)
! and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
! with Competing Use restriction waived ("No competing clause").
! See LICENSE and LICENSE.FSL in the repository root.
!
! Implements:
!   1) Analytical circular-corral eigenmode superposition (Bessel J_m modes)
!      matching: Psi(r,theta,t) = sum_n c_n * phi_n(r,theta) * exp(-i*E_n*t/hbar)
!   2) 2D Time-Dependent Schrodinger Equation (TDSE) RK4 finite-difference solver
!      with N_atoms discrete Gaussian scatterers (leaky atomic corral potential)
!      and boundary-constraint / interference metrics.
!
! Streams CSV wavefield data to stdout for piping into MATLAB (corral_viz.m):
!   gfortran -O3 -std=f2008 corral_sim.f90 -o corral_sim
!   ./corral_sim --mode eigen --a1 0.99 --a2 0.81 --phase 190 > corral_out.csv
!   ./corral_sim --mode tdse  --leak 0.15 --steps 300 | matlab -batch "corral_viz"
!===============================================================================
program corral_sim
    use, intrinsic :: iso_fortran_env, only: dp => real64, output_unit
    implicit none

    real(dp), parameter :: PI = 3.14159265358979323846_dp
    ! First zeros of Bessel functions J_0 and J_1 for hard-wall circular corral
    real(dp), parameter :: Z01 = 2.40482555769577276862_dp
    real(dp), parameter :: Z11 = 3.83170597020751231561_dp

    ! Simulation parameters (defaults match interactive prompt values)
    character(len=16) :: mode_str
    integer :: grid_n, n_steps, n_atoms
    real(dp) :: a1, a2, phase_deg, phase_rad
    real(dp) :: domain_L, corral_R, leak_alpha, dt, t_eval

    ! Parse CLI arguments
    call parse_args(mode_str, grid_n, n_steps, n_atoms, a1, a2, phase_deg, leak_alpha)
    phase_rad = phase_deg * PI / 180.0_dp
    domain_L  = 3.0_dp
    corral_R  = 1.0_dp
    dt        = 0.0004_dp
    t_eval    = 0.0_dp

    if (trim(mode_str) == "tdse") then
        call run_tdse(grid_n, n_steps, n_atoms, domain_L, corral_R, leak_alpha, dt, a1, a2, phase_rad)
    else
        call run_eigen_superposition(grid_n, corral_R, domain_L, n_atoms, a1, a2, phase_rad, t_eval)
    end if

contains

    !---------------------------------------------------------------------------
    ! Mode 1: Analytical Circular-Corral Eigenmode Superposition
    !---------------------------------------------------------------------------
    subroutine run_eigen_superposition(N, R, L, natoms, amp1, amp2, dphi, t)
        integer, intent(in) :: N, natoms
        real(dp), intent(in) :: R, L, amp1, amp2, dphi, t

        integer :: i, j, k
        real(dp) :: dx, x, y, r_val, theta, norm_c
        real(dp) :: phi1, phi2, E1, E2, V_val, ax, ay, dist2
        complex(dp) :: psi, c1, c2
        real(dp) :: rho, p_in, p_total, p_left, p_right

        dx = (2.0_dp * L) / real(N - 1, dp)
        ! Dimensionless energies E_n ~ z_{m,n}^2
        E1 = Z01 * Z01
        E2 = Z11 * Z11

        norm_c = sqrt(max(amp1*amp1 + amp2*amp2, 1.0e-12_dp))
        c1 = cmplx(amp1 / norm_c, 0.0_dp, kind=dp)
        c2 = cmplx(amp2 / norm_c, 0.0_dp, kind=dp) * exp(cmplx(0.0_dp, dphi, kind=dp))

        p_in = 0.0_dp
        p_total = 0.0_dp
        p_left = 0.0_dp
        p_right = 0.0_dp

        write(output_unit, '(A)') "x,y,rho,real_psi,imag_psi,V"

        do j = 1, N
            y = -L + real(j - 1, dp) * dx
            do i = 1, N
                x = -L + real(i - 1, dp) * dx
                r_val = sqrt(x*x + y*y)
                theta = atan2(y, x)

                ! Atomic ring potential (for STM-style surface visualization)
                V_val = 0.0_dp
                do k = 1, natoms
                    ax = R * cos(2.0_dp * PI * real(k - 1, dp) / real(natoms, dp))
                    ay = R * sin(2.0_dp * PI * real(k - 1, dp) / real(natoms, dp))
                    dist2 = (x - ax)**2 + (y - ay)**2
                    V_val = V_val + 12.0_dp * exp(-dist2 / (0.045_dp**2))
                end do

                if (r_val <= R) then
                    phi1 = bessel_j0(Z01 * r_val / R)
                    phi2 = bessel_j1(Z11 * r_val / R) * cos(theta)
                    psi  = c1 * phi1 * exp(cmplx(0.0_dp, -E1 * t, kind=dp)) + &
                           c2 * phi2 * exp(cmplx(0.0_dp, -E2 * t, kind=dp))
                else
                    ! Evanescent tail outside corral wall
                    psi = cmplx(0.0_dp, 0.0_dp, kind=dp)
                end if

                rho = real(psi * conjg(psi), dp)
                p_total = p_total + rho * dx * dx
                if (r_val <= R) p_in = p_in + rho * dx * dx
                if (x < 0.0_dp) then
                    p_left = p_left + rho * dx * dx
                else
                    p_right = p_right + rho * dx * dx
                end if

                write(output_unit, '(F10.5,",",F10.5,",",E14.6,",",E14.6,",",E14.6,",",E14.6)') &
                    x, y, rho, real(psi, dp), aimag(psi), V_val
            end do
        end do

        ! Metadata summary line prefixed with # for downstream MATLAB parser
        write(output_unit, '(A,I0,A,F6.3,A,F6.3,A,F7.2,A,F8.5,A,F8.5)') &
            "# META N=", N, " a1=", amp1, " a2=", amp2, " phase_deg=", dphi * 180.0_dp / PI, &
            " P_left=", p_left / max(p_total, 1.0e-12_dp), &
            " P_right=", p_right / max(p_total, 1.0e-12_dp)
    end subroutine run_eigen_superposition

    !---------------------------------------------------------------------------
    ! Mode 2: 2D TDSE Solver (RK4 Finite Difference) with Leaky Atomic Corral
    !---------------------------------------------------------------------------
    subroutine run_tdse(N, steps, natoms, L, R, leak, dt_step, amp1, amp2, dphi)
        integer, intent(in) :: N, steps, natoms
        real(dp), intent(in) :: L, R, leak, dt_step, amp1, amp2, dphi

        complex(dp), allocatable :: psi(:,:), k1(:,:), k2(:,:), k3(:,:), k4(:,:), tmp(:,:)
        real(dp), allocatable :: V(:,:), x_grid(:), y_grid(:)
        integer :: i, j, k, step
        real(dp) :: dx, r_val, theta, ax, ay, dist2, V0, sigma_a, norm_c
        real(dp) :: phi1, phi2, rho, p_in, p_tot, p_left, p_right
        complex(dp) :: c1, c2

        allocate(psi(N,N), k1(N,N), k2(N,N), k3(N,N), k4(N,N), tmp(N,N))
        allocate(V(N,N), x_grid(N), y_grid(N))

        dx = (2.0_dp * L) / real(N - 1, dp)
        do i = 1, N
            x_grid(i) = -L + real(i - 1, dp) * dx
            y_grid(i) = -L + real(i - 1, dp) * dx
        end do

        ! Construct leaky atomic corral potential:
        ! Higher leak -> weaker barrier height V0 and wider inter-atomic gaps
        V0 = 180.0_dp * max(0.05_dp, 1.0_dp - leak)
        sigma_a = 0.055_dp * max(0.35_dp, 1.0_dp - 0.5_dp * leak)

        V = 0.0_dp
        do j = 1, N
            do i = 1, N
                r_val = sqrt(x_grid(i)**2 + y_grid(j)**2)
                do k = 1, natoms
                    ax = R * cos(2.0_dp * PI * real(k - 1, dp) / real(natoms, dp))
                    ay = R * sin(2.0_dp * PI * real(k - 1, dp) / real(natoms, dp))
                    dist2 = (x_grid(i) - ax)**2 + (y_grid(j) - ay)**2
                    V(i,j) = V(i,j) + V0 * exp(-dist2 / (sigma_a**2))
                end do
                ! Soft absorbing boundary near domain edge to prevent artificial reflections
                if (r_val > 0.85_dp * L) then
                    V(i,j) = V(i,j) + 40.0_dp * ((r_val - 0.85_dp * L) / (0.15_dp * L))**2
                end if
            end do
        end do

        ! Initialize superposition inside the corral
        norm_c = sqrt(max(amp1*amp1 + amp2*amp2, 1.0e-12_dp))
        c1 = cmplx(amp1 / norm_c, 0.0_dp, kind=dp)
        c2 = cmplx(amp2 / norm_c, 0.0_dp, kind=dp) * exp(cmplx(0.0_dp, dphi, kind=dp))

        psi = cmplx(0.0_dp, 0.0_dp, kind=dp)
        do j = 1, N
            do i = 1, N
                r_val = sqrt(x_grid(i)**2 + y_grid(j)**2)
                theta = atan2(y_grid(j), x_grid(i))
                if (r_val <= R) then
                    phi1 = bessel_j0(Z01 * r_val / R)
                    phi2 = bessel_j1(Z11 * r_val / R) * cos(theta)
                    psi(i,j) = c1 * phi1 + c2 * phi2
                end if
            end do
        end do

        ! Normalize initial wavefunction
        p_tot = sum(real(psi * conjg(psi), dp)) * dx * dx
        if (p_tot > 0.0_dp) psi = psi / sqrt(p_tot)

        ! RK4 Time Integration: i d(psi)/dt = (-0.5 * Lap(psi) + V*psi)
        do step = 1, steps
            call compute_rhs(N, dx, V, psi, k1)
            tmp = psi + 0.5_dp * dt_step * k1
            call compute_rhs(N, dx, V, tmp, k2)
            tmp = psi + 0.5_dp * dt_step * k2
            call compute_rhs(N, dx, V, tmp, k3)
            tmp = psi + dt_step * k3
            call compute_rhs(N, dx, V, tmp, k4)
            psi = psi + (dt_step / 6.0_dp) * (k1 + 2.0_dp*k2 + 2.0_dp*k3 + k4)
        end do

        p_in = 0.0_dp
        p_tot = 0.0_dp
        p_left = 0.0_dp
        p_right = 0.0_dp

        write(output_unit, '(A)') "x,y,rho,real_psi,imag_psi,V"
        do j = 1, N
            do i = 1, N
                r_val = sqrt(x_grid(i)**2 + y_grid(j)**2)
                rho = real(psi(i,j) * conjg(psi(i,j)), dp)
                p_tot = p_tot + rho * dx * dx
                if (r_val <= R) p_in = p_in + rho * dx * dx
                if (x_grid(i) < 0.0_dp) then
                    p_left = p_left + rho * dx * dx
                else
                    p_right = p_right + rho * dx * dx
                end if
                write(output_unit, '(F10.5,",",F10.5,",",E14.6,",",E14.6,",",E14.6,",",E14.6)') &
                    x_grid(i), y_grid(j), rho, real(psi(i,j), dp), aimag(psi(i,j)), V(i,j)
            end do
        end do

        write(output_unit, '(A,I0,A,F5.2,A,F8.5,A,F8.5,A,F8.5)') &
            "# META N=", N, " leak=", leak, " P_in=", p_in / max(p_tot, 1.0e-12_dp), &
            " P_left=", p_left / max(p_tot, 1.0e-12_dp), &
            " P_right=", p_right / max(p_tot, 1.0e-12_dp)

        deallocate(psi, k1, k2, k3, k4, tmp, V, x_grid, y_grid)
    end subroutine run_tdse

    !---------------------------------------------------------------------------
    ! 5-point Laplacian RHS: d(psi)/dt = -i * (-0.5 * Lap(psi) + V * psi)
    !---------------------------------------------------------------------------
    subroutine compute_rhs(N, dx, V, psi_in, rhs_out)
        integer, intent(in) :: N
        real(dp), intent(in) :: dx, V(N,N)
        complex(dp), intent(in) :: psi_in(N,N)
        complex(dp), intent(out) :: rhs_out(N,N)

        integer :: i, j
        real(dp) :: inv_dx2
        complex(dp) :: lap, H_psi
        complex(dp), parameter :: NEG_I = cmplx(0.0_dp, -1.0_dp, kind=dp)

        inv_dx2 = 1.0_dp / (dx * dx)
        rhs_out = cmplx(0.0_dp, 0.0_dp, kind=dp)

        do j = 2, N - 1
            do i = 2, N - 1
                lap = (psi_in(i+1,j) + psi_in(i-1,j) + psi_in(i,j+1) + psi_in(i,j-1) - 4.0_dp * psi_in(i,j)) * inv_dx2
                H_psi = -0.5_dp * lap + V(i,j) * psi_in(i,j)
                rhs_out(i,j) = NEG_I * H_psi
            end do
        end do
    end subroutine compute_rhs

    !---------------------------------------------------------------------------
    ! Command-line argument parser
    !---------------------------------------------------------------------------
    subroutine parse_args(mode_out, N_out, steps_out, natoms_out, amp1_out, amp2_out, phase_out, leak_out)
        character(len=*), intent(out) :: mode_out
        integer, intent(out) :: N_out, steps_out, natoms_out
        real(dp), intent(out) :: amp1_out, amp2_out, phase_out, leak_out

        integer :: idx, argc
        character(len=64) :: arg, val

        mode_out   = "eigen"
        N_out      = 128
        steps_out  = 250
        natoms_out = 48
        amp1_out   = 0.99_dp
        amp2_out   = 0.81_dp
        phase_out  = 190.0_dp
        leak_out   = 0.10_dp

        argc = command_argument_count()
        idx = 1
        do while (idx <= argc)
            call get_command_argument(idx, arg)
            if (idx < argc) then
                call get_command_argument(idx + 1, val)
                select case (trim(arg))
                case ("--mode")
                    mode_out = trim(val)
                    idx = idx + 2
                case ("--n")
                    read(val, *) N_out
                    idx = idx + 2
                case ("--steps")
                    read(val, *) steps_out
                    idx = idx + 2
                case ("--atoms")
                    read(val, *) natoms_out
                    idx = idx + 2
                case ("--a1")
                    read(val, *) amp1_out
                    idx = idx + 2
                case ("--a2")
                    read(val, *) amp2_out
                    idx = idx + 2
                case ("--phase")
                    read(val, *) phase_out
                    idx = idx + 2
                case ("--leak")
                    read(val, *) leak_out
                    idx = idx + 2
                case default
                    idx = idx + 1
                end select
            else
                idx = idx + 1
            end if
        end do
    end subroutine parse_args

end program corral_sim
