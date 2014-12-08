-- Simple Material in Lua

return
{
	Constants =
	{
		g_color_perMaterial = { 1.0, 1.0, 1.0 },
	},
	
	VertexShader =
	{
		Path = "data/vertexShader.shd",
		ModelToWorld = "g_transform_modelToWorld",
		WorldToView = "g_transform_worldToView",
		ViewToScreen = "g_transform_viewToScreen",
	},

	FragmentShader =
	{
		Path = "data/fragmentShader.shd",
		AmbientLight = "g_lighting_ambient",
		DiffuseLight = "g_lighting",
		LightDirection = "g_light_direction",
	},
	
	Texture =
	{
		Path = "data/flappybird.dds",
		Sampler = "g_color_sampler"
	},
}
