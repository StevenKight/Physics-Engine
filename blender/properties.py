import bpy
from bpy.props import BoolProperty, EnumProperty, FloatProperty, FloatVectorProperty

TIME_STEP_UNITS = [
    ('SECONDS', "Seconds", ""),
    ('MINUTES', "Minutes", ""),
    ('HOURS',   "Hours",   ""),
    ('DAYS',    "Days",    ""),
    ('WEEKS',   "Weeks",   ""),
    ('MONTHS',  "Months",  ""),
    ('YEARS',   "Years",   ""),
]

TIME_STEP_MULTIPLIERS = {
    'SECONDS':      1,
    'MINUTES':      60,
    'HOURS':        3_600,
    'DAYS':         86_400,
    'WEEKS':        604_800,
    'MONTHS':       2_629_800,   # average (365.25 / 12 days)
    'YEARS':        31_557_600,  # Julian year (365.25 days)
}


class PhysicsEngineSceneProperties(bpy.types.PropertyGroup):
    time_step_value: FloatProperty(
        name="Time Step",
        description="Duration of each simulation tick",
        default=1.0,
        min=0.001,
    )
    time_step_unit: EnumProperty(
        name="Unit",
        description="Time unit for the simulation tick",
        items=TIME_STEP_UNITS,
        default='HOURS',
    )


def on_enabled_update(self, context):
    if not self.enabled:
        return
    obj = context.object
    if obj is None:
        return

    if obj.field and obj.field.type != 'NONE':
        obj.field.type = 'NONE'

    physics_types = {'SOFT_BODY', 'COLLISION', 'FLUID', 'CLOTH', 'DYNAMIC_PAINT'}
    for mod in list(obj.modifiers):
        if mod.type in physics_types:
            obj.modifiers.remove(mod)

    if obj.rigid_body is not None:
        bpy.ops.rigidbody.object_remove()

    if obj.rigid_body_constraint is not None:
        bpy.ops.rigidbody.constraint_remove()


class PhysicsEngineObjectProperties(bpy.types.PropertyGroup):
    enabled: BoolProperty(
        name="N-Body Object",
        description="Include this object in the N-Body physics simulation",
        default=False,
        update=on_enabled_update,
    )
    mass: FloatProperty(
        name="Mass",
        description="Mass in kilograms",
        default=1.0,
        min=0.0,
        unit="MASS",
    )
    velocity: FloatVectorProperty(
        name="Initial Velocity",
        description="Initial velocity in m/s",
        default=(0.0, 0.0, 0.0),
        subtype="VELOCITY",
    )
