mcv_scripts_path = getDir #userStartupScripts
mcv_scripts_path += "/../"
format "Hi from MCV boot %" mcv_scripts_path

filein (mcv_scripts_path + "mcv_common.ms")
filein (mcv_scripts_path + "mesh_exporter.ms")
filein (mcv_scripts_path + "jsonFormatter.ms")
filein (mcv_scripts_path + "material_exporter.ms")
filein (mcv_scripts_path + "scene_exporter.ms")
filein (mcv_scripts_path + "skeleton_exporter.ms")
filein (mcv_scripts_path + "ui_exporter.ms")