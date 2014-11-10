-- Plane mesh in Lua

return
{
	VertexCount = 4,
	
	--Each Vertex Data
	Vertices =
	{
		{
			Position = {-5.0, 0.0, -5.0},
			UV = {0, 1},
			Color = {100, 100, 100}
		},
		{
			Position = {-5.0, 0.0, 5.0},
			UV = {0, 0},
			Color = {150, 150, 150}
		},
		{
			Position = {5.0, 0.0, 5.0},
			UV = {1, 0},
			Color = {200, 200, 200}
		},
		{
			Position = {5.0, 0.0, -5.0},
			UV = {1, 1},
			Color = {50, 50, 50}
		}
	},
	
	IndexCount = 6,
	
	--Each triangle index data
	Indices = 
	{
		0, 1, 3,	--First Triangle Indices
		3, 1, 2		--Second Triangle Indices
	}
}
