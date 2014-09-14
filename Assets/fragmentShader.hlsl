/*
	This is an example of a fragment shader
*/

// Entry Point
//============

void main( in const float3 i_color : COLOR0, out float4 o_color : COLOR0 )
{
	// Set the color of this fragment to the interpolated color value
	// (The interpolated color will depend on how close
	// the fragment is to each of the three vertices of the triangle;
	// the closer it is to a vertex, the more its color will be influenced
	// by that vertex)
	o_color = float4( i_color.rgb, 1.0 );
	// "RGB" = "Red/Green/Blue" and "A" = "Alpha".
	// For now the A value should _always_ be 1.0.
}
