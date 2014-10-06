/*
	This is an example of a vertex shader
*/

// Constants
//==========

// Per-View
//---------

uniform float4x4 g_transform_worldToView;
uniform float4x4 g_transform_viewToScreen;

// Per-Instance
//-------------

uniform float4x4 g_transform_modelToWorld;

// Entry Point
//============

void main( in const float3 i_position_model : POSITION, in const float3 i_color : COLOR0,
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

		// Any matrix transformations that include translation
		// will operate on a float4 position,
		// which _must_ have 1 for the w value
		float4 position_world = mul( float4( i_position_model, 1.0 ), g_transform_modelToWorld );
		float4 position_view = mul( position_world, g_transform_worldToView );
		o_position_screen = mul( position_view, g_transform_viewToScreen );
	}

	// Calculate color
	{
		// Set the output color directly from the input color:
		o_color = i_color;
	}
}
