struct TMaterialExporter (
	fs = TJsonFormatter(),
	scene_exporter,
	material_filenames = #(),
	
	fn quoteFile txt = (
		local file = substituteString txt "\\" "/"
		return "\"" + file + "\""
	),
	
	-- Other formats, please look at "C:/Code/engine/tools/"
	-- out_format = BC1_UNORM
	fn convertTexture in_file out_file out_format = (
		-- Convert a non-dds file to dds
		local tools_path = scene_exporter.bin_path + "../tools/"
		
		-- Quote the conversor jus in the case tools_path contains spaces
		local conversor = tools_path + "texconv.exe"
		local cmd = quoteFile conversor
		cmd = cmd + " -f " + out_format		-- format
		cmd = cmd + " -pow2"
		local opath = getFilenamePath out_file
		cmd = cmd + " " + (quoteFile in_file)
		
		-- Quoting everything so the doscommand does not complain
		cmd = quoteFile cmd 
		
		-- The conversion leaves the converted filename .dds in the same folder as the input file
		format "%\n" cmd
		doscommand cmd
		
		-- Copy the resulting file to our destination
		local tmp_file = (getFilenamePath in_file)  + (getFilenameFile in_file) + ".dds"
		
		if not doesFileExist tmp_file then (
			throw ("Failed to convert input texture to dds format\n" +  cmd + "\nExpected file is missing:\n" + tmp_file)
		)
		
		format "copying % to %\n" tmp_file out_file
		renameFile tmp_file out_file
	),
	
	fn exportMap key_name map default_name = (
		
		if map == undefined then (
			fs.writeKeyValue key_name default_name
			return true
		)
		
		-- Use max to resolve the full path in the local machine
		local jpg_full_filename = (map.filename as AssetUser).getFullFilePath() -- desktop/batman.jpg
		local base_name = getFilenameFile jpg_full_filename			-- batman
		local out_extension = ".dds"
		local dds_resource = base_name + out_extension
		local resource_name = scene_exporter.textures_path + dds_resource
		fs.writeKeyValue key_name resource_name
		
		local dds_full_filename = scene_exporter.bin_path + resource_name
		
		local dds_format = "BC1_UNORM"
		
		-- Convert the jpg_full_filename into dds_full_filename??
		if not doesFileExist dds_full_filename then (
			local curr_ext = getFilenameType jpg_full_filename
			if curr_ext == out_extension then (
				-- The input file is already a .dds file
				format "Copying dds % to %\n" jpg_full_filename dds_full_filename
				copyFile jpg_full_filename dds_full_filename
			) else (
				format "Converting % to %\n" jpg_full_filename dds_full_filename
				convertTexture jpg_full_filename dds_full_filename dds_format
			)
		)
	),
	
	fn exportStdMaterial mat = (
		local ofilename = mat.name + ".mat"
		local resource_name = scene_exporter.materials_path + ofilename
		local output_filename = scene_exporter.bin_path + resource_name
		format "Exporting material %\n" output_filename
		fs.begin output_filename
		fs.beginObj()
			exportMap "albedo" mat.diffuseMap "data/textures/white.dds"
			fs.writeComma()
			exportMap "normal" mat.bumpMap "data/textures/null_normal.dds"
		fs.endObj()
		fs.end()
		append material_filenames resource_name
	),
	
	fn exportMultiMaterial multimat = (
		format "exporting multimaterial %" multimat
		
		local groups = get_faces_by_material scene_exporter.curr_obj
		for idx = 1 to groups.count do (
			local mesh_group = groups[ idx ] 
			if mesh_group == undefined then continue
			local mat_for_this_group = multimat[idx]
			if mat_for_this_group == undefined then (
				-- Fake with a null for this position, the scene_exporter will then assign the default material
				append material_filenames undefined
			) else (
				exportStdMaterial multimat[idx]
			)
		)
		
	),
	
	fn exportMaterial mat = (
		if classof mat == StandardMaterial then (
			exportStdMaterial mat
		) else if classof mat == Multimaterial then (
			exportMultiMaterial mat
		) else if mat == undefined then (
			return undefined
		) else (
			--throw ("I don't know how to export material of class " + ((classof mat) as string) + " in object " + scene_exporter.curr_obj.name )
			throw "Invalid mat class"
		)
	)
)
