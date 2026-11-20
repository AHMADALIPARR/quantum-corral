(* ::Package:: *)
(* Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 *)
BeginPackage["QuantumCorral`"];
CorralDirichletResidual::usage = "CorralDirichletResidual[m,n,R] evaluates the circular Dirichlet boundary residual.";
CorralOrthonormalityIntegral::usage = "CorralOrthonormalityIntegral[{m1,n1},{m2,n2},R] numerically integrates overlap between two modes.";
CorralRadialRoot::usage = "CorralRadialRoot[m,n] is the nth positive zero of J_abs(m).";
Begin["`Private`"];

CorralRadialRoot[m_Integer,n_Integer?Positive] := BesselJZero[Abs[m],n];
CorralDirichletResidual[m_Integer,n_Integer?Positive,R_?Positive] :=
  N[BesselJ[Abs[m],CorralRadialRoot[m,n]]];

CorralOrthonormalityIntegral[{m1_Integer,n1_Integer?Positive},
  {m2_Integer,n2_Integer?Positive},R_?Positive] := Module[{a1,a2,z1,z2,radial},
  (* Integrate angular factor analytically to avoid oscillatory complex quadrature. *)
  If[m1 != m2, Return[0.]];
  a1 = Abs[m1]; a2 = Abs[m2];
  z1 = N[CorralRadialRoot[m1,n1],30]; z2 = N[CorralRadialRoot[m2,n2],30];
  radial = NIntegrate[
     BesselJ[a1,z1 r/R] BesselJ[a2,z2 r/R] r,
     {r,0,R},WorkingPrecision->25,AccuracyGoal->15,PrecisionGoal->15];
  N[2 Pi radial/(R^2 Pi Abs[BesselJ[a1+1,z1]] Abs[BesselJ[a2+1,z2]])]
];
End[];
EndPackage[];
