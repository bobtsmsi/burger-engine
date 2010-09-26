uniform sampler2D diffuseMap; // regular texture: texture unit 0
uniform sampler2D lightBuffer; // regular texture: texture unit 6

void main()
{
	vec2 vTexCoord = gl_TexCoord[0].xy;
	vec2 vScreenTexCoord = vec2( gl_FragCoord.x / 1280.0, gl_FragCoord.y / 720.0 );

	vec4 vLighting = texture2D( lightBuffer, vScreenTexCoord );

	vec3 vChromacity = clamp( vLighting.rgb / ( dot( vLighting.rgb, vec3(0.2126, 0.7152, 0.0722 ) ) + 0.00001f ), 0.0, 1.0 );

	gl_FragColor = vec4( (vLighting.rgb + vChromacity * vLighting.a ) * texture2D( diffuseMap, vTexCoord ),1.0);
}