#version 330 core
in vec2 vUV;
out vec4 FragColor;
void main(){
    // Colorir o triângulo para depurar: degrade por UV
    FragColor = vec4(vUV.x, vUV.y, 0.2, 1.0);
}


