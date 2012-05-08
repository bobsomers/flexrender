-- Import base types.
local types = require "base.types"
require "base.common"
require "base.vec2"
require "base.vec3"
require "base.vec4"
require "base.mat4"

-- Parent base types to common type.
setmetatable(types.vec2, types.common)
setmetatable(types.vec3, types.common)
setmetatable(types.vec4, types.common)
setmetatable(types.mat4, types.common)

-- Import utility functions.
local trig = require "base.trig"
local geometric = require "base.geometric"
local transform = require "base.transform"

-- Module exports.
return {
    vec2 = types.vec2,
    vec3 = types.vec3,
    vec4 = types.vec4,
    mat4 = types.mat4,
    radians = trig.radians,
    degrees = trig.degrees,
    length = geometric.length,
    distance = geometric.distance,
    dot = geometric.dot,
    cross = geometric.cross,
    normalize = geometric.normalize,
    scale = transform.scale,
    rotate = transform.rotate,
    translate = transform.translate
}
