import bpy
from bpy import context

filepath = bpy.path.abspath("//") + "myscene.scene"
folderpath = bpy.path.abspath("//") + "meshes/"

# Deselect all objects
bpy.ops.object.select_all(action='DESELECT')

export_scale = 1.0

with open(filepath, 'w') as f:
    
    f.write("#MESH MODEL\n")
         
    for object in context.visible_objects:    
        if object.type != 'MESH':
            continue
        
        name = object.data.name + ".obj"

        # Store original mat        
        original_mat = object.matrix_world.copy()
        
        # Set identity to export in 0,0,0 and no rotations
        object.matrix_world.identity()
        
        # Export OBJ
        object.select_set(True)
        bpy.ops.export_scene.obj(filepath=folderpath+name, filter_glob="*.obj;*.mtl", use_selection=True, use_triangles=True, global_scale=export_scale)
        object.select_set(False)

        # Restore transform
        object.matrix_world = original_mat
                
        # Export object in scene file
        f.write("meshes/" + name + " ")

        round_val = 5

        # Export matrix data (Change Y and Z axis and scale translation by global scale)

        f.write( str( round(object.matrix_world[0][0], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[2][0], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[1][0], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[3][0], round_val) ) )
        f.write(",")
        
        f.write( str( round(object.matrix_world[0][1], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[2][1], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[1][1], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[3][1], round_val) ) )
        f.write(",")
        
        f.write( str( round(object.matrix_world[0][2], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[2][2], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[1][2], round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[3][2], round_val) ) )
        f.write(",")
        
        f.write( str( round(object.matrix_world[0][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[2][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(-object.matrix_world[1][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(object.matrix_world[3][3], round_val) ) )
        
        f.write("\n")