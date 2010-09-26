varying mat3 mTBN;

uniform sampler2D diffuseMap; // regular texture: texture unit 0
uniform sampler2D normalMap; // regular texture: texture unit 1


void main()
{
	vec3 N = texture2D( normalMap, gl_TexCoord[0].xy );
	N = N * 2.0 - 1.0;
	N = N * mTBN;
	N = ( N + 1.0 ) / 2.0;

	//gl_FragData[0] = texture2D( diffuseMap, gl_TexCoord[0].xy );
	//gl_FragData[1] = vec4(N, 0.0);
	gl_FragColor = vec4(N, 0.0);
}