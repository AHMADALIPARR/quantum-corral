# Quantum Corral

[![License: BSL-1.0](https://img.shields.io/badge/License-BSL--1.0-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![License: FSL-1.1-MIT](https://img.shields.io/badge/License-FSL--1.1--MIT-green.svg)](https://fsl.software/)
[![C11](https://img.shields.io/badge/C-C11-informational.svg)](#native-c11-runtime--bridges)
[![Fortran](https://img.shields.io/badge/Fortran-2008-informational.svg)](#fortran)
[![MATLAB](https://img.shields.io/badge/MATLAB-compatible-informational.svg)](#matlab)
[![Wolfram](https://img.shields.io/badge/Wolfram-Language-informational.svg)](#wolfram-language)
[![Vulkan](https://img.shields.io/badge/Vulkan-GLSL450-informational.svg)](#vulkan)

Multi-language stack for **2D circular hard-wall electron corral** simulations: Bessel eigenmodes, coherent superpositions, probability density \(\rho = |\psi|^2\), and time-dependent Schrödinger evolution (RK4).

```
  Wolfram (symbolic modes) ──► tinyAPL (array IR) ──► C11 / Fortran kernels
           │                                              │
           └──────────── MATLAB / Vulkan visualization ───┘
```

---

## Live front end (GitHub Pages)

The interactive corral viewer lives at https://ahmadaliparr.github.io/quantum-corral/ and ships in this repo under `docs/`.

It shows the same density field two ways: a 3D height map (density as height, STM-style colormap, 48 golden atom spikes on the rim) and a 2D top-down view (dark disk, gold rim dots, glowing blue interference rings). Sliders set the two mode amplitudes (a1, a2), the relative phase, and the time evolution, and the page recomputes the density from the Bessel model. The default field is the real Fortran simulator output (a1 0.99, a2 0.81, phase 190), not a sketch.

The page is a single self-contained file (`docs/index.html`, no external requests). The grid behind it is in `docs/corral_data.json`, with the full provenance in `docs/DATA_PROVENANCE.md`. It is an ideal hard-wall continuum model in dimensionless units (R = m* = ħ = 1), not a calibrated Cu(111)/Fe-adatom surface-state corral.

---

## Repository layout

```
quantum-corral/
├── LICENSE / LICENSE.FSL          Dual BSL-1.0 + FSL-1.1-MIT (competing use waived)
├── Makefile                       C11 build: tinyaplc, ABI + e2e verification
├── README.md
├── docs/                          GitHub Pages front end
│   ├── index.html                 Self-contained interactive viewer (3D + 2D)
│   ├── corral_data.json           Real Fortran density grid behind the page
│   └── DATA_PROVENANCE.md         Data source, model, and test results
│
├── wolfram/                       Symbolic Dirichlet modes, grid export
│   ├── CorralOperators.wl         Modes, wavefunction, CorralGrid, JSON/WXF export
│   └── CorralBoundary.wl          Dirichlet residual, orthonormality integrals
│
├── tinyapl/                       tinyAPL → C11 compiler pipeline
│   ├── tinyaplc.c                 Driver
│   ├── parser/                    Lexer + recursive-descent parser
│   ├── array_ir/                  Typed array IR nodes
│   ├── typecheck/                 Rank / shape / broadcast checker
│   └── lowering/c11/              IR → C11 backend (calls corral_density ABI)
│
├── native/                        C11 runtime + language bridges
│   ├── include/corral.h           CorralArray ABI, density / Laplacian / RK4 decls
│   ├── runtime/corral_runtime.c   Array alloc/wrap/free, C fallbacks
│   └── bridge/
│       ├── wstp_adapter.c/.h      Wolfram WSTP superposition eval
│       └── matlab_adapter.c/.h    MATLAB Engine density eval
│
├── fortran/                       ISO_C_BINDING kernels + full simulator
│   ├── corral_kernels.f90         corral_density (bind(C))
│   ├── laplacian.f90              2D finite-difference Laplacian
│   ├── evolution.f90              RK4 TDSE step
│   └── corral_sim.f90             CLI: eigen / tdse modes → CSV stream
│
├── matlab/                        Reference field + visualization UI
│   ├── corral_field.m             ρ = re² + im²
│   └── corral_viz.m               CSV/pipe viewer, 2D/3D, interactive sliders
│
├── vulkan/                        GPU density + headless render
│   ├── compute/                   Staging buffers, density dispatch
│   ├── renderer/                  PPM export (STM-style colormap)
│   └── shaders/
│       ├── corral_density.comp    GLSL 450 |ψ|²
│       ├── corral_field.vert
│       └── corral_field.frag
│
└── verification/
    ├── abi/test_abi_layout.c      Row/col-major stride round-trip
    ├── array_semantics/
    │   └── test_tinyapl_lowering.apl
    ├── numerical/
    │   ├── test_corral_e2e.c      Cross-stack density + TDSE conservation
    │   ├── CorralTests.wlt        Wolfram VerificationTest suite
    │   ├── run_wolfram_tests.wls
    │   └── run_wolfram_example.wls
    └── symbolic/test_symbolic.wl
```

---

## Physics model

For angular index \(m\), radial index \(n\), radius \(R\), let \(j_{|m|,n}\) be the \(n\)th positive zero of \(J_{|m|}\). Dirichlet eigenmodes:

```
φ_mn(r,θ) = J_|m|(j_|m|,n · r/R) · exp(i m θ)
            / [ R √π |J_(|m|+1)(j_|m|,n)| ]

E_mn = ħ² j_|m|,n² / (2 m* R²)
ψ = Σ c_mn φ_mn exp(-i E_mn t / ħ)
ρ = |ψ|²
```

Default units are dimensionless (\(R = m^* = ħ = 1\)). This is an **ideal hard-wall continuum model**, not a calibrated Cu(111)/Fe-adatom surface-state corral.

---

## Component status

| Stack | Role | Key entry points |
|-------|------|------------------|
| **Wolfram** | Symbolic modes, grid sampling, JSON/WXF interchange | `CorralGrid`, `CorralExportJSON` |
| **tinyAPL** | Array IR + typecheck + C11 lowering | `./tinyaplc input.apl -o out.c` |
| **Native C11** | `CorralArray` ABI, density / Laplacian / RK4 fallbacks | `corral.h`, `corral_runtime.c` |
| **Fortran** | ISO C kernels + full eigen/TDSE simulator | `corral_sim --mode eigen\|tdse` |
| **MATLAB** | Field kernel + interactive visualization | `corral_viz`, `corral_field` |
| **Vulkan** | FP32 density compute + headless PPM | `vk_corral_compute_*`, `vk_corral_render_ppm` |
| **Verification** | ABI, e2e cross-stack, Wolfram `.wlt` tests | `make test`, `run_wolfram_tests.wls` |

---

## Build & verify (C11 + tinyAPL)

Requires a C11 compiler (`gcc` / `clang`).

```bash
make          # builds tinyaplc, verify_abi, verify_e2e
make test     # runs ABI layout + numerical e2e
make clean
```

Optional feature macros (link the matching SDKs):

| Flag | Effect |
|------|--------|
| `-DUSE_FORTRAN_KERNELS` | Prefer Fortran ISO C kernels over C fallbacks |
| `-DUSE_WSTP` | Real Wolfram WSTP path in bridge |
| `-DUSE_MATLAB_ENGINE` | Real MATLAB Engine path in bridge |
| `-DUSE_VULKAN` | Real Vulkan dispatch (else headless FP32 mirror) |

---

## Fortran + MATLAB pipeline

```bash
# Build simulator
gfortran -O3 -std=f2008 fortran/corral_sim.f90 -o corral_sim

# Eigenmode superposition → CSV
./corral_sim --mode eigen --a1 0.99 --a2 0.81 --phase 190 > corral_out.csv

# TDSE evolution → CSV stream
./corral_sim --mode tdse --steps 200 --dt 0.0002 > corral_tdse.csv
```

In MATLAB:

```matlab
corral_viz('corral_out.csv')   % static 2D/3D from file
corral_viz()                   % interactive amplitude/phase sliders
```

---

## Wolfram Language

Requires Mathematica or Wolfram Engine (`wolframscript`).

```bash
# Numerical verification suite (boundary, orthonormality, grid, energy scaling)
wolframscript -file verification/numerical/run_wolfram_tests.wls

# Example: 129×129 grid → output/corral_129.json + .wxf
wolframscript -file verification/numerical/run_wolfram_example.wls
```

Packages live under `wolfram/` and export the `QuantumCorral`` context (`CorralMode`, `CorralWavefunction`, `CorralGrid`, `CorralExportJSON`, `CorralDirichletResidual`, …).

Grid interchange contract: row-major, zero-based `i = y*nx + x`, coordinates on `[-R, R]` inclusive, `mask=1` inside the disk.

---

## tinyAPL

Minimal array language lowered to C11 that calls the shared density ABI:

```apl
rho <- |psi|
P   <- +/ rho
```

```bash
./tinyaplc verification/array_semantics/test_tinyapl_lowering.apl -o generated_corral.c
```

Pipeline: parse → typecheck (rank/shape/broadcast) → C11 emit (`corral_density`, `corral_total_probability`).

---

## Vulkan

Host code stages FP64 → FP32, runs density (or a CPU mirror when `USE_VULKAN` is unset), and can write an STM-style PPM:

- `vulkan/compute/vk_corral_compute.c` — init / dispatch / destroy
- `vulkan/renderer/vk_corral_renderer.c` — `vk_corral_render_ppm`
- `vulkan/shaders/corral_density.comp` — GLSL 450 compute

---

## Verification matrix

| Test | Path | Checks |
|------|------|--------|
| ABI layout | `verification/abi/test_abi_layout.c` | Row/col-major strides, transpose round-trip |
| tinyAPL fixture | `verification/array_semantics/test_tinyapl_lowering.apl` | Lowering input for `tinyaplc` |
| Cross-stack e2e | `verification/numerical/test_corral_e2e.c` | tinyAPL ↔ MATLAB ↔ Vulkan density; TDSE probability drift |
| Wolfram suite | `verification/numerical/CorralTests.wlt` | Boundary residual, orthonormality, grid shape, density identity, energy \(R^{-2}\) scaling |
| Symbolic WL | `verification/symbolic/test_symbolic.wl` | Lightweight symbolic checks |

---

## License

Dual-licensed under:

- **Boost Software License 1.0** (`LICENSE`) — fully permissive
- **Functional Source License 1.1 — MIT Future License** (`LICENSE.FSL`) with the **Competing Use clause waived**

Copyright © 2026 AHMAD ALI PARR / SNAPKITTYWEST
