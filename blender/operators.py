"""
Blender operators for running and clearing the N-Body simulation bake.

Provides two operators:

- ``PHYSICS_ENGINE_OT_run``  — steps the simulation frame-by-frame and inserts
  location keyframes on every N-Body object.
- ``PHYSICS_ENGINE_OT_clear`` — removes those keyframes and restores each
  object to its pre-bake position.
"""

import bpy
from .preferences import load_interface


_TIME_STEP_MULTIPLIERS = {
    'DAYS':         86_400,
    'HOURS':        3_600,
    'MINUTES':      60,
    'SECONDS':      1,
    'MILLISECONDS': 1e-3,
    'MICROSECONDS': 1e-6,
}


def time_conversion(time: float, unit: str | None = None) -> float:
    """
    Convert a time value from the given Blender time unit to seconds.

    Args:
        time: The numeric time-step value entered by the user.
        unit: A Blender time-unit identifier (e.g. ``'SECONDS'``, ``'MINUTES'``).
              Defaults to a 1:1 multiplier when ``None`` or unrecognised.

    Returns:
        The equivalent duration in seconds.
    """
    time_to_s = _TIME_STEP_MULTIPLIERS.get(unit, 1.0) if unit else 1.0
    return time * time_to_s


class PHYSICS_ENGINE_OT_run(bpy.types.Operator):
    """Bake the N-Body simulation and insert location keyframes for each participant."""

    bl_idname = "physics_engine.run"
    bl_label = "Run Simulation"
    bl_description = "Bake the N-Body simulation and insert location keyframes"

    def execute(self, context):
        """
        Step the simulation over the scene's frame range and keyframe each object.

        Loads the Physics Engine interface, builds ``PhysicsObject`` instances from
        the scene, then advances the simulation one step per frame while inserting
        a location keyframe. A Blender progress bar tracks the bake.

        Returns:
            ``{'FINISHED'}`` on success, ``{'CANCELLED'}`` if the interface cannot
            be loaded or no N-Body objects are present.
        """
        result, err = load_interface()
        if err or not result:
            self.report({'ERROR'}, err or "An unknown error occured")
            return {'CANCELLED'}

        PhysicsObject, sim_run = result

        objects = [obj for obj in context.scene.objects if obj.physics_engine.enabled]
        if not objects:
            self.report({'ERROR'}, "No N-Body objects in the scene.")
            return {'CANCELLED'}

        scene = context.scene
        scene_props = scene.physics_engine

        # Respect a custom simulation range when set, otherwise use the render range.
        if scene.use_custom_simulation_range:
            start = scene.simulation_frame_start
            end = scene.simulation_frame_end
        else:
            start = scene.frame_start
            end = scene.frame_end

        time_step = time_conversion(scene_props.time_step_value, scene.unit_settings.time_unit)

        saved_frame = scene.frame_current

        # Snapshot each object's current location and build the simulation state.
        sim_objects = []
        for obj in objects:
            obj["pe_initial_location"] = tuple(obj.location)

            sim_objects.append(PhysicsObject(
                mass=obj.physics_engine.mass,
                x=obj.location.x,
                y=obj.location.y,
                z=obj.location.z,
                vx=obj.physics_engine.velocity[0],
                vy=obj.physics_engine.velocity[1],
                vz=obj.physics_engine.velocity[2],
            ))

        # Insert the initial keyframe at the start frame before advancing.
        for obj in objects:
            obj.keyframe_insert(data_path="location", frame=start)

        wm = context.window_manager
        total = end - start
        wm.progress_begin(0, total)

        # Advance the simulation one step per frame and record the resulting positions.
        for i, frame in enumerate(range(start + 1, end + 1)):
            sim_run(sim_objects, time_step, num_steps=1)
            for obj, sim_obj in zip(objects, sim_objects):
                obj.location = (
                    sim_obj.position.x,
                    sim_obj.position.y,
                    sim_obj.position.z,
                )
                obj.keyframe_insert(data_path="location", frame=frame)
            wm.progress_update(i)

        wm.progress_end()

        scene.frame_set(saved_frame)
        self.report({'INFO'}, f"Simulation baked: frames {start}-{end}.")
        return {'FINISHED'}


class PHYSICS_ENGINE_OT_clear(bpy.types.Operator):
    """Remove baked simulation keyframes and restore each N-Body object to its initial position."""

    bl_idname = "physics_engine.clear"
    bl_label = "Clear Simulation"
    bl_description = "Remove baked simulation keyframes from all N-Body objects"

    def execute(self, context):
        """
        Delete location F-curves written by the bake and restore pre-bake positions.

        Traverses Blender's layered animation data model (action → layer → strip →
        channelbag) to remove only the ``location`` F-curves inserted by this addon,
        leaving any other animation data untouched.

        Returns:
            ``{'FINISHED'}`` on success, ``{'CANCELLED'}`` if no N-Body objects exist.
        """
        objects = [obj for obj in context.scene.objects if obj.physics_engine.enabled]
        if not objects:
            self.report({'WARNING'}, "No N-Body objects in the scene.")
            return {'CANCELLED'}

        # Walk the layered animation hierarchy to find and remove location F-curves.
        for obj in objects:
            if not obj.animation_data or not obj.animation_data.action:
                continue
            action = obj.animation_data.action
            slot = obj.animation_data.action_slot
            for layer in action.layers:
                for strip in layer.strips:
                    channelbag = strip.channelbag(slot)
                    if channelbag is None:
                        continue
                    to_remove = [fc for fc in channelbag.fcurves if fc.data_path == "location"]
                    for fc in to_remove:
                        channelbag.fcurves.remove(fc)

        # Restore each object to the position it held before the bake was run.
        for obj in objects:
            initial = obj.get("pe_initial_location")
            if initial is not None:
                obj.location = initial
                del obj["pe_initial_location"]

        context.scene.frame_set(0)

        self.report({'INFO'}, f"Cleared simulation keyframes from {len(objects)} object(s).")
        return {'FINISHED'}
