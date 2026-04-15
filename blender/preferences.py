import sys
import bpy
from bpy.props import StringProperty


def get_prefs():
    return bpy.context.preferences.addons[__package__].preferences


def load_interface():
    prefs = get_prefs()
    root = prefs.project_root.strip()
    if not root:
        return None, "Physics Engine project root is not set in addon preferences."
    if root not in sys.path:
        sys.path.insert(0, root)
    try:
        from interface.nbody import PhysicsObject, sim_run
        return (PhysicsObject, sim_run), None
    except FileNotFoundError as e:
        return None, str(e)
    except ImportError as e:
        return None, str(e)


class PhysicsEnginePreferences(bpy.types.AddonPreferences):
    bl_idname = __package__

    project_root: StringProperty(
        name="Project Root",
        description="Absolute path to the Physics Engine project directory",
        subtype="DIR_PATH",
        default="",
    )

    def draw(self, context):
        self.layout.prop(self, "project_root")
