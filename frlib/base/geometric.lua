-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"
require "base.vec4"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4

-- Computes the scalar length of a vector.
local function length(v)
    local mt = getmetatable(v)

    if mt == vec2 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2])
    elseif mt == vec3 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3])
    elseif mt == vec4 then
        return math.sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3] + v[4] * v[4])
    end

    error("length is only defined for vec2/vec3/vec4", 2)
end

-- Computes the distance between two points, represented as vectors.
local function distance(p1, p2)
    local mt = getmetatable(p1)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("distance is only defined for vec2/vec3/vec4", 2)
    end
    if mt ~= getmetatable(p2) then
        error("arguments to distance must be of the same type", 2)
    end

    return length(p1 - p2)
end

-- Computes the dot product of two vectors.
local function dot(a, b)
    local mt = getmetatable(a)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("dot is only defined for vec2/vec3/vec4", 2)
    end
    if mt ~= getmetatable(b) then
        error("arguments to dot must be of the same type", 2)
    end

    if mt == vec2 then
        return a[1] * b[1] + a[2] * b[2]
    elseif mt == vec3 then
        return a[1] * b[1] + a[2] * b[2] + a[3] * b[3]
    elseif mt == vec4 then
        return a[1] * b[1] + a[2] * b[2] + a[3] * b[3] + a[4] * b[4]
    end
end

-- Computes the cross product of two 3D vectors.
local function cross(a, b)
    if getmetatable(a) ~= vec3 or getmetatable(b) ~= vec3 then
        error("cross is only defined for vec3", 2)
    end

    return vec3.frnew(a[2] * b[3] - b[2] * a[3],
                      a[3] * b[1] - b[3] * a[1],
                      a[1] * b[2] - b[1] * a[2])
end

-- Normalizes a vector to unit length.
local function normalize(v)
    local mt = getmetatable(v)

    if not(mt == vec2 or mt == vec3 or mt == vec4) then
        error("normalize is only defined for vec2/vec3/vec4", 2)
    end

    return v / length(v)
end

-- Module exports.
return {
    length = length,
    distance = distance,
    dot = dot,
    cross = cross,
    normalize = normalize
}
