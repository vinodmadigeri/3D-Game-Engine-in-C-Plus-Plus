-- Simple Material in Lua

return
{
	Constants =
	{
		g_color_perMaterial = { 1.0, 1.0, 1.0 },
		g_specular_shininess = 6.0,
	},
	
	VertexShader =
	{
		Path = "data/vertexShaderSpecular.shd",
		ModelToWorld = "g_transform_modelToWorld",
		WorldToView = "g_transform_worldToView",
		ViewToScreen = "g_transform_viewToScreen",
	},

	FragmentShader =
	{
		Path = "data/fragmentShaderSpecular.shd",
		AmbientLight = "g_lighting_ambient",
		DiffuseLight = "g_lighting",
		LightDirection = "g_light_direction",
		CameraPosition = "g_camera_position",
	},
	
	Texture =
	{
		Path = "data/stripes.dds",
		Sampler = "g_color_sampler"
	},
}
