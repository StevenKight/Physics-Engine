"""
Blender addon entry point for the Physics Engine N-Body simulation integration.

Registers all classes, properties, and UI extensions required by the addon,
and cleans them up on unregister.
"""

import bpy
from .preferences import PhysicsEnginePreferences
from .properties import PhysicsEngineObjectProperties, PhysicsEngineSceneProperties
from .panels import PHYSICS_ENGINE_PT_panel, draw_physics_add_button, draw_simulation_time_step
from .operators import PHYSICS_ENGINE_OT_run, PHYSICS_ENGINE_OT_clear


def register():
    """Register all addon classes and attach custom properties to Blender types."""
    bpy.utils.register_class(PhysicsEnginePreferences)
    bpy.utils.register_class(PHYSICS_ENGINE_OT_run)
    bpy.utils.register_class(PHYSICS_ENGINE_OT_clear)
    bpy.utils.register_class(PhysicsEngineSceneProperties)
    bpy.types.Scene.physics_engine = bpy.props.PointerProperty(
        type=PhysicsEngineSceneProperties
    )
    bpy.utils.register_class(PhysicsEngineObjectProperties)
    bpy.types.Object.physics_engine = bpy.props.PointerProperty(
        type=PhysicsEngineObjectProperties
    )
    bpy.utils.register_class(PHYSICS_ENGINE_PT_panel)
    bpy.types.PHYSICS_PT_add.append(draw_physics_add_button)
    bpy.types.SCENE_PT_simulation.append(draw_simulation_time_step)


def unregister():
    """Unregister all addon classes and remove custom properties from Blender types."""
    bpy.types.SCENE_PT_simulation.remove(draw_simulation_time_step)
    bpy.types.PHYSICS_PT_add.remove(draw_physics_add_button)
    bpy.utils.unregister_class(PHYSICS_ENGINE_PT_panel)
    del bpy.types.Object.physics_engine
    bpy.utils.unregister_class(PhysicsEngineObjectProperties)
    del bpy.types.Scene.physics_engine
    bpy.utils.unregister_class(PhysicsEngineSceneProperties)
    bpy.utils.unregister_class(PHYSICS_ENGINE_OT_clear)
    bpy.utils.unregister_class(PHYSICS_ENGINE_OT_run)
    bpy.utils.unregister_class(PhysicsEnginePreferences)


if __name__ == "__main__":
    register()
