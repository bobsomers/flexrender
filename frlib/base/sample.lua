-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"
require "base.vec4"
require "base.mat4"
local geometric = require "base.geometric"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4
local mat4 = types.mat4
local dot = geometric.dot
local cross = geometric.cross
local normalize = geometric.normalize

-- Samples the hemisphere above the surface normal n with a cosine-weighted
-- sampling and returns a random direction (unit) vector within that hemisphere.
local function hemisample(n)
    -- First, sample a unit disc by picking an x value and restricting the
    -- y value to be on the disc.
    local x = 2 * math.random() - 1
    local limit = math.sqrt(1 - x * x)
    local y = (2 * math.random() - 1) * limit

    -- Find the point above the disc to sample the hemisphere with a cosine
    -- weight (Malley's method).
    local z = math.sqrt(math.max(0, 1 - x * x - y * y))

    -- Build an orthonormal basis from the surface normal.
    local w = n

    -- First try using the x-axis to build a basis.
    local t = vec3(1, 0, 0)
    if math.abs(dot(t, w)) > 0.9 then
        -- Too close to being colinear with the x-axis, use the y-axis instead.
        t = vec3(0, 1, 0)
    end
    local u = normalize(cross(t, w))

    -- Complete the basis!
    local v = normalize(cross(w, u))

    -- Build the transformation matrix.
    local m = mat4(vec4(u, 0),
                   vec4(v, 0),
                   vec4(w, 0),
                   vec4(0, 0, 0, 1))

    -- Transform our direction vector into the basis.
    return normalize(vec3(m * vec4(x, y, z, 0)))
end

-- Module exports.
return {
    hemisample = hemisample
}
