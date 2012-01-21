uniform float fTileSize; //Used to scale texture coordinates
uniform float4x4 mMVP;

void main()
{
	// texture coordinates
	gl_TexCoord[0] =  gl_MultiTexCoord0 * fTileSize;
	gl_Position = mMVP * gl_Vertex;
}