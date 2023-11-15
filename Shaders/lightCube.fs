#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform bool darkBonusAchieved;

void main()
{
    vec3 result;
    if (darkBonusAchieved == true) {
        result = vec3(0.1f, 0.1f, 0.1f);
    }
    else {
        result = color;
    }

    FragColor = vec4(result, 1.0); 
}