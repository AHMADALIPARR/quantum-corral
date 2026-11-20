(* Run from repo root: wolframscript -file verification/numerical/run_wolfram_tests.wls *)
root = DirectoryName[DirectoryName[DirectoryName[ExpandFileName[$InputFileName]]]];
report = TestReport[FileNameJoin[{root, "verification", "numerical", "CorralTests.wlt"}]];
Print[report];
If[report["TestsFailedCount"] > 0 || report["TestsSucceededCount"] == 0, Exit[1], Exit[0]];
