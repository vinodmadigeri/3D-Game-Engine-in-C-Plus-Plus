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

local function BuildAsset( i_relativeSourcePath, i_relativeTargetPath, i_builderProgramFileName )
	-- A stack level (or call stack depth) can be provided to the error() function
	-- to indicate which level of the call stack the error originated at.
	-- A level of 1 indicates the current function (and is the default).
	-- A level of 2 indicates one level higher.
	-- This means that if someone calls the current function with a bad parameter
	-- we can pass 2 to the error() function so that the _caller's_ file and line number
	-- will show up in the error message.
	local stackLevel = 2

	-- Get the absolute paths to the source and target
	-- (The "source" is the authored asset,
	-- and the "target" is the built asset that is ready to be used in-game.
	-- In this example program we will just copy the source to the target
	-- and so the two will be the same,
	-- but in a real asset build pipeline the two will usually be different:
	-- The source will be in a format that is optimal for authoring purposes
	-- and the target will be in a format that is optimal for real-time purposes.)
	local path_source = s_AuthoredAssetDir .. i_relativeSourcePath
	local path_target = s_BuiltAssetDir .. i_relativeTargetPath

	-- Verify that the source exists
	if not DoesFileExist( path_source ) then
		OutputErrorMessage( "The source asset path \"" .. path_source .. "\" does not exist" )
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
			local command
			do
				if type( i_builderProgramFileName ) == "string" then
					local builderPath = s_BinDir .. i_builderProgramFileName
					if DoesFileExist( builderPath ) then
						command = "\"" .. builderPath .. "\""
					else
						error( "The specified builder (\"" .. builderPath .. "\") doesn't exist", stackLevel )
					end
				else
					error( "The specified builder file name must be a string (instead of a " ..
						type( i_builderProgramFileName ) .. ")", stackLevel )
				end
			end
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
				-- There's a chance that the builder already created the target file,
				-- in which case it will have a new time stamp and wouldn't get built again
				-- even though the process failed
				if DoesFileExist( path_target ) then
					local result, errorMessage = os.remove( path_target )
					if not result then
						OutputErrorMessage( "Failed to delete the incorrectly-built target: " .. errorMessage, path_target )
					end
				end

				return false, exitCode
			end
		end
	else
		return true
	end
end

local function BuildAssets( i_assetsToBuild )
	local wereThereErrors = false

	-- Iterate through every type of assets
	for assetType, assetInfo in pairs( i_assetsToBuild ) do
		-- Get the name of the builder to use for this asset type
		local builderProgramFileName = assetInfo.builder
		-- Iterate through each asset of this type and build it
		local assetsToBuild = assetInfo.assets
		local sourceExtention = assetInfo.extensions.source
		local targetExtension = assetInfo.extensions.target
		
		for i, assetToBuild in ipairs( assetsToBuild ) do
			local sourceAssetToBuild = assetToBuild .. "." .. sourceExtention
			local targetAssetToBuild = assetToBuild .. "." .. targetExtension
			if not BuildAsset( sourceAssetToBuild, targetAssetToBuild, builderProgramFileName ) then
				-- If there's an error then the asset build should fail,
				-- but we can still try to build any remaining assets
				wereThereErrors = true
			end
		end
	end

	return not wereThereErrors
end

-- Entry Point
--============

-- Command line arguments are represented in Lua as three dots ("...")
local commandLineArgument = ...
if commandLineArgument then
	local path_assetsToBuild = commandLineArgument
	if DoesFileExist( path_assetsToBuild ) then
		local assetsToBuild = dofile( path_assetsToBuild )
		return BuildAssets( assetsToBuild )
	else
		OutputErrorMessage( "The path to the list of assets to build that was provided to BuildAssets.lua as argument #1 (\"" ..
			path_assetsToBuild .. "\") doesn't exist" )
	end
else
	OutputErrorMessage( "BuildAssets.lua must be called with the path to the list of assets to build as an argument" )
end
