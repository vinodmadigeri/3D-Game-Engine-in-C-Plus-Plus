/*
	This is an example of a fragment shader
*/

// Constants
//==========

// Per-Material
//-------------

uniform float3 g_color_perMaterial =
	// This constant modifies the color by multiplying
	// and so the default is set to white, the multiplicative identity.
	// (It will have no visible effect unless a specific material
	// overrides the default value)
	{ 1.0, 1.0, 1.0 };


// Textures
//=========

// NOTE: These are actually not necessary to declare in Direct3D 9.
// I debated whether it would be more confusing to include the declarations or to leave them out,
// and finally decided it would be best to show them even though they're unnecessary.
// It is up to you to decide how to represent textures and samplers in your material file format.

texture2D g_color_texture;

// Samplers
//=========

sampler2D g_color_sampler;
// Entry Point
//============

void main( in const float3 i_normal_world : NORMAL, in const float2 i_uv : TEXCOORD0, in const float3 i_color_perVertex : COLOR0,
	out float4 o_color : COLOR0 )
{
	// "Sample" the texture to get the color at the given texture coordinates
	float3 color_sample = tex2D( g_color_sampler, i_uv ).rgb;
	// The output color is the texture color
	// modified by the interpolated per-vertex color
	// and the per-material constant
	o_color = float4( color_sample * g_color_perMaterial * i_color_perVertex,
		// For now the A value should _always_ be 1.0
		1.0 );
}
