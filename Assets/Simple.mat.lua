-- Simple Material in Lua

return
{
	Constants =
	{
		g_colorModifier = { 0.0, 0.5, 1.0 },
	},
	
	VertexShader =
	{
		"data/vertexShader.hlsl"
	},

	FragmentShader =
	{
		"data/fragmentShader.hlsl"
	},
}
