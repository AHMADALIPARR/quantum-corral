(* ::Package:: *)

(* CorralBoundary.wl
 * Quantum Corral boundary conditions and radius utilities
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under the Boost Software License 1.0 (BSL-1.0)
 * and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
 * with Competing Use restriction waived ("No competing clause").
 * See LICENSE and LICENSE.FSL in the repository root.
 *
 * Placeholder for Dirichlet / hard-wall boundary handling
 * on a circular corral of radius R.
 *)

ClearAll[CorralRadius, HardWallBoundary];

CorralRadius[R_?Positive] := R;

HardWallBoundary[psi_, r_, R_] :=
  psi * UnitStep[R - r];
