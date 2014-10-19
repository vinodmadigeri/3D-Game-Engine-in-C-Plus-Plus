-- Simple Material in Lua

return
{
	Constants =
	{
		g_colorModifier = { 1.0, 1.0, 1.0 },
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
