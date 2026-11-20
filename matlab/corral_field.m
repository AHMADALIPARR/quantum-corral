function rho = corral_field(real_field, imag_field)
% CORRAL_FIELD  Reference MATLAB field density evaluation
%
% Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
% Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
% See LICENSE and LICENSE.FSL.

    arguments
        real_field (:,:) double
        imag_field (:,:) double
    end
    assert(isequal(size(real_field), size(imag_field)));
    rho = real_field.^2 + imag_field.^2;
end
