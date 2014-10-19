--[[
	This file lists every asset to build
]]

return
{
	-- Fragment Shader Programs
	{
		builder = "FragmentShaderBuilder.exe",
		extensions =
		{
			source = "hlsl",
			target = "shd",
		},
		-- Fragment shader assets
		assets =
		{
			"fragmentShader",
		},
	},

	-- Vertex Shader Programs
	{
		builder = "VertexShaderBuilder.exe",
		extensions =
		{
			source = "hlsl",
			target = "shd",
		},
		-- Vertex shader assets
		assets =
		{
			"vertexShader",
		},
	},
	
	-- Generic Assets
	-- (That just get copied as-is rather than built)
	{
		builder = "GenericBuilder.exe",
		extensions =
		{
			source = "mat.lua",
			target = "mat.lua",
		},
		assets =
		{
			"simpleMaterial",
		},
	},
}
