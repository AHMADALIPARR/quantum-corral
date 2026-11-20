%% corral_field.m
% Quantum Corral density field computation (MATLAB)
%
% Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
% Licensed under the Boost Software License 1.0 (BSL-1.0)
% and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
% with Competing Use restriction waived ("No competing clause").
% See LICENSE and LICENSE.FSL in the repository root.
%
% Computes the probability density rho = |psi|^2 from real and imaginary
% field components of a quantum corral wavefunction.

function rho = corral_field(real_field, imag_field)
    arguments
        real_field (:,:) double
        imag_field (:,:) double
    end

    assert(isequal(size(real_field), size(imag_field)));

    rho = real_field.^2 + imag_field.^2;
end
