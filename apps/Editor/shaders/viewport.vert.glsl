#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;

layout(std140) uniform Globals {
    mat4 uView;
    mat4 uProj;
};

out vec2 vUV;

void main(){
    vUV = aUV;
    gl_Position = uProj * uView * vec4(aPos, 1.0);
}


