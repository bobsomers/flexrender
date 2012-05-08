-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"

-- Import utility functions.
local geometric = require "base.geometric"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3

-- Creates a function that returns geometry for a cube with sides of length n
-- centered at the origin.
local function cube(n)
    return function()
        local h = n / 2

        -- Front face.
        triangle {{
            v = vec3(-h, -h, h),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }, {
            v = vec3(-h, h, h),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }, {
            v = vec3(h, -h, h),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(h, h, h),
            n = vec3(0, 0, 1),
            t = vec2(1, 1)
        }, {
            v = vec3(h, -h, h),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }, {
            v = vec3(-h, h, h),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }}

        -- Back face.
        triangle {{
            v = vec3(h, -h, -h),
            n = vec3(0, 0, -1),
            t = vec2(0, 0)
        }, {
            v = vec3(h, h, -h),
            n = vec3(0, 0, -1),
            t = vec2(0, 1)
        }, {
            v = vec3(-h, -h, -h),
            n = vec3(0, 0, -1),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(-h, h, -h),
            n = vec3(0, 0, -1),
            t = vec2(1, 1)
        }, {
            v = vec3(-h, -h, -h),
            n = vec3(0, 0, -1),
            t = vec2(1, 0)
        }, {
            v = vec3(h, h, -h),
            n = vec3(0, 0, -1),
            t = vec2(0, 1)
        }}

        -- Left face.
        triangle {{
            v = vec3(-h, -h, -h),
            n = vec3(-1, 0, 0),
            t = vec2(0, 0)
        }, {
            v = vec3(-h, h, -h),
            n = vec3(-1, 0, 0),
            t = vec2(0, 1)
        }, {
            v = vec3(-h, -h, h),
            n = vec3(-1, 0, 0),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(-h, h, h),
            n = vec3(-1, 0, 0),
            t = vec2(1, 1)
        }, {
            v = vec3(-h, -h, h),
            n = vec3(-1, 0, 0),
            t = vec2(1, 0)
        }, {
            v = vec3(-h, h, -h),
            n = vec3(-1, 0, 0),
            t = vec2(0, 1)
        }}

        -- Right face.
        triangle {{
            v = vec3(h, -h, h),
            n = vec3(1, 0, 0),
            t = vec2(0, 0)
        }, {
            v = vec3(h, h, h),
            n = vec3(1, 0, 0),
            t = vec2(0, 1)
        }, {
            v = vec3(h, -h, -h),
            n = vec3(1, 0, 0),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(h, h, -h),
            n = vec3(1, 0, 0),
            t = vec2(1, 1)
        }, {
            v = vec3(h, -h, -h),
            n = vec3(1, 0, 0),
            t = vec2(1, 0)
        }, {
            v = vec3(h, h, h),
            n = vec3(1, 0, 0),
            t = vec2(0, 1)
        }}

        -- Top face.
        triangle {{
            v = vec3(-h, h, h),
            n = vec3(0, 1, 0),
            t = vec2(0, 0)
        }, {
            v = vec3(-h, h, -h),
            n = vec3(0, 1, 0),
            t = vec2(0, 1)
        }, {
            v = vec3(h, h, h),
            n = vec3(0, 1, 0),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(h, h, -h),
            n = vec3(0, 1, 0),
            t = vec2(1, 1)
        }, {
            v = vec3(h, h, h),
            n = vec3(0, 1, 0),
            t = vec2(1, 0)
        }, {
            v = vec3(-h, h, -h),
            n = vec3(0, 1, 0),
            t = vec2(0, 1)
        }}

        -- Bottom face.
        triangle {{
            v = vec3(-h, -h, -h),
            n = vec3(0, -1, 0),
            t = vec2(0, 0)
        }, {
            v = vec3(-h, -h, h),
            n = vec3(0, -1, 0),
            t = vec2(0, 1)
        }, {
            v = vec3(h, -h, -h),
            n = vec3(0, -1, 0),
            t = vec2(1, 0)
        }}
        triangle {{
            v = vec3(h, -h, h),
            n = vec3(0, -1, 0),
            t = vec2(1, 1)
        }, {
            v = vec3(h, -h, -h),
            n = vec3(0, -1, 0),
            t = vec2(1, 0)
        }, {
            v = vec3(-h, -h, h),
            n = vec3(0, -1, 0),
            t = vec2(0, 1)
        }}
    end
end

-- Module exports.
return {
    cube = cube
}
