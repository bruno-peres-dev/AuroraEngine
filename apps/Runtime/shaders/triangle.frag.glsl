#version 330 core
layout(std140) uniform Globals { vec4 color; };
out vec4 FragColor;
void main(){
    FragColor = color;
}


