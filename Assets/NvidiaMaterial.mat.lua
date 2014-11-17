-- Simple Material in Lua

return
{
	Constants =
	{
		g_color_perMaterial = { 1.0, 1.0, 1.0 },
	},
	
	VertexShader =
	{
		Path = "data/vertexShader.shd"
	},

	FragmentShader =
	{
		Path = "data/fragmentShader.shd"
	},
	
	Texture =
	{
		Path = "data/nvidia.dds",
		Sampler = "g_color_sampler"
	},
}
