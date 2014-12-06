/*
	vertex shader for sprites
*/

// Constants
//==========

// Per-View
//---------
uniform float4x4 g_transform_worldToView;
uniform float4x4 g_transform_viewToScreen;

// Per-Instance
//-------------


// Entry Point
//============
void main( in const float3 i_position_world : POSITION, in const float3 i_color : COLOR0,
			out float4 o_position_screen : POSITION, out float3 o_color : COLOR0 )
{
	// Calculate position
	{
		float4 position_view = mul( i_position_world, g_transform_worldToView );
		o_position_screen = mul( position_view, g_transform_viewToScreen );
	}

	o_color = i_color;
}
