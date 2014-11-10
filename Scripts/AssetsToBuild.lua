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
	
	-- Mesh Assets
	-- (Gets converted into binary data file)
	{
		builder = "MeshBuilder.exe",
		extensions =
		{
			source = "mesh.lua",
			target = "dat",
		},
		assets =
		{
			"plane",
			"cube"
		},
	},
	
	-- Texture Assets
	-- (Converts .png to .dds format)
	{
		builder = "TextureBuilder.exe",
		extensions =
		{
			source = "png",
			target = "dds",
		},
		assets =
		{
			"imageWithText",
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
