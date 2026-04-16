"""
Custom Blender property groups for the Physics Engine addon.

Defines per-scene simulation settings and per-object N-Body physics properties,
including a callback that strips conflicting Blender physics systems when an
object is added to the N-Body simulation.
"""

import bpy
from bpy.props import BoolProperty, FloatProperty, FloatVectorProperty


class PhysicsEngineSceneProperties(bpy.types.PropertyGroup):
    """Scene-level settings for the N-Body simulation, stored on ``bpy.types.Scene``."""

    time_step_value: FloatProperty(
        name="Time Step",
        description="Duration of each simulation tick in the unit of time of the scene.",
        default=1.0,
        min=0.001,
    )


def on_enabled_update(self, context):
    """
    Callback fired when an object's N-Body ``enabled`` flag is toggled on.

    Removes any conflicting Blender physics systems (force fields, soft body,
    cloth, collision, fluid, dynamic paint, and rigid bodies) so the object is
    exclusively managed by the N-Body simulation.
    """
    if not self.enabled:
        return
    obj = context.object
    if obj is None:
        return

    # Clear force field — it interferes with custom gravitational integration.
    if obj.field and obj.field.type != 'NONE':
        obj.field.type = 'NONE'

    # Remove modifiers that imply a competing physics system.
    physics_types = {'SOFT_BODY', 'COLLISION', 'FLUID', 'CLOTH', 'DYNAMIC_PAINT'}
    for mod in list(obj.modifiers):
        if mod.type in physics_types:
            obj.modifiers.remove(mod)

    # Remove rigid body and any associated constraint.
    if obj.rigid_body is not None:
        bpy.ops.rigidbody.object_remove()

    if obj.rigid_body_constraint is not None:
        bpy.ops.rigidbody.constraint_remove()


class PhysicsEngineObjectProperties(bpy.types.PropertyGroup):
    """Per-object N-Body simulation properties, stored on ``bpy.types.Object``."""

    enabled: BoolProperty(
        name="N-Body Object",
        description="Include this object in the N-Body physics simulation",
        default=False,
        update=on_enabled_update,
    )
    mass: FloatProperty(
        name="Mass",
        description="How much the object 'weighs' irrespective of gravity.",
        default=1.0,
        min=0.0,
        unit="MASS",
    )

    velocity: FloatVectorProperty(
        name="Initial Velocity",
        description="Initial velocity in scene units.",
        default=(0.0, 0.0, 0.0),
        subtype="VELOCITY",
        unit="VELOCITY",
    )
