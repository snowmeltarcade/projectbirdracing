#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 color;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor * ubo.color;
}