utility MyMCVExporter "MCV Exporter"
(
	
	rollout exporter "The Exporter"
	(
		button btn_mesh "Export Mesh"
		button btn_collision "Export Collision"
		button btn_scene "Export Scene"
		\
		button btn_skel_data "Export Skeleton And Meshes"
		button btn_skel_anim "Export Skeleton Animation"

		-- Exports the current selection mesh to file
		on btn_mesh pressed do
		(
			try (
				if $ == undefined then throw "You must select an object to export"
				gc()
				export_mesh $ undefined undefined
				MessageBox "Single mesh exported OK"
			)
			catch (
				messageBox ("Error Exporting Single Mesh:\n" + getCurrentException())
			)
		)
		
		-- Exports the current selection mesh to file
		on btn_collision pressed do
		(
			try (
				if $ == undefined then throw "You must select an object to export as collision"
				gc()
				export_mesh $ undefined "Pos"
				MessageBox "Single mesh exported OK"
			)
			catch (
				messageBox ("Error Exporting Single Collision Mesh:\n" + getCurrentException())
			)
		)
		
		-- Exports the current selection mesh to file
		on btn_scene pressed do
		(
			try (
				gc()
				local se = TSceneExporter()
				se.exportScene()
				MessageBox "Scene exported OK"
			)
			catch (
				messageBox ("Error Exporting Scene:\n" + getCurrentException())
			)
		)
		
		-- Exports the current selection mesh to file
		on btn_skel_data pressed do
		(
			try (
				local se = TSkeletonsExporter()
				se.exportSkelAndMeshes()
				MessageBox "Skeleton and Meshes exported OK"
			)
			catch (
				messageBox ("Error Exporting Skeleton:\n" + getCurrentException())
			)
		)
		
		-- Exports the current selection mesh to file
		on btn_skel_anim pressed do
		(
			try (
				local se = TSkeletonsExporter()
				se.exportAnim()
				MessageBox "Skeleton Animation exported OK"
			)
			catch (
				messageBox ("Error Exporting Skeleton:\n" + getCurrentException())
			)
		)
	)
	
	-- ...
	on MyMCVExporter open do
	(
		--- read from an .ini the current directory path...
		addRollout exporter
	) 
	on MyMCVExporter close do
	(
		removeRollout exporter
	) 
	
)
