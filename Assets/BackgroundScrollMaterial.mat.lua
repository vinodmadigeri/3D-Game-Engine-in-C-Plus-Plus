-- Simple Material in Lua

return
{
	Constants =
	{
		g_color_perMaterial = { 1.0, 1.0, 1.0 },
		g_uv_speed = {0.0001, 0.0},
	},
	
	VertexShader =
	{
		Path = "data/vertexShaderScroll.shd",
		ModelToWorld = "g_transform_modelToWorld",
		WorldToView = "g_transform_worldToView",
		ViewToScreen = "g_transform_viewToScreen",
		TimeElapsed = "g_totalSecondsElapsed",
	},

	FragmentShader =
	{
		Path = "data/fragmentShaderToonShader.shd",
		AmbientLight = "g_lighting_ambient",
		DiffuseLight = "g_lighting",
		LightDirection = "g_light_direction",
	},
	
	Texture =
	{
		Path = "data/background.dds",
		Sampler = "g_color_sampler"
	},
}
