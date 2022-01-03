#version 450

//layout(set = 0, binding = 0) uniform UniformBufferObject {
//    vec4 color;
//} ubo;
//
//layout(location = 0) in vec2 inPosition;

//layout(location = 0) out vec4 fragColor;

//void main() {
//    gl_Position = vec4(inPosition.xy, 0.0, 1.0);
//    fragColor = ubo.color;
//}

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    //fragColor = ubo.color;
}