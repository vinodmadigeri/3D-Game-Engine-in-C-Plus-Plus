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
			"fragmentShaderSprite",
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
			"vertexShaderSprite",
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
			"cube",
			"torus",
			"helix",
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
			"missingTexture",
			"microsoftWindows",
			"directX",
			"nvidia",
			"amd",
			"stripes",
			"logo",
		},
	},
	
	-- Material Assets
	-- (Just gets copied as-is rather than built)
	{
		builder = "GenericBuilder.exe",
		extensions =
		{
			source = "mat.lua",
			target = "mat",
		},
		assets =
		{
			"AMDMaterial",
			"MicrosoftMaterial",
			"DirectXMaterial",
			"NvidiaMaterial",
			"StripesMaterial",
		},
	},
	
	-- Level Assets
	-- (Just get copied as-is rather than built)
	{
		builder = "GenericBuilder.exe",
		extensions =
		{
			source = "level.lua",
			target = "level",
		},
		assets =
		{
			"Start",
		},
	},
}
