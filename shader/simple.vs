#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 trasform;

void main() {
    gl_Position = trasform * vec4(aPos, 1.0);
}