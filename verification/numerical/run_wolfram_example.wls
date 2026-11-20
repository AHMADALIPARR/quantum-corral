(* Run: wolframscript -file verification/numerical/run_wolfram_example.wls *)
root = DirectoryName[DirectoryName[DirectoryName[ExpandFileName[$InputFileName]]]];
Get[FileNameJoin[{root, "wolfram", "CorralOperators.wl"}]];
Get[FileNameJoin[{root, "wolfram", "CorralBoundary.wl"}]];
output = FileNameJoin[{root, "output"}];
If[!DirectoryQ[output], CreateDirectory[output]];
terms = {{0, 1, Sqrt[3]/2}, {1, 1, I/2}};
grid = QuantumCorral`CorralGrid[terms, 129, 129, 1., 0.1];
If[grid === $Failed, Print["Grid construction failed"]; Exit[1]];
QuantumCorral`CorralExportJSON[FileNameJoin[{output, "corral_129.json"}], grid];
QuantumCorral`CorralExportWXF[FileNameJoin[{output, "corral_129.wxf"}], grid];
Print["Wrote ", grid["nx"], "x", grid["ny"], " corral fields to ", output];
