/*
	This is an example of a vertex shader
*/

// Entry Point
//============

void main( in const float2 i_position : POSITION, out float4 o_position : POSITION )
{
	// Calculate position
	{
		// Set the "out" position directly from the "in" position:
		o_position = float4( i_position.x, i_position.y, 0.0, 1.0 );
		// Or, equivalently:
		o_position = float4( i_position.xy, 0.0, 1.0 );
		o_position = float4( i_position, 0.0, 1.0 );
	}
}
