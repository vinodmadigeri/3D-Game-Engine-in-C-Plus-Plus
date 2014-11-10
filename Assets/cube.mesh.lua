-- Plane mesh in Lua

return
{
	VertexCount = 8,
	
	--Each Vertex Data
	Vertices =
	{
		{
			Position = {-1.0, -1.0, -1.0},
			UV = {0, 1},
			Color = {255, 255, 255}
		},
		{
			Position = {-1.0, 1.0, -1.0},
			UV = {0, 0},
			Color = {255, 255, 255}
		},
		{
			Position = {1.0, 1.0, -1.0},
			UV = {1, 0},
			Color = {255, 255, 255}
		},
		{
			Position = {1.0, -1.0, -1.0},
			UV = {1, 1},
			Color = {255, 255, 255}
		},
		{
			Position = {-1.0, -1.0, 1.0},
			UV = {1, 1},
			Color = {255, 255, 255}
		},
		{
			Position = {-1.0, 1.0, 1.0},
			UV = {1, 0},
			Color = {255, 255, 255}
		},
		{
			Position = {1.0, 1.0, 1.0},
			UV = {1, 0},
			Color = {255, 255, 255}
		},
		{
			Position = {1.0, -1.0, 1.0},
			UV = {1, 1},
			Color = {255, 255, 255}
		},
	},
	
	IndexCount = 36,
	
	--Each triangle index data
	Indices = 
	{
		0, 1, 3,	--First Triangle Indices
		3, 1, 2,	--Second Triangle Indices
		3, 2, 7,
		7, 2, 6,
		0, 4 ,1,
		1, 4, 5,
		4, 7, 5,
		5, 7, 6,
		2, 1, 5,
		2, 5, 6,
		0, 3, 4,
		4, 3, 7
	}
}
