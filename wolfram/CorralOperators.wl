(* ::Package:: *)
(* Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 *)
BeginPackage["QuantumCorral`"];

CorralMode::usage = "CorralMode[m,n,r,theta,R] is a normalized circular Dirichlet eigenmode for angular index m and radial index n.";
CorralWavefunction::usage = "CorralWavefunction[terms,r,theta,t,R,mass,hbar] combines {{m,n,coefficient},...} with time evolution; mass and hbar default to 1.";
CorralDensity::usage = "CorralDensity[psi] returns Abs[psi]^2 (a real nonnegative expression for numeric psi).";
CorralEnergy::usage = "CorralEnergy[m,n,R,mass,hbar] gives the hard-wall energy eigenvalue.";
CorralWaveNumber::usage = "CorralWaveNumber[m,n,R] gives the radial wave number.";
CorralNormalizeTerms::usage = "CorralNormalizeTerms[terms] normalizes the L2 coefficients of distinct orthonormal modes; duplicate modes are combined.";
CorralLaplacianPolar::usage = "CorralLaplacianPolar[expr,r,theta] computes the two-dimensional polar Laplacian.";
CorralGrid::usage = "CorralGrid[terms,nx,ny,R,t,mass,hbar] produces a row-major grid with real, imaginary, density and mask arrays.";
CorralExportJSON::usage = "CorralExportJSON[path,grid] exports an explicit row-major, double-precision field interchange contract.";
CorralExportWXF::usage = "CorralExportWXF[path,grid] exports the Wolfram expression in native WXF form.";

Begin["`Private`"];

CorralWaveNumber[m_Integer, n_Integer?Positive, R_?Positive] := BesselJZero[Abs[m], n]/R;
CorralEnergy[m_Integer, n_Integer?Positive, R_?Positive, mass_?Positive : 1, hbar_?Positive : 1] :=
  hbar^2 CorralWaveNumber[m,n,R]^2/(2 mass);

(* Normalization: integral_0^R integral_0^(2 Pi) |phi|^2 r dtheta dr = 1. *)
CorralMode[m_Integer, n_Integer?Positive, r_, theta_, R_?Positive] := Module[
  {order = Abs[m], z},
  z = BesselJZero[order,n];
  Piecewise[{{BesselJ[order,z r/R] Exp[I m theta]/
      (R Sqrt[Pi] Abs[BesselJ[order+1,z]]), 0 <= r <= R}}, 0]
];

CorralNormalizeTerms::terms = "Expected a nonempty list of {integer m, positive integer n, numeric coefficient} triples with nonzero norm.";
CorralNormalizeTerms[terms_List] := Module[{merged, norm},
  If[Length[terms] == 0 || !AllTrue[terms,
     MatchQ[#, {_Integer, _Integer?Positive, _?NumericQ}] &],
    Message[CorralNormalizeTerms::terms]; Return[$Failed]];
  merged = KeyValueMap[Append[List @@ #1, #2] &,
    Merge[Rule[Take[#,2],Last[#]] & /@ terms, Total]];
  merged = Select[merged, N[Abs[Last[#]]] > 10^-14 &];
  norm = Sqrt[Total[Abs[Last[#]]^2 & /@ merged]];
  If[!TrueQ[N[norm] > 0], Message[CorralNormalizeTerms::terms]; Return[$Failed]];
  Append[Take[#,2],Last[#]/norm] & /@ merged
];

CorralWavefunction[terms_List, r_, theta_, t_, R_?Positive, mass_?Positive : 1, hbar_?Positive : 1] :=
  Total[Function[term, With[{m=term[[1]],n=term[[2]],c=term[[3]]},
    c CorralMode[m,n,r,theta,R] Exp[-I CorralEnergy[m,n,R,mass,hbar] t/hbar]
  ]] /@ terms];

CorralDensity[psi_] := Abs[psi]^2;
CorralLaplacianPolar[expr_, r_Symbol, theta_Symbol] :=
  D[expr,{r,2}] + D[expr,r]/r + D[expr,{theta,2}]/r^2;

CorralGrid::size = "nx and ny must be integers of at least 2, and R must be positive.";
CorralGrid[terms_List, nx_Integer, ny_Integer, R_?Positive, t_:0, mass_?Positive:1, hbar_?Positive:1] := Module[
  {xcoords,ycoords,samples,psi,inside,normalized,entries},
  If[nx < 2 || ny < 2, Message[CorralGrid::size]; Return[$Failed]];
  normalized = CorralNormalizeTerms[terms];
  If[normalized === $Failed, Return[$Failed]];
  xcoords = N[Subdivide[-R,R,nx-1],MachinePrecision];
  ycoords = N[Subdivide[-R,R,ny-1],MachinePrecision];
  entries = Flatten[Table[
    With[{x=xcoords[[ix]],y=ycoords[[iy]]},
      inside = TrueQ[x^2+y^2 <= R^2];
      psi = If[inside, N[CorralWavefunction[normalized,Sqrt[x^2+y^2],ArcTan[x,y],t,R,mass,hbar]],0.+0.I];
      {N[Re[psi]],N[Im[psi]],N[Abs[psi]^2],If[inside,1,0]}
    ],{iy,ny},{ix,nx}],1];
  <|
    "schema"->"quantum-corral-field-v1", "nx"->nx,"ny"->ny,
    "radius"->N[R],"time"->N[t],"mass"->N[mass],"hbar"->N[hbar],
    "layout"->"row-major", "index"->"linearIndex=y*nx+x (zero-based)",
    "coordinates"->"x,y from -radius to +radius inclusive", "dtype"->"float64",
    "real"->entries[[All,1]],"imag"->entries[[All,2]],
    "density"->entries[[All,3]],"mask"->entries[[All,4]],
    "terms"->( <|"m"->#[[1]],"n"->#[[2]],"coefficientRe"->N[Re[#[[3]]]],
      "coefficientIm"->N[Im[#[[3]]]]|> & /@ normalized)
  |>
];

CorralExportJSON[path_String,grid_Association] := Export[path,grid,"RawJSON"];
CorralExportWXF[path_String,grid_Association] := Export[path,grid,"WXF"];

End[];
EndPackage[];
