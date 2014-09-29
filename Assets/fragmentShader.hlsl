/*
	This is an example of a fragment shader
*/

// Constants
//==========

// Per-Material
//-------------

uniform float3 g_colorModifier =
	// This constant modifies the color by multiplying
	// and so the default is set to white, the multiplicative identity.
	// (It will have no visible effect unless a specific material
	// overrides the default value)
	{ 1.0, 1.0, 1.0 };

// Entry Point
//============

void main( in const float3 i_color : COLOR0, out float4 o_color : COLOR0 )
{
	// The output color is the interpolated per-vertex color
	// modified by a per-material constant
	o_color = float4( i_color * g_colorModifier,
		// For now the A value should _always_ be 1.0
		1.0 );
}
