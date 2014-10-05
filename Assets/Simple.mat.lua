-- Simple Material in Lua

return
{
	Constants =
	{
		g_colorModifier = { 1.0, 0.5, 0.7 },
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
