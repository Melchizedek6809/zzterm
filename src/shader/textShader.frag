uniform sampler2D curTex;

flat in int glyph;
in vec2 texCoord;

out vec4 fragColor;

void main() {
	vec2 glyphCoord = vec2(fract(glyph / 16.0), floor(glyph / 16.0) / 16.0);
	vec2 tex = fract(texCoord) / 16.0;
	fragColor = texture(curTex, glyphCoord + tex);
}
