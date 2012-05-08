-- Import relevant base types.
local types = require "base.types"
require "base.vec3"
require "base.vec4"
require "base.mat4"

-- Import utility functions.
local geometric = require "base.geometric"

-- Module aliases.
local vec3 = types.vec3
local vec4 = types.vec4
local mat4 = types.mat4
local normalize = geometric.normalize

-- Creates an affine scaling matrix. If v is a vec3, it is a non-uniform scale.
-- If v is a number, it is a uniform scale.
local function scale(v)
    v = vec3.frcoerce(v)
    return mat4.frnew(vec4.frnew(v[1], 0, 0, 0),
                      vec4.frnew(0, v[2], 0, 0),
                      vec4.frnew(0, 0, v[3], 0),
                      vec4.frnew(0, 0, 0, 1))
end

-- Creates an affine rotation matrix. Expects v to be a vector indicating the
-- axis of rotation. Expects a to be a number indicating the angle of rotation
-- around that axis, in radians.
local function rotate(a, v)
    if type(a) ~= "number" then
        error("rotate expects the angle to be a number (in radians)", 2)
    end
    v = vec3.frcoerce(v)

    local c = math.cos(a)
    local s = math.sin(a)
    local axis = normalize(v)

    -- Credit: GLM, in matrix_transform.inl
    local d = vec4.frnew(c + (1 - c) * axis[1] * axis[1],
                         (1 - c) * axis[1] * axis[2] + s * axis[3],
                         (1 - c) * axis[1] * axis[3] - s * axis[2],
                         0)
    local e = vec4.frnew((1 - c) * axis[2] * axis[1] - s * axis[3],
                         c + (1 - c) * axis[2] * axis[2],
                         (1 - c) * axis[2] * axis[3] + s * axis[1],
                         0)
    local f = vec4.frnew((1 - c) * axis[3] * axis[1] + s * axis[2],
                         (1 - c) * axis[3] * axis[2] - s * axis[1],
                         c + (1 - c) * axis[3] * axis[3],
                         0)

    return mat4.frnew(d, e, f, vec4.frnew(0, 0, 0, 1))
end

-- Creates an affine translation matrix. If v is a vec3, it is a non-uniform
-- translation. If v is a number, it is a uniform translation.
local function translate(v)
    v = vec3.frcoerce(v)

    return mat4.frnew(vec4.frnew(1, 0, 0, 0),
                      vec4.frnew(0, 1, 0, 0),
                      vec4.frnew(0, 0, 1, 0),
                      vec4(v, 1))
end

-- Module exports.
return {
    scale = scale,
    rotate = rotate,
    translate = translate
}
