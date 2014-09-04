/*
	This is an example of a fragment shader
*/

// Entry Point
//============

void main( out float4 o_color : COLOR0 )
{
	// Until we have more sophisticated shaders the output color can be set to an arbitrary RGBA value:
	o_color = float4( 1.0, 1.0, 1.0, 1.0 );
	// "RGB" = "Red/Green/Blue" and "A" = "Alpha".
	// The RGB values should each be something between 0.0 and 1.0,
	// and for now the A value should _always_ be 1.0.
}
