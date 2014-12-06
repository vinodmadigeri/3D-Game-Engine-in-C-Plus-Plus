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

// Samplers
//=========


// Entry Point
//============

void main( in const float3 i_color : COLOR0,
	out float4 o_color : COLOR0 )
{
	o_color = float4(i_color, 1.0);
}
