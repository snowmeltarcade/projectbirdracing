#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec2 fragTexCoord2;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D tex2;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = (texture(tex, fragTexCoord) * 0.5f) + (texture(tex2, fragTexCoord2) * 0.5f);
    outColor = texture(tex, fragTexCoord);
}
