# Quantum Corral Compute Stack

[![License: BSL-1.0](https://img.shields.io/badge/License-BSL--1.0-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![License: FSL-1.1-MIT](https://img.shields.io/badge/License-FSL--1.1--MIT-green.svg)](https://fsl.software/)

Symbolic-to-array-to-GPU stack for electron quantum corral simulations:

- **Wolfram Language (`wolfram/`)**: Symbolic Schrödinger operators, Bessel eigenmodes (`CorralOperators.wl`), and leaky atomic ring boundaries (`CorralBoundary.wl`).
- **tinyAPL (`tinyapl/`)**: Lexer/parser, typed Array IR (rank, shape, complex types), typechecker, and C11 lowering backend (`tinyaplc`).
- **C11 Native Runtime & Bridges (`native/`)**: `CorralArray` descriptor ABI (`include/corral.h`, `runtime/corral_runtime.c`), Wolfram WSTP adapter (`bridge/wstp_adapter.c`), and MATLAB Engine adapter (`bridge/matlab_adapter.c`).
- **Fortran 2018 (`fortran/`)**: `ISO_C_BINDING` kernels for probability density (`corral_kernels.f90`), 2D Laplacian (`laplacian.f90`), RK4 TDSE evolution (`evolution.f90`), and full simulator (`corral_sim.f90`).
- **MATLAB (`matlab/`)**: Reference `corral_field.m` and visualization/pipe receiver (`corral_viz.m`).
- **Vulkan (`vulkan/`)**: GLSL 450 compute/render shaders (`shaders/`) and host buffer/pipeline driver (`compute/`, `renderer/`).
- **Verification (`verification/`)**: ABI layout tests, array-semantics APL fixture, numerical e2e, symbolic WL checks.

## Build & Verify (Standalone C11 + tinyAPL)

```bash
make test
```

To enable external SDKs, compile with `-DUSE_WSTP`, `-DUSE_MATLAB_ENGINE`, `-DUSE_FORTRAN_KERNELS`, or `-DUSE_VULKAN` and link the corresponding libraries.

## Fortran + MATLAB pipeline

```bash
gfortran -O3 -std=f2008 fortran/corral_sim.f90 -o corral_sim
./corral_sim --mode eigen --a1 0.99 --a2 0.81 --phase 190 > corral_out.csv
# then in MATLAB:
corral_viz('corral_out.csv')
# or interactive:
corral_viz()
```

## License

Dual-licensed under **Boost Software License 1.0** (BSL-1.0) and **Functional Source License 1.1 MIT Future** (FSL-1.1-MIT) with the Competing Use restriction **waived**. See `LICENSE` and `LICENSE.FSL`.

Copyright © 2026 AHMAD ALI PARR / SNAPKITTYWEST
