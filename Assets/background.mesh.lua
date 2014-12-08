-- mesh in Lua

return
{
	VertexCount = 4,

	--Each Vertex Data
	Vertices =
	{
		{
			Position = {-9.98195, -1.77738e-015, -8.0046},
			UV = {0, 1},
			Normal = {0, 1, -2.22045e-016},
			Tangent = {1, 0, -0},
			BiTangent = {-0, -2.22045e-016, -1},
			Color = {255, 255, 255},
		},
		{
			Position = {9.98195, -1.77738e-015, -8.0046},
			UV = {1, 1},
			Normal = {0, 1, -2.22045e-016},
			Tangent = {1, 0, -0},
			BiTangent = {-0, -2.22045e-016, -1},
			Color = {255, 255, 255},
		},
		{
			Position = {-9.98195, 1.77738e-015, 8.0046},
			UV = {0, 0.198092},
			Normal = {0, 1, -2.22045e-016},
			Tangent = {1, 0, -0},
			BiTangent = {-0, -2.22045e-016, -1},
			Color = {255, 255, 255},
		},
		{
			Position = {9.98195, 1.77738e-015, 8.0046},
			UV = {1, 0.198092},
			Normal = {0, 1, -2.22045e-016},
			Tangent = {1, 0, -0},
			BiTangent = {-0, -2.22045e-016, -1},
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
