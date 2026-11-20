%% corral_viz.m
% Quantum Corral visualization & Fortran-pipe receiver (MATLAB)
%
% Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
% Licensed under the Boost Software License 1.0 (BSL-1.0)
% and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
% with Competing Use restriction waived ("No competing clause").
% See LICENSE and LICENSE.FSL in the repository root.
%
% CORRAL_VIZ  MATLAB visualization & Fortran-pipe receiver for Quantum Corral
%
% Usage:
%   1) Pipe directly from Fortran executable (compile first: gfortran -O3 corral_sim.f90 -o corral_sim):
%      corral_viz('pipe', 'eigen')   % runs ./corral_sim --mode eigen and parses stdout
%      corral_viz('pipe', 'tdse')    % runs ./corral_sim --mode tdse  and parses stdout
%   2) Read an existing piped CSV file:
%      corral_viz('corral_out.csv')
%   3) Run standalone interactive MATLAB simulation (no Fortran binary needed):
%      corral_viz()

function corral_viz(csv_source, mode)
    if nargin < 1 || isempty(csv_source)
        csv_source = 'standalone';
    end
    if nargin < 2 || isempty(mode)
        mode = 'eigen';
    end

    % Default parameters matching interactive quantum corral controls
    a1 = 0.99;        % Ground-state amplitude (99%)
    a2 = 0.81;        % Second-mode amplitude (81%)
    phase_deg = 190;  % Relative phase (190 deg)
    R = 1.0;
    n_atoms = 48;

    if strcmpi(csv_source, 'pipe')
        cmd = sprintf('./corral_sim --mode %s --a1 %.2f --a2 %.2f --phase %.1f', ...
                      mode, a1, a2, phase_deg);
        [status, raw_out] = system(cmd);
        if status ~= 0
            error('Fortran pipe failed (%s). Compile first with: gfortran -O3 corral_sim.f90 -o corral_sim', raw_out);
        end
        [X, Y, RHO, V, meta] = parse_piped_csv(raw_out);
        render_corral_figures(X, Y, RHO, V, R, n_atoms, meta);

    elseif isfile(csv_source)
        raw_out = fileread(csv_source);
        [X, Y, RHO, V, meta] = parse_piped_csv(raw_out);
        render_corral_figures(X, Y, RHO, V, R, n_atoms, meta);

    else
        % Standalone interactive mode with live sliders
        launch_interactive_corral(a1, a2, phase_deg, R, n_atoms);
    end
end

%% ------------------------------------------------------------------------
function [X, Y, RHO, V, meta] = parse_piped_csv(raw_text)
    lines = splitlines(raw_text);
    meta = '';
    data_lines = {};
    for k = 1:numel(lines)
        ln = strtrim(lines{k});
        if isempty(ln)
            continue;
        elseif startsWith(ln, '#')
            meta = ln;
        elseif startsWith(ln, 'x,')
            continue;
        else
            data_lines{end+1} = ln; %#ok<AGROW>
        end
    end

    M = str2double(split(data_lines, ','));
    N = round(sqrt(size(M, 1)));
    X   = reshape(M(:,1), N, N)';
    Y   = reshape(M(:,2), N, N)';
    RHO = reshape(M(:,3), N, N)';
    V   = reshape(M(:,6), N, N)';
end

%% ------------------------------------------------------------------------
function render_corral_figures(X, Y, RHO, V, R, n_atoms, meta)
    theta_atoms = linspace(0, 2*pi, n_atoms + 1);
    theta_atoms(end) = [];
    ax = R * cos(theta_atoms);
    ay = R * sin(theta_atoms);

    % Figure 1: 2D Probability Density (Interactive-style cyan/dark plot)
    fig1 = figure('Name', 'Quantum Corral 2D Probability Density', 'Color', [0.03 0.05 0.10]);
    imagesc(X(1,:), Y(:,1), RHO);
    axis equal tight; set(gca, 'YDir', 'normal', 'Color', [0.03 0.05 0.10], ...
        'XColor', [0.7 0.8 0.9], 'YColor', [0.7 0.8 0.9]);
    colormap(fig1, cyan_corral_cmap(256));
    hold on;
    plot(R*cos(linspace(0,2*pi,200)), R*sin(linspace(0,2*pi,200)), '-', 'Color', [0.45 0.68 0.88], 'LineWidth', 1.2);
    scatter(ax, ay, 36, [0.92 0.76 0.45], 'filled');
    xlim([-1.15*R, 1.15*R]); ylim([-1.15*R, 1.15*R]);
    title({'|\Psi(r,\theta)|^2 Superposition Density', meta}, 'Color', 'w', 'Interpreter', 'none');
    colorbar('Color', 'w');

    % Figure 2: 3D STM-Style Surface Relief (|Psi|^2 + Atomic Barrier V)
    fig2 = figure('Name', 'STM-Style 3D Quantum Corral Relief', 'Color', 'k');
    rho_norm = RHO / max(RHO(:));
    V_norm   = V / max(V(:) + eps);
    Z_stm    = 0.55 * rho_norm + 0.85 * V_norm;

    surf(X, Y, Z_stm, 'EdgeColor', 'none', 'FaceColor', 'interp');
    colormap(fig2, turbo(256));
    camlight('headlight'); lighting gouraud; material shiny;
    view(-35, 52); axis tight off;
    title('3D STM-Style Electron Wavefield & Atomic Ring', 'Color', 'w');
