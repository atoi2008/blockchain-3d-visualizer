// Example GLSL fragment shader for glowing effect
uniform float time;
in vec3 blockColor;
out vec4 FragColor;

void main() {
    float intensity = abs(sin(time));  // Pulsating effect
    vec3 glow = blockColor * intensity;
    FragColor = vec4(glow, 1.0);
}
