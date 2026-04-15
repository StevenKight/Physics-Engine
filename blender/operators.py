import bpy
from .preferences import load_interface
from .properties import TIME_STEP_MULTIPLIERS


class PHYSICS_ENGINE_OT_run(bpy.types.Operator):
    bl_idname = "physics_engine.run"
    bl_label = "Run Simulation"
    bl_description = "Bake the N-Body simulation and insert location keyframes"

    def execute(self, context):
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

        if scene.use_custom_simulation_range:
            start = scene.simulation_frame_start
            end = scene.simulation_frame_end
        else:
            start = scene.frame_start
            end = scene.frame_end

        time_step = scene_props.time_step_value * TIME_STEP_MULTIPLIERS[scene_props.time_step_unit]

        saved_frame = scene.frame_current

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

        for obj in objects:
            obj.keyframe_insert(data_path="location", frame=start)

        wm = context.window_manager
        total = end - start
        wm.progress_begin(0, total)

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
    bl_idname = "physics_engine.clear"
    bl_label = "Clear Simulation"
    bl_description = "Remove baked simulation keyframes from all N-Body objects"

    def execute(self, context):
        objects = [obj for obj in context.scene.objects if obj.physics_engine.enabled]
        if not objects:
            self.report({'WARNING'}, "No N-Body objects in the scene.")
            return {'CANCELLED'}

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

        for obj in objects:
            initial = obj.get("pe_initial_location")
            if initial is not None:
                obj.location = initial
                del obj["pe_initial_location"]

        context.scene.frame_set(0)

        self.report({'INFO'}, f"Cleared simulation keyframes from {len(objects)} object(s).")
        return {'FINISHED'}
