/*
	fragment shader for sprites
*/

// Constants
//==========

// Per-Material
//-------------


// Per-Frame
//----------


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

void main( in const float2 i_uv : TEXCOORD0,
	out float4 o_color : COLOR0 )
{
	// "Sample" the texture to get the color at the given texture coordinates
	float4 color_sample = tex2D( g_color_sampler, i_uv ).rgba;
	
	o_color = color_sample;
}
