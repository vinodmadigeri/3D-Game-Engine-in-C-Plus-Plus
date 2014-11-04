-- Plane mesh in Lua

return
{
	VertexCount = 8,
	
	--Each Vertex Data
	Vertices =
	{
		{
			Position = {-1.0, -1.0, -1.0},
			Color = {255, 0, 0}
		},
		{
			Position = {-1.0, 1.0, -1.0},
			Color = {0, 255, 0}
		},
		{
			Position = {1.0, 1.0, -1.0},
			Color = {0, 0, 255}
		},
		{
			Position = {1.0, -1.0, -1.0},
			Color = {200, 50, 80}
		},
		{
			Position = {-1.0, -1.0, 1.0},
			Color = {255, 0, 0}
		},
		{
			Position = {-1.0, 1.0, 1.0},
			Color = {0, 0, 255}
		},
		{
			Position = {1.0, 1.0, 1.0},
			Color = {90, 150, 60}
		},
		{
			Position = {1.0, -1.0, 1.0},
			Color = {80, 90, 20}
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
