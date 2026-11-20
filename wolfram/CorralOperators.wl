(* wolfram/CorralOperators.wl - Symbolic Schrodinger Operators & Eigenmodes
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 *)
BeginPackage["CorralOperators`"];

CorralMode::usage = "CorralMode[m, n, r, theta, radius] returns the ideal circular corral eigenmode.";
CorralDensity::usage = "CorralDensity[psi] returns |psi|^2 expanded symbolically.";
CorralSuperposition::usage = "CorralSuperposition[a1, a2, phaseDeg, r, theta, radius] returns two-mode superposition.";
CorralExportIR::usage = "CorralExportIR[a1, a2, phaseDeg, radius] emits typed array IR specification.";

Begin["`Private`"];

CorralMode[m_Integer, n_Integer, r_, theta_, radius_] := Module[{zero},
  zero = BesselJZero[m, n];
  BesselJ[m, zero r / radius] Exp[I m theta]
];

CorralDensity[psi_] := ComplexExpand[Re[psi]^2 + Im[psi]^2];

CorralSuperposition[a1_, a2_, phaseDeg_, r_, theta_, radius_] := Module[{normC, c1, c2},
  normC = Sqrt[a1^2 + a2^2];
  c1 = a1 / normC;
  c2 = (a2 / normC) Exp[I phaseDeg Degree];
  c1 CorralMode[0, 1, r, theta, radius] + c2 CorralMode[1, 1, r, theta, radius]
];

CorralExportIR[a1_, a2_, phaseDeg_, radius_] := <|
  "ir_version" -> "1.0",
  "z01" -> N[BesselJZero[0, 1], 16],
  "z11" -> N[BesselJZero[1, 1], 16],
  "a1" -> N[a1, 16],
  "a2" -> N[a2, 16],
  "phase_deg" -> N[phaseDeg, 16],
  "radius" -> N[radius, 16]
|>;

End[];
EndPackage[];
