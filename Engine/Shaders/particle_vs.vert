attribute float id;
attribute vec4 position;
attribute vec2 texcoord;

uniform float time;

varying vec2 v_texcoord;
varying vec4 v_color;

void main() {
    float o = id + time;
    gl_Position = position + vec4(
        vec2(
             fract(o * 0.1373),
             fract(o * 0.5127)) * 2.0 - 1.0,
        0, 0);

    v_texcoord = texcoord;
    v_color = vec4(fract(vec3(id) * vec3(0.127, 0.373, 0.513)), 1);
}