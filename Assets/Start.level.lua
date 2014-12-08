
return
{
	-- All class types in this level
	ClassTypes = 
	{
		"Camera",
		"Light",
		"Player",
		"Plane",
		"Pipe"
	},

	LightingData = 
	{
		AmbientLight = {0.2, 0.2, 0.2},
		DiffuseLight = {1.0, 1.0, 1.0},
		LightDirection = {0.0, -1.0, 0.0}
	},
	
	CameraData = 
	{
		class = "Camera", --For collision
		velocity  = { 0.0, 0.0, 0.0 },
		acceleration  = { 0.0, 0.0, 0.0 },
		size = {2.0, 2.0, 2.0},
		rotation = 0.0,
		EyeLocation = {0.0, 11.0, 1.0},
		LookAtLocation = {0.0, -1.0, 1.0},
		Up = {0.0, 0.0, 1.0},
		FieldOfView = 60.0, --degrees
		NearPlane = 0.1,
		FarPlane = 100.0,
		collisionSettings = 
		{
			canCollideWith = 
			{
				--"Cube",
			}
		}
	},
	
	--Default values for actors that can be spawned
	SpawanableActors = 
	{
		
	},

	-- Actor data in level
	Actors = 
	{
		{
			name = "flappy",
			class = "Player",
			position  = { 0.0, 0.0, -1.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/flappybird.dat",
				materialPath = "data/flappyBirdMaterial.mat",
			},
			collisionSettings = 
			{
				canCollideWith = 
				{
					--"Torus",
				}
			}
		} ,
		{
			name = "Background",
			class = "Plane",
			position  = { 0.0, -1.0, 0.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/background.dat",
				materialPath = "data/BackgroundScrollBumpMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
		{
			name = "Pipe",
			class = "Pipe",
			position  = { 3.0, 0.0, 2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/pipe.dat",
				materialPath = "data/pipeMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
		{
			name = "InvertedPipe",
			class = "Pipe",
			position  = { 3.0, 0.0, -2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/invertedPipe.dat",
				materialPath = "data/pipeMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
	}
}