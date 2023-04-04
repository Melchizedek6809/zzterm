uniform mat4 matMVP;
uniform sampler2D textBuf;

in vec4 pos;
in ivec2 bufPos;

out vec2 texCoord;
flat out int glyph;

void main(){
	gl_Position = matMVP * pos;
	texCoord = pos.xy / 16.0;
	vec4 bufData = texelFetch(textBuf, bufPos, 0);
	glyph = int(bufData.r * 256.0);
}
