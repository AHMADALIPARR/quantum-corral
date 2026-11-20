(* ::Package:: *)

(* CorralOperators.wl
 * Quantum Corral mode and density operators (Wolfram Language)
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under the Boost Software License 1.0 (BSL-1.0)
 * and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
 * with Competing Use restriction waived ("No competing clause").
 * See LICENSE and LICENSE.FSL in the repository root.
 *
 * Analytical specification of circular quantum-corral eigenmodes
 * via Bessel zeros and probability density extraction.
 *)

ClearAll[mode, density];

mode[m_Integer, n_Integer, r_, theta_, radius_] :=
 Module[{zero},
  zero = BesselJZero[m, n];
  BesselJ[m, zero r/radius] Exp[I m theta]
 ];

density[psi_] := ComplexExpand[
  Re[psi]^2 + Im[psi]^2
];

(* Example superposition (coefficients a, b free) *)
(*
psi = a mode[0, 1, r, theta, R] +
      b mode[1, 1, r, theta, R];

rho = density[psi];
*)
