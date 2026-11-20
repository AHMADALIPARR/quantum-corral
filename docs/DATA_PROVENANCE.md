# Quantum Corral front-end data provenance (executed output only — no README claims)

## Source of truth
All values below come from building and running the repo code in
~/workspace/quantum-corral. Nothing was taken from the README.

## Fortran simulator (executed)
- Build: `gfortran -O3 -std=f2008 fortran/corral_sim.f90 -o corral_sim`
  (conda-forge gfortran 16.2.0, ~/workspace/.tools/fortran-env/bin/gfortran)
- Run: `./corral_sim --mode eigen --a1 0.99 --a2 0.81 --phase 190 --n 256`
- Output: 256x256 CSV, columns x,y,rho,real_psi,imag_psi,V, domain L=3, R=1.
- META line: `# META N=256 a1= 0.990 a2= 0.810 phase_deg= 190.00 P_left= 0.80826 P_right= 0.19174`
- Model read from corral_sim.f90 source: phi1 = J0(z01*r/R), phi2 = J1(z11*r/R)*cos(theta),
  psi = c1*phi1 + c2*phi2*exp(i*phase), rho = |psi|^2, zero outside r>R.
  z01 = 2.4048255576957724, z11 = 3.8317059702075123, E1 = z01^2 = 5.783, E2 = z11^2 = 14.682.
  Coefficients normalized: c1 = a1/sqrt(a1^2+a2^2), c2 = (a2/sqrt(a1^2+a2^2))*exp(i*phase).
  Atomic ring potential V: 48 Gaussians, amplitude 12.0, sigma 0.045, at radius R=1.
  Time evolution (analytic eigen): psi(t) = c1*phi1*exp(-i*E1*t) + c2*phi2*exp(-i*E2*t).

## C11 tests (executed)
- `make test` exit 0:
  - [PASS] ABI layout & stride round-trip verification
  - [PASS] Cross-stack density match: max|tinyAPL-MATLAB|=0.000e+00, max|tinyAPL-Vulkan|=9.943e-08
  - [PASS] TDSE probability conservation: P0=0.609359, P1=0.609358, rel_drift=9.396e-07

## corral_data.json (this folder)
- 102x102 grid cropped from the Fortran 256x256 CSV to x,y in [-1.188235, 1.188235], dx=0.0235294.
- Fields: rho (Fortran density), V (Fortran atomic ring potential), both rounded to 6 decimals.
- maxRho = 0.960156 (at r=0.318), maxV = 11.6465. P_in over cropped grid = 0.609359
  (matches the e2e test P0=0.609359).
- Row-major, index = j*nx + i, j = y row, i = x column, x = x0 + i*dx, y = x0 + j*dx.

## JS Bessel model verification (executed with Node)
- J0/J1 by power series (40 terms). Compared against the full 10,404-point Fortran grid:
  max|fortran - JS| = 1.187e-6 (rounding level of the 6-decimal JSON).
  J0(z01) = 1.75e-16, J1(z11) = -5.5e-17 (zeros confirmed).
- So the front-end slider recomputation from the Bessel model reproduces the Fortran
  output; the embedded JSON is the default field, sliders recompute from the model.

## Colormaps / formulas (read from code, not README)
- matlab/corral_viz.m cyan_corral_cmap: r = 0.03+0.27*t^2.2, g = 0.05+0.78*t^1.15, b = 0.10+0.90*t^0.75.
- matlab/corral_viz.m 3D: Z_stm = 0.55*rho_norm + 0.85*V_norm, view(-35, 52), turbo colormap.
- vulkan/shaders/corral_field.frag: bg rgb(0.03,0.05,0.10), cyan rgb(0.25,0.85,1.00),
  gold ring rgb(0.92,0.76,0.45).
- Gold atom dots: 48 atoms at radius R=1, same color as the ring.

## Honesty note (must appear on the page)
Ideal hard-wall continuum model, dimensionless units (R = m* = hbar = 1).
NOT a calibrated Cu(111)/Fe-adatom surface-state corral.
