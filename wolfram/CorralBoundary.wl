(* wolfram/CorralBoundary.wl - Atomic Ring & Leaky Boundary Conditions
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 *)
BeginPackage["CorralBoundary`"];

AtomicRingPotential::usage = "AtomicRingPotential[x, y, radius, nAtoms, v0, sigma] builds Gaussian scatterer ring.";
CorralLeakFactor::usage = "CorralLeakFactor[leakAlpha, v0] scales barrier height for leaky walls.";

Begin["`Private`"];

CorralLeakFactor[leakAlpha_, v0_] := v0 * Max[0.05, 1.0 - leakAlpha];

AtomicRingPotential[x_, y_, radius_, nAtoms_Integer, v0_, sigma_] := Module[{k, ax, ay},
  Sum[
    ax = radius Cos[2 Pi (k - 1)/nAtoms];
    ay = radius Sin[2 Pi (k - 1)/nAtoms];
    v0 Exp[-((x - ax)^2 + (y - ay)^2)/(sigma^2)],
    {k, 1, nAtoms}
  ]
];

End[];
EndPackage[];
