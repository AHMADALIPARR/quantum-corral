Get[FileNameJoin[{DirectoryName[DirectoryName[DirectoryName[$TestFileName]]],"wolfram","CorralOperators.wl"}]];
Get[FileNameJoin[{DirectoryName[DirectoryName[DirectoryName[$TestFileName]]],"wolfram","CorralBoundary.wl"}]];
VerificationTest[Abs[N[QuantumCorral`CorralDirichletResidual[0,1,1]]] < 10^-12,True,TestID->"boundary-m0n1"]
VerificationTest[Abs[N[QuantumCorral`CorralDirichletResidual[2,3,1]]] < 10^-12,True,TestID->"boundary-m2n3"]
VerificationTest[Abs[QuantumCorral`CorralOrthonormalityIntegral[{0,1},{0,1},1] - 1] < 10^-10,True,TestID->"normalized-ground"]
VerificationTest[Abs[QuantumCorral`CorralOrthonormalityIntegral[{1,1},{1,2},1]] < 10^-10,True,TestID->"orthogonal-radial"]
VerificationTest[Abs[QuantumCorral`CorralOrthonormalityIntegral[{1,1},{-1,1},1]] < 10^-10,True,TestID->"orthogonal-angular"]
VerificationTest[Abs[Total[Abs[Last[#]]^2 & /@ QuantumCorral`CorralNormalizeTerms[{{0,1,2},{1,1,I}}]]-1] < 10^-12,True,TestID->"coefficients-normalized"]
VerificationTest[Module[{g=QuantumCorral`CorralGrid[{{0,1,1}},9,7,1.]},Length[g["real"]] == 63 && Length[g["density"]] == 63 && g["mask"][[1]]==0],True,TestID->"row-major-shape"]
VerificationTest[Module[{g=QuantumCorral`CorralGrid[{{0,1,1}},9,9,1.]},Max[Abs[g["density"]-(g["real"]^2+g["imag"]^2)]] < 10^-12],True,TestID->"density-identity"]
VerificationTest[Abs[N[QuantumCorral`CorralEnergy[1,1,2,1,1]/QuantumCorral`CorralEnergy[1,1,1,1,1]]-1/4] < 10^-12,True,TestID->"energy-scales-Rminus2"]
