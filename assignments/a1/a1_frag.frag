#version 330 core

uniform vec2 iResolution;
uniform float iTime;
uniform int iFrame;
in vec2 fragCoord;
out vec4 fragColor;

const float M_PI = 3.1415926535;
const vec3 BG_COLOR = vec3(184, 243, 255) / 255.;

// To convert from Polar Coordinates to Cartesian coordinates
vec2 polar2cart(float angle, float length)
{
    return vec2(cos(angle) * length, sin(angle) * length);
}

// To check if a point is inside a circle
bool inCircle(vec2 p, vec2 center, float radius)
{
    vec2 to_center = p - center;
    if(dot(to_center, to_center) < radius * radius)
    {
        return true;
    }
    return false;
}

// To check if a point is inside a triangle
bool inTriangle(vec2 p, vec2 p1, vec2 p2, vec2 p3)
{
    if(dot(cross(vec3(p2 - p1, 0), vec3(p - p1, 0)), cross(vec3(p2 - p1, 0), vec3(p3 - p1, 0))) >= 0. &&
        dot(cross(vec3(p3 - p2, 0), vec3(p - p2, 0)), cross(vec3(p3 - p2, 0), vec3(p1 - p2, 0))) >= 0. &&
        dot(cross(vec3(p1 - p3, 0), vec3(p - p3, 0)), cross(vec3(p1 - p3, 0), vec3(p2 - p3, 0))) >= 0.)
    {
        return true;
    }
    return false;
}

vec3 drawCircle(vec2 pos, vec2 orig, float radius, vec3 color)
{
    if(inCircle(pos, orig, radius))
    {
        return color;
    }
    return vec3(0);
}

vec3 drawRectangle(vec2 pos, vec2 leftBottom, vec2 rightTop, vec3 color)
{
    if(pos.x >= leftBottom.x && pos.x <= rightTop.x && pos.y >= leftBottom.y && pos.y <= rightTop.y)
    {
        return color;
    }
    return vec3(0);
}

// Creative part: draw a snowflake
// Return the rgba color of the grid at position (x, y) 
vec3 drawTriangle(vec2 pos, vec2 center, vec3 color)
{
    vec2 p1 = polar2cart(iTime * 2, 160.) + center;
    vec2 p2 = polar2cart(iTime * 2 + 2. * M_PI / 3., 160.) + center;
    vec2 p3 = polar2cart(iTime * 2 + 4. * M_PI / 3., 160.) + center;
    if(inTriangle(pos, p1, p2, p3))
    {
        return color;
    }
    return vec3(0);
}

// The function called in the fragment shader
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // window center
    vec2 center = vec2(iResolution / 2.);

    vec3 fragOutput = drawTriangle(fragCoord, center, vec3(1.0));
    // Draw a circle at the center of the window
    fragOutput = drawCircle(fragCoord, center, 250, vec3(1.0));

    // Draw a rectangle at the center of the window
    fragOutput = drawRectangle(fragCoord, center - vec2(500, 50), center + vec2(500, 50), vec3(1.0));


    // Draw a circle with changing radius
    fragOutput = drawCircle(fragCoord, center, 150 + 50. * sin(iTime * 10), vec3(1.0));

    // Draw a triangle with changing position
    vec2 offset = vec2(sin(iTime * 10), cos(iTime * 10)) * 100.;
    fragOutput += drawRectangle(fragCoord, center - vec2(500, 50) + offset, center + vec2(500, 50) + offset, vec3(1.0));

    if(fragOutput == vec3(0))
    {
        fragOutput = BG_COLOR;
    }
    fragColor = vec4(fragOutput, 1.0);
}

void main()
{
    mainImage(fragColor, fragCoord);
}