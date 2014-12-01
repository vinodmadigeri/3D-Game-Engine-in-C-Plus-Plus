/*
	vertex shader for sprites
*/

// Constants
//==========

// Per-View
//---------


// Per-Instance
//-------------


// Entry Point
//============
void main( in const float3 i_position_screen : POSITION, in const float2 i_uv : TEXCOORD0,
			out float4 o_position_screen : POSITION, out float2 o_uv : TEXCOORD0 )
{
	// Calculate position
	{
		o_position_screen = float4 (i_position_screen.xy, 0.0, 1.0);
	}

	// Calculate Texture UV
	{
		// Set the output texture coordinates directly from the input coordinates:
		o_uv = i_uv;
	}
}
