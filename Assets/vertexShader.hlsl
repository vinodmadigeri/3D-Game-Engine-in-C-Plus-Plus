/*
	This is an example of a vertex shader
*/

// Constants
//==========

// Per-Instance
//-------------

uniform float2 g_meshPosition_screen =
	// Default values should never be relied upon
	// for per-instance constants,
	// but as a sanity check the position can be set to the origin
	{ 0.0, 0.0 };

// Entry Point
//============

void main( in const float2 i_position_model : POSITION, in const float3 i_color : COLOR0,
	out float4 o_position_screen : POSITION, out float3 o_color : COLOR0 )
{
	// Calculate position
	{
		// The position stored in the vertex is in "model space",
		// meaning that it is relative to the center (or "origin", or "pivot")
		// of the model.
		// The graphics hardware needs the position of the vertex
		// in normalized device coordinates,
		// meaning where the position of the vertex should be drawn
		// on the screen.
		// This position that we need to output, then,
		// is the result of taking the original vertex in "model space"
		// and transforming it into "screen space".
		o_position_screen = float4(
			g_meshPosition_screen + i_position_model,
			// (We still won't worry about the Z or the W coordinate until a later assignment)
			0.0, 1.0 );
	}

	// Calculate color
	{
		// Set the output color directly from the input color:
		o_color = i_color;
	}
}
