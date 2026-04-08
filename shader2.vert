#version 410
in vec3 vertex_position;
uniform float time;


void main() {
  vec3 pos = vertex_position;
  pos.y += sin( time );
  gl_Position = vec4(pos, 1.0 );
}