end

%% ------------------------------------------------------------------------
function launch_interactive_corral(a1_init, a2_init, phase_init, R, n_atoms)
    N = 160;
    L = 1.15 * R;
    [X, Y] = meshgrid(linspace(-L, L, N), linspace(-L, L, N));
    r = hypot(X, Y);
    theta = atan2(Y, X);

    z01 = 2.4048255577;
    z11 = 3.8317059702;
    phi1 = besselj(0, z01 * r / R) .* (r <= R);
    phi2 = besselj(1, z11 * r / R) .* cos(theta) .* (r <= R);

    theta_atoms = linspace(0, 2*pi, n_atoms + 1);
    theta_atoms(end) = [];
    ax = R * cos(theta_atoms);
    ay = R * sin(theta_atoms);

    fig = figure('Name', 'Interactive Quantum Corral Simulation', ...
                 'Color', [0.04 0.06 0.11], 'Position', [100 100 720 680]);
    ax_h = axes('Parent', fig, 'Position', [0.10 0.25 0.80 0.68]);
    img_h = imagesc(ax_h, X(1,:), Y(:,1), zeros(N));
    axis(ax_h, 'equal', 'tight'); set(ax_h, 'YDir', 'normal', 'XColor', 'none', 'YColor', 'none');
    colormap(ax_h, cyan_corral_cmap(256));
    hold(ax_h, 'on');
    plot(ax_h, R*cos(linspace(0,2*pi,200)), R*sin(linspace(0,2*pi,200)), '-', 'Color', [0.45 0.68 0.88], 'LineWidth', 1.2);
    scatter(ax_h, ax, ay, 36, [0.92 0.76 0.45], 'filled');
    ttl_h = title(ax_h, '', 'Color', 'w', 'FontSize', 12);

    % Sliders for Ground-state amplitude, Second-mode amplitude, Relative phase
    s1 = uicontrol(fig, 'Style', 'slider', 'Min', 0, 'Max', 1, 'Value', a1_init, ...
        'Units', 'normalized', 'Position', [0.30 0.14 0.55 0.03]);
    s2 = uicontrol(fig, 'Style', 'slider', 'Min', 0, 'Max', 1, 'Value', a2_init, ...
        'Units', 'normalized', 'Position', [0.30 0.09 0.55 0.03]);
    s3 = uicontrol(fig, 'Style', 'slider', 'Min', 0, 'Max', 360, 'Value', phase_init, ...
        'Units', 'normalized', 'Position', [0.30 0.04 0.55 0.03]);

    uicontrol(fig, 'Style', 'text', 'String', 'Ground-state amp', 'Units', 'normalized', ...
        'Position', [0.06 0.14 0.22 0.03], 'BackgroundColor', [0.04 0.06 0.11], 'ForegroundColor', 'w');
    uicontrol(fig, 'Style', 'text', 'String', 'Second-mode amp', 'Units', 'normalized', ...
        'Position', [0.06 0.09 0.22 0.03], 'BackgroundColor', [0.04 0.06 0.11], 'ForegroundColor', 'w');
    uicontrol(fig, 'Style', 'text', 'String', 'Relative phase (deg)', 'Units', 'normalized', ...
        'Position', [0.06 0.04 0.22 0.03], 'BackgroundColor', [0.04 0.06 0.11], 'ForegroundColor', 'w');

    s1.Callback = @(~,~) update_plot();
    s2.Callback = @(~,~) update_plot();
    s3.Callback = @(~,~) update_plot();
    update_plot();

    function update_plot()
        w1 = s1.Value; w2 = s2.Value; ph = deg2rad(s3.Value);
        norm_c = sqrt(max(w1^2 + w2^2, 1e-12));
        psi = (w1/norm_c)*phi1 + (w2/norm_c)*exp(1i*ph)*phi2;
        rho = abs(psi).^2;
        set(img_h, 'CData', rho);
        set(ttl_h, 'String', sprintf('a_1 = %.0f%%   |   a_2 = %.0f%%   |   \\Delta\\phi = %.0f^\\circ', ...
            100*w1, 100*w2, s3.Value));
    end
end

%% ------------------------------------------------------------------------
function cmap = cyan_corral_cmap(n)
    t = linspace(0, 1, n)';
    r = 0.03 + 0.27 * (t.^2.2);
    g = 0.05 + 0.78 * (t.^1.15);
    b = 0.10 + 0.90 * (t.^0.75);
    cmap = [r, g, b];
end
