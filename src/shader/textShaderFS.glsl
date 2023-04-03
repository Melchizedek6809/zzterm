uniform sampler2D curTex;

in vec2 multiTexCoord;
in vec4 frontColor;

out vec4 fragColor;

void main() {
	//fragColor = frontColor * texture(curTex, multiTexCoord);
	fragColor = vec4(0.0,1.0,1.0,1.0);
}
