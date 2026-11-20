#version 450
layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform RenderConsts {
    uint nx;
    uint ny;
    float max_rho;
    float radius_norm;
} rc;

layout(std430, binding = 0) readonly buffer DensityField { float rho[]; };

void main() {
    uint x = clamp(uint(v_uv.x * float(rc.nx)), 0u, rc.nx - 1u);
    uint y = clamp(uint(v_uv.y * float(rc.ny)), 0u, rc.ny - 1u);
    float val = clamp(rho[y * rc.nx + x] / max(rc.max_rho, 1e-6), 0.0, 1.0);

    vec3 bg   = vec3(0.03, 0.05, 0.10);
    vec3 cyan = vec3(0.25, 0.85, 1.00);
    vec3 col  = mix(bg, cyan, pow(val, 0.85));

    /* Subtle circular corral boundary ring */
    vec2 centered = (v_uv - 0.5) * 2.0;
    float r = length(centered);
    if (abs(r - rc.radius_norm) < 0.012) {
        col = mix(col, vec3(0.92, 0.76, 0.45), 0.85);
    }
    fragColor = vec4(col, 1.0);
}
