fn getMCVRootFolder = (
	-- To be changed
	return "C:/Code2022/engine/"
)

fn isCollider obj = (
	return obj != undefined and ( findString obj.name "collider" != undefined )
)

fn isTrigger obj = (
	return obj != undefined and (findString obj.name "trigger" != undefined )
)

fn inMeters pos = (
	if units.SystemType == #centimeters then return ( pos * 0.01 )
	if units.SystemType == #inches then return ( pos / 2.54 )
	if units.SystemType == #meters then return ( pos )
	return pos
)

-- -------------------------------------------------------------
struct TMeshGroup (
	first_index = 0,
	num_indices = 0,
	max_material_index = 0,
	mat
)

-- #( #(face_id using mat1), undef, undef ... #(faces using mat8))
function get_faces_by_material obj = (
	local results = #()
	local num_faces = getNumFaces obj
	for face_id = 1 to num_faces do (
		local mat_id = getfacematid obj face_id 
		if results[ mat_id ] == undefined then (
			results[ mat_id ] = #()
		)
		append results[ mat_id ] face_id
	)
	return results
)