clearListener()

struct TChildrenInfo (
	children = #(),
	physics_shapes = #(),
	parent = undefined,
	on create do (
		for child in parent.children do (
			if isCollider child or isTrigger child then (
				append physics_shapes child
			) else (
				append children child 
			)
		)
	)
)

struct TSceneExporter (
	fs = TJsonFormatter(),
	bin_path, 			-- E:/Code/mcv/bin/
	scene_name,
	output_data_path,
	output_filename,
	scenes_path = "data/scenes/",	
	meshes_path = "data/meshes/",
	textures_path = "data/textures/",
	materials_path = "data/materials/",
	collisions_path = "data/collision_meshes/",
	objs_exported,
	
	-- Context
	curr_obj,
	
	fn exportCompName = (
		fs.writeKeyValue "name" curr_obj.name
	),
	
	fn exportCompTransform = (
		fs.writeKey "transform"
		local max_transform = curr_obj.transform
		local max_to_mcv = RotateXMatrix -90
		local mcv_to_max = RotateXMatrix 90
		local mcv_transform = mcv_to_max * max_transform * max_to_mcv
		
		local mcv_pos_meters = inMeters( mcv_transform.position )
		local mcv_scale = mcv_transform.scale
		local mcv_rot = mcv_transform.rotation
		fs.beginObj()
			fs.writeKeyValue "pos" mcv_pos_meters
			fs.writeComma()
			fs.writeKeyValue "scale" mcv_scale
			fs.writeComma()
			fs.writeKeyValue "rot" mcv_rot
		fs.endObj()
	),
	
	fn exportMaterialResource = (
		local mat_exporter = TMaterialExporter()
		mat_exporter.scene_exporter = this
		mat_exporter.exportMaterial curr_obj.mat
		return mat_exporter.material_filenames
	),
	
	fn exportDrawCall mesh_group mesh_group_idx = (
		local mat_resources = exportMaterialResource()
		local mesh_resource = meshes_path + curr_obj.name + ".mesh"
		local mat_resource = mat_resources[mesh_group_idx]
		if mat_resource == undefined then mat_resource = materials_path + "missing.mat"
		fs.beginObj()
			fs.writeKeyValue "mesh" mesh_resource
				fs.writeComma()
			fs.writeKeyValue "mat" mat_resource
				fs.writeComma()
			fs.writeKeyValue "mesh_group" (mesh_group_idx - 1)
		fs.endObj()
	),
	
	fn exportCompRender = (
		
		-- Ensure the obj is an editable mesh
		local curr_mesh = curr_obj
		if classof curr_mesh != Editable_Mesh then (
			-- Do a copy, and convert the copy to an EditableMesh
			curr_mesh = copy curr_obj
			convertToMesh curr_mesh
			curr_mesh.name = curr_obj.name
		)
		
		-- Scan materials used by this obj
		local mesh_groups = get_faces_by_material curr_mesh
		
		-- There are more material groups but the material is not a multi-material
		if mesh_groups.count > 1 and classof curr_mesh.mat != MultiMaterial then (
			format "Hey, the mesh has % groups, but the material % is not a multimaterial\n" mesh_groups.count curr_mesh.mat.name 
			if curr_mesh != curr_obj then (
				format "Setting all faces to use the first material id\n"
				for face_id = 1 to getNumFaces curr_mesh do (
					setfacematid curr_mesh face_id 1
				)
				mesh_groups = get_faces_by_material curr_mesh
			)
		)
		
		fs.writeKey "render"
		
		fs.beginArray()
		
			local group_idx = 1
			for mesh_group in mesh_groups do (
				if mesh_group == undefined then continue
				if group_idx > 1 then fs.writeComma()
				exportDrawCall mesh_group group_idx
				group_idx = group_idx + 1
			)
		
		fs.endArray()
			
		-- Do the real mesh geometry export once
		local mesh_resource = meshes_path + curr_obj.name + ".mesh"
		local vertex_type = undefined
		export_mesh curr_mesh (bin_path + mesh_resource) vertex_type
			
		-- Delete the temporal copy created in case the curr_obj was not a mesh
		if curr_mesh != curr_obj then delete curr_mesh
	),
	
	fn exportCompHierarchy = (
		fs.writeKey "hierarchy"
		fs.beginObj()
			
		fs.endObj()
	),
	
	fn exportShape objshape = (
		fs.beginObj()
		if classof objshape == Sphere then (
			fs.writeKeyValue "shape" "sphere"
			fs.writeComma()
			fs.writeKeyValue "radius" ( inMeters objshape.radius )
			
		) else if classof objshape == Box then (
			fs.writeKeyValue "shape" "box"
			fs.writeComma()
			-- Check the sizes x,y,z order is correct!!
			local half_sizes = [objshape.width, objshape.height, objshape.length] * 0.5
			fs.writeKeyValue "half_size" ( inMeters half_sizes )
			
		) else if classof objshape == Capsule then (
			fs.writeKeyValue "shape" "capsule"
			fs.writeComma()
			fs.writeKeyValue "radius" ( inMeters objshape.radius )
			fs.writeComma()
			fs.writeKeyValue "height" ( inMeters objshape.height )
			
		) else (
			
			local as_convex_mesh_str = getUserProp curr_obj "convex" 
			local as_convex_mesh = (as_convex_mesh_str == 1) or (as_convex_mesh_str == "true")
			format "as_convex_mesh % str --%--\n" as_convex_mesh as_convex_mesh_str
			if as_convex_mesh == true then (
				fs.writeKeyValue "shape" "convex"
			) else (
				fs.writeKeyValue "shape" "trimesh"
			)
			
			local cmesh_resource = collisions_path + objshape.name + ".cmesh"
			fs.writeComma()
			fs.writeKeyValue "collision_mesh" cmesh_resource
			
			-- Do the real export to cmesh
			export_mesh objshape (bin_path + cmesh_resource) "Pos"
		)
		
		if isTrigger objshape then (
			fs.writeComma()
			fs.writeKeyValue "trigger" true
		)
		
		-- compute offset wrt my parent
		-- export as matrix and attribute 'offset'
		
		if (getUserProp curr_obj "group") != undefined then (
      fs.writeComma()
			fs.writeKeyValue "group" (getUserProp curr_obj "group")
		)
    
		if (getUserProp curr_obj "mask") != undefined then (
      fs.writeComma()
			fs.writeKeyValue "mask" (getUserProp curr_obj "mask")
		)
		
		fs.endObj()
	),
	
	fn exportCompCollider children_info = (
		fs.writeKey "collider"
		
		if getUserProp curr_obj "dynamic" != undefined then (
			fs.writeKeyValue "dynamic" true
		)
		
		if getUserProp curr_obj "kinematic" != undefined then (
			fs.writeKeyValue "kinematic" true
		)
		
		fs.beginObj()
			fs.writeKey "shapes"
			fs.beginArray()
			local nshapes = 0
			for obj in children_info.physics_shapes do (
				if nshapes > 0 then fs.writeComma()
				exportShape obj
				nshapes = nshapes + 1
			)
			fs.endArray()
		fs.endObj()
	),
	
	fn exportObj obj parent_obj = (
		
		local children_info = TChildrenInfo parent:obj
		
		curr_obj = obj
		format "Exporting % to %\n" obj.name scene_name
		fs.beginObj()
		fs.writeKey "entity"
		fs.beginObj()
			exportCompName()
				fs.writeComma()
			exportCompTransform()
				fs.writeComma()
			exportCompRender()
			
			if children_info.physics_shapes.count > 0 then (
				fs.writeComma()
				exportCompCollider children_info
			)
			
			if parent_obj != undefined then (
				fs.writeComma()
				exportCompHierarchy parent_obj
			)
			
			-- Export cust Attributes of this obj...
			local num_ca = custAttributes.count obj
			for idx_ca = 1 to num_ca do (
				local ca = custAttributes.get obj idx_ca
				ca.exportAsComponent fs
			)
			
		fs.endObj()
		fs.endObj()
		
		-- If obj has children which are not collisions or triggers, export them having me as parent
		for child in children_info.children do (
			exportObj child obj
		)
	),
	
	fn exportScene = (
		bin_path = getMCVRootFolder() + "bin/"
		output_data_path = bin_path + "data/"
		scene_name = getFilenameFile maxfilename
		if scene_name == "" then scene_name = "unnamed"
		output_filename = bin_path + scenes_path + scene_name + ".json"
		fs.begin output_filename
		fs.beginArray()
		local nobjs = 0
		for obj in $* do (
			if obj.parent != undefined then continue
			
			if nobjs > 0 then fs.writeComma()
			exportObj obj undefined
			nobjs = nobjs + 1
		)
		fs.endArray()
		fs.end()
	)
)

--se = TSceneExporter()
--se.exportScene()
--ci = TChildrenInfo parent:$


