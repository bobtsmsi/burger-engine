uniform vec4 vDofParams;
//x = near blur; y = focal plane; z = far blur; w = blurriness cutoff

uniform vec4 vColor;

float ComputeDepthBlur( float fDepth )
{

	float f;

	if( fDepth < vDofParams.y )
	{
		f = ( fDepth - vDofParams.y ) / ( vDofParams.y - vDofParams.x );
		f = clamp( f, -1.0, 0.0);
	}
	else
	{
		f = ( fDepth - vDofParams.y ) / ( vDofParams.z - vDofParams.y );
		f = clamp( f, 0.0, vDofParams.w );
	}
	
	return f * 0.5 + 0.5;

}

void main()
{
	gl_FragData[0] = vec4( vColor );

	//Computing blur information for Depth of Field
	float fDepth = gl_FragCoord.z / gl_FragCoord.w;	
	gl_FragData[1] = vec4( ComputeDepthBlur( fDepth ), 0.0, 0.0, 0.0);
}