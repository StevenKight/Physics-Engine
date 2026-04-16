"""
Addon preferences and Physics Engine interface loader.

Provides access to user-configured addon preferences and a utility for
dynamically importing the simulation interface from the Physics Engine
project root set by the user.
"""

import sys
import bpy
from bpy.props import StringProperty


def get_prefs():
    """Return the addon preferences for this package."""
    return bpy.context.preferences.addons[__package__].preferences


def load_interface():
    """
    Import and return the simulation interface from the Physics Engine project.

    Ensures the configured project root is on ``sys.path``, then imports
    ``PhysicsObject`` and ``sim_run`` from ``interface.nbody``.

    Returns:
        tuple: ``((PhysicsObject, sim_run), None)`` on success, or
               ``(None, error_message)`` if the root is unset or the import fails.
    """
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
    """Addon preferences panel for configuring the Physics Engine project root."""

    bl_idname = __package__

    project_root: StringProperty(
        name="Project Root",
        description="Absolute path to the Physics Engine project directory",
        subtype="DIR_PATH",
        default="",
    )

    def draw(self, context):
        """Draw the project root path field in the addon preferences panel."""
        self.layout.prop(self, "project_root")
