import bpy
from .preferences import load_interface


def draw_simulation_time_step(self, context):
    layout = self.layout
    props = context.scene.physics_engine

    row = layout.row(align=True)
    row.enabled = context.scene.use_custom_simulation_range
    row.prop(props, "time_step_value")
    row.prop(props, "time_step_unit", text="")

    row = layout.row(align=True)
    row.operator("physics_engine.run", icon="PLAY")
    row.operator("physics_engine.clear", icon="X")


def draw_physics_add_button(self, context):
    obj = context.object
    if obj is None:
        return
    col = self.layout.column(align=True)
    col.prop(obj.physics_engine, "enabled", text="N-Body", toggle=True, icon="FORCE_FORCE")


class PHYSICS_ENGINE_PT_panel(bpy.types.Panel):
    bl_label = "N-Body Simulation"
    bl_idname = "PHYSICS_ENGINE_PT_panel"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "physics"

    @classmethod
    def poll(cls, context):
        obj = context.object
        return obj is not None and obj.physics_engine.enabled

    def draw(self, context):
        layout = self.layout
        props = context.object.physics_engine

        _, err = load_interface()
        if err:
            box = layout.box()
            box.label(text="Interface unavailable:", icon="ERROR")
            box.label(text=err)
            box.operator(
                "preferences.addon_show",
                text="Open Preferences",
                icon="PREFERENCES",
            ).module = __package__
            layout.separator()

        layout.prop(props, "mass")

        col = layout.column(align=True)
        col.label(text="Initial Velocity:")
        col.prop(props, "velocity", index=0, text="X")
        col.prop(props, "velocity", index=1, text="Y")
        col.prop(props, "velocity", index=2, text="Z")
