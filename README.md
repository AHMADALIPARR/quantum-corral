# Quantum Corral

[![License: BSL-1.0](https://img.shields.io/badge/License-BSL--1.0-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![License: FSL-1.1-MIT](https://img.shields.io/badge/License-FSL--1.1--MIT-green.svg)](https://fsl.software/)
[![Language](https://img.shields.io/badge/Lang-MATLAB%20%7C%20Fortran%20%7C%20C%20%7C%20WL%20%7C%20APL-informational)](https://github.com/AHMADALIPARR/quantum-corral)
[![Status](https://img.shields.io/badge/Status-Initial%20Kernel-orange)](https://github.com/AHMADALIPARR/quantum-corral)

**Multi-language numerical interface and analytical specification for quantum-corral wavefunctions, density operators, and boundary conditions.**

The repository establishes a shared density kernel (`ρ = |Re ψ|² + |Im ψ|²`) and Bessel-mode analytical forms across MATLAB, Fortran (ISO C binding), native C, Wolfram Language, with planned TinyAPL, Vulkan compute, and formal verification layers.

> **Licensing note:** Dual-licensed under **Boost Software License 1.0** (BSL-1.0) and **Functional Source License 1.1 MIT Future** (FSL-1.1-MIT). The Competing Use clause of FSL is **waived** for this project ("No competing clause"). See [`LICENSE`](LICENSE) and [`LICENSE.FSL`](LICENSE.FSL).

---

## Repository Layout

```text
quantum-corral/
├── wolfram/
│   ├── CorralOperators.wl      # mode[m,n] + density
│   └── CorralBoundary.wl       # hard-wall / radius helpers
├── tinyapl/                   # (planned)
│   ├── parser/
│   ├── array_ir/
│   ├── typecheck/
│   └── lowering/c11/
├── native/
│   ├── include/corral.h       # C ABI
│   ├── runtime/               # (planned)
│   └── bridge/                # (planned)
├── fortran/
│   ├── corral_kernels.f90     # density (C-bindable)
│   ├── laplacian.f90          # kinetic operator stub
│   └── evolution.f90          # propagator stub
├── matlab/
│   └── corral_field.m         # density kernel
├── vulkan/                   # (planned)
│   ├── shaders/
│   ├── compute/
│   └── renderer/
└── verification/             # (planned)
    ├── symbolic/
    ├── array_semantics/
    ├── abi/
    └── numerical/
```

---

## Core Numerical Interface

| Language   | Entry Point              | Signature / Form                                      | Notes                          |
|------------|--------------------------|-------------------------------------------------------|--------------------------------|
| MATLAB     | `corral_field`           | `rho = corral_field(real_field, imag_field)`          | Size-checked, element-wise     |
| Fortran    | `corral_density`         | `corral_density(re, im, rho, nx, ny)` (bind(C))       | Column-major arrays            |
| C          | `corral_density`         | `void corral_density(const double*, …, int nx, int ny)` | Row-major FP64                 |
| Wolfram    | `mode` / `density`       | `mode[m,n,r,θ,R]`, `density[ψ]`                       | BesselJ + ComplexExpand        |

### Analytical Mode (Wolfram)

```wolfram
mode[m_Integer, n_Integer, r_, theta_, radius_] :=
 Module[{zero},
  zero = BesselJZero[m, n];
  BesselJ[m, zero r/radius] Exp[I m theta]
 ]

density[psi_] := ComplexExpand[Re[psi]^2 + Im[psi]^2]
```

Typical superposition:

```wolfram
psi = a mode[0, 1, r, theta, R] + b mode[1, 1, r, theta, R];
rho = density[psi];
```

### Density Kernel (shared semantics)

```
ρ(x,y) = Re(ψ)² + Im(ψ)²
```

Identical semantics across MATLAB, Fortran, and the C ABI.

---

## Architecture Diagram

```text
                    ┌─────────────────────────┐
                    │   Analytical Layer (WL)  │
                    │  mode[m,n]  density[ψ]   │
                    └───────────┬────────────┘
                                 │
              ┌────────────────┴───────────────┐
              │         Shared Density Kernel          │
              │   ρ = Re² + Im²   (MATLAB / F90 / C)   │
              └─────────────┬─────────────┬────────┘
                             │                    │
              ┌─────────────┴─────────┐   ┌─────────┴───────────┐
              │   Native / Bridge    │   │  Vulkan Compute   │
              │   runtime + ABI      │   │  shaders / render │
              └────────────────────┘   └────────────────────┘
                             │
              ┌─────────────┴─────────────┐
              │     Verification Layer      │
              │  symbolic / numerical / ABI │
              └──────────────────────────┘
```

---

## Topics / Keywords

`quantum-corral` · `bessel-modes` · `probability-density` · `wavefunction`  
`matlab` · `fortran` · `c-abi` · `wolfram` · `tinyapl` · `vulkan`  
`formal-verification` · `iso-c-binding` · `circular-billard` · `hard-wall`

---

## Quick Start

### MATLAB

```matlab
re = randn(64); im = randn(64);
rho = corral_field(re, im);
```

### Fortran (with C interop)

```fortran
use corral_kernels
call corral_density(re, im, rho, nx, ny)
```

### C

```c
#include "corral.h"
corral_density(re, im, rho, nx, ny);
```

### Wolfram

```wolfram
<< CorralOperators`
psi = mode[0, 1, r, theta, 1.0];
rho = density[psi];
```

---

## Status

| Component              | State          |
|------------------------|----------------|
| Density kernel (ML/F90/C) | Implemented |
| Bessel mode operators  | Implemented    |
| Boundary helpers       | Stub           |
| Laplacian / evolution  | Stub           |
| TinyAPL pipeline       | Planned        |
| Vulkan compute         | Planned        |
| Verification suites    | Planned        |

---

## License

- **Primary:** [Boost Software License 1.0](https://www.boost.org/users/license.html) (BSL-1.0)
- **Additional:** [Functional Source License 1.1 MIT Future](https://fsl.software/) (FSL-1.1-MIT) with **Competing Use clause waived**

Copyright © 2026 AHMAD ALI PARR / SNAPKITTYWEST
