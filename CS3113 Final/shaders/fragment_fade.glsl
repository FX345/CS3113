uniform sampler2D diffuse;
varying vec2 texCoordVar;
uniform vec4 color;  // Add a color uniform to control alpha from the application
 
void main() {
    vec4 texColor = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(texColor.rgb, texColor.a * color.a);  // Use application-provided alpha
} 