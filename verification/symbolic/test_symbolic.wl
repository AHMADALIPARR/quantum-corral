(* verification/symbolic/test_symbolic.wl *)
Get["../../wolfram/CorralOperators.wl"];
psi = CorralSuperposition[0.99, 0.81, 190, r, theta, 1.0];
rho = CorralDensity[psi];
ir  = CorralExportIR[0.99, 0.81, 190, 1.0];
Print["Symbolic IR: ", ir];
