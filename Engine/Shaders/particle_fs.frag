precision mediump float;
varying vec2 v_texcoord;
varying vec4 v_color;

float circle(in vec2 st, in float radius) {
    vec2 dist = st - vec2(0.5);
    return 1.0 - smoothstep(
        radius - (radius * 0.01),
        radius +(radius * 0.01),
        dot(dist, dist) * 4.0);
}

void main() {
    if (circle(v_texcoord, 1.0) < 0.5) {
        discard;
    }
    gl_FragColor = v_color;
}