
return
{
	-- All class types in this level
	ClassTypes = 
	{
		"Camera",
		"Light",
		"Cube",
		"Plane",
		"Torus",
		"Helix",
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
		EyeLocation = {0.0, 11.0, 0.0},
		LookAtLocation = {0.0, -1.0, 0.0},
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
			name = "CubeNvidia",
			class = "Cube",
			position  = { 3.0, 0.0, -2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/cube.dat",
				materialPath = "data/NvidiaMaterial.mat",
			},
			collisionSettings = 
			{
				canCollideWith = 
				{
					"Torus",
				}
			}
		} ,
		{
			name = "CubeAMD",
			class = "Cube",
			position  = { -3.0, 0.0, -2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/cube.dat",
				materialPath = "data/AMDMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
		{
			name = "TorusStripes",
			class = "Torus",
			position  = { -3.0, 0.0, 2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/torus.dat",
				materialPath = "data/StripesMaterial.mat",
			},
			collisionSettings = 
			{
				canCollideWith = 
				{
					"Cube",
				}
			}
		} ,
		{
			name = "HelixStripes",
			class = "Helix",
			position  = { 3.0, 0.0, 2.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/helix.dat",
				materialPath = "data/StripesMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
		{
			name = "BaseQuad",
			class = "Plane",
			position  = { 0.0, -1.0, 0.0 },
			velocity  = { 0.0, 0.0, 0.0 },
			acceleration  = { 0.0, 0.0, 0.0 },
			size = {2.0, 2.0, 2.0},
			rotation = 0.0,
			renderSettings = 
			{
				meshPath = "data/plane.dat",
				materialPath = "data/MicrosoftMaterial.mat",
			},
			collisionSettings = 
			{
				
			}
		} ,
	}
}