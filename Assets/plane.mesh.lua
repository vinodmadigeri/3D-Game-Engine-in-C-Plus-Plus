-- mesh in Lua

return
{
	VertexCount = 4,

	--Each Vertex Data
	Vertices =
	{
		{
			Position = {-5.92308, -1.33709e-015, -6.02171},
			UV = {0, 1},
			Normal = {0, 1, -2.22045e-016},
			Color = {255, 255, 255},
		},
		{
			Position = {5.92308, -1.33709e-015, -6.02171},
			UV = {0.983622, 1},
			Normal = {0, 1, -2.22045e-016},
			Color = {255, 255, 255},
		},
		{
			Position = {-5.92308, 1.33709e-015, 6.02171},
			UV = {0, 0},
			Normal = {0, 1, -2.22045e-016},
			Color = {255, 255, 255},
		},
		{
			Position = {5.92308, 1.33709e-015, 6.02171},
			UV = {0.983622, 0},
			Normal = {0, 1, -2.22045e-016},
			Color = {255, 255, 255},
		},
	},

	IndexCount = 6,

	--Each triangle index data
	Indices =
	{
		0, 2, 1, 
		2, 3, 1, 
	},
}
