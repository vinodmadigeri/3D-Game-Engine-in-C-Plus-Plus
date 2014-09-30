--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

local s_AuthoredAssetDir, s_BuiltAssetDir
local s_BinDir
do
	-- AuthoredAssetDir
	do
		local key = "AuthoredAssetDir"
		s_AuthoredAssetDir = os.getenv( key )
		if not s_AuthoredAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
	-- BuiltAssetDir
	do
		local key = "BuiltAssetDir"
		s_BuiltAssetDir = os.getenv( key )
		if not s_BuiltAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end

	-- BinDir
	do
		local key = "BinDir"
		s_BinDir = os.getenv( key )
		if not s_BinDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
end

-- Function Definitions
--=====================

function BuildAsset( i_relativePath )
	-- Get the absolute paths to the source and target
	-- (The "source" is the authored asset,
	-- and the "target" is the built asset that is ready to be used in-game.
	-- In this example program we will just copy the source to the target
	-- and so the two will be the same,
	-- but in a real asset build pipeline the two will usually be different:
	-- The source will be in a format that is optimal for authoring purposes
	-- and the target will be in a format that is optimal for real-time purposes.)
	local path_source = s_AuthoredAssetDir .. i_relativePath
	local path_target = s_BuiltAssetDir .. i_relativePath

	-- Verify that the source exists
	if not DoesFileExist( path_source ) then
		OutputErrorMessage( "The source path \"" .. path_source .. "\" specified to build does not exist" )
		return false
	end

	-- Decide if the target needs to be built
	local shouldTargetBeBuilt
	do
		-- The simplest reason a target should be built is if it doesn't exist
		local doesTargetExist = DoesFileExist( path_target )
		if doesTargetExist then
			-- Even if the target exists it may be out-of-date.
			-- If the source has been modified more recently than the target
			-- then the target should be re-built.
			local lastWriteTime_source = GetLastWriteTime( path_source )
			local lastWriteTime_target = GetLastWriteTime( path_target )
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target
		else
			shouldTargetBeBuilt = true;
		end
	end

	-- Build the target if necessary
	if shouldTargetBeBuilt then
		-- Create the target directory if necessary
		CreateDirectoryIfNecessary( path_target )
		-- Build
		do
			-- Use the appropriate builder for this asset type
			-- (As of this comment there is only a single generic builder)
			local builderFileName = "GenericBuilder.exe"
			local command = "\"" .. s_BinDir .. builderFileName .. "\""
			-- The source and target path must always be passed in
			local arguments = "\"" .. path_source .. "\" \"" .. path_target .. "\""
			-- [If a specific builder uses extra arguments add them here]

			-- Surround the entire command line in quotes
			local commandLine = "\"" .. command .. " " .. arguments .. "\""
			local result, terminationType, exitCode = os.execute( commandLine )
			if result then
				-- Display a message for each asset
				print( "Built " .. path_source )
				return true, exitCode
			else
				-- The builder should already output a descriptive error message,
				-- but you can do another one if you wish
				-- (this can be helpful if there is a bug in a specific builder
				-- which causes it to _not_ output an error message;
				-- without a final error message here it can be confusing
				-- why the asset build failed otherwise)
				do
					local errorMessage = "The command " .. tostring( commandLine )
					if terminationType ~= "signal" then
						errorMessage = errorMessage .. " exited with code "
					else
						errorMessage = errorMessage .. " was terminated by the signal "
					end
					errorMessage = errorMessage .. tostring( exitCode )
					OutputErrorMessage( errorMessage, path_source )
				end

				return false, exitCode
			end
		end
	else
		return true
	end
end
