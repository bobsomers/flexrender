-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"

-- Import utility functions.
local geometric = require "base.geometric"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3

-- Creates a function that returns geometry for a square in the xy-plane with
-- sides of length n centered at the origin.
local function plane(n)
    return function()
        local h = n / 2
        
        vertex {
            v = vec3(-h, -h, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(-h, h, 0),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(h, -h, 0),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(h, h, 0),
            n = vec3(0, 0, 1),
            t = vec2(1, 1)
        }

        triangle {0, 1, 2}
        triangle {3, 2, 1}
    end
end

-- Creates a function that returns geometry for a cube with sides of length n
-- centered at the origin.
local function cube(n)
    return function()
        local h = n / 2

        -- Front face.
        vertex {
            v = vec3(-h, -h, h),
            n = vec3(0, 0, 1),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(-h, h, h),
            n = vec3(0, 0, 1),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(h, -h, h),
            n = vec3(0, 0, 1),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(h, h, h),
            n = vec3(0, 0, 1),
            t = vec2(1, 1)
        }
        triangle {0, 1, 2}
        triangle {3, 2, 1}

        -- Back face.
        vertex {
            v = vec3(h, -h, -h),
            n = vec3(0, 0, -1),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(h, h, -h),
            n = vec3(0, 0, -1),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(-h, -h, -h),
            n = vec3(0, 0, -1),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(-h, h, -h),
            n = vec3(0, 0, -1),
            t = vec2(1, 1)
        }
        triangle {4, 5, 6}
        triangle {7, 6, 5}

        -- Left face.
        vertex {
            v = vec3(-h, -h, -h),
            n = vec3(-1, 0, 0),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(-h, h, -h),
            n = vec3(-1, 0, 0),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(-h, -h, h),
            n = vec3(-1, 0, 0),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(-h, h, h),
            n = vec3(-1, 0, 0),
            t = vec2(1, 1)
        }
        triangle {8, 9, 10}
        triangle {11, 10, 9}

        -- Right face.
        vertex {
            v = vec3(h, -h, h),
            n = vec3(1, 0, 0),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(h, h, h),
            n = vec3(1, 0, 0),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(h, -h, -h),
            n = vec3(1, 0, 0),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(h, h, -h),
            n = vec3(1, 0, 0),
            t = vec2(1, 1)
        }
        triangle {12, 13, 14}
        triangle {15, 14, 13}

        -- Top face.
        vertex {
            v = vec3(-h, h, h),
            n = vec3(0, 1, 0),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(-h, h, -h),
            n = vec3(0, 1, 0),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(h, h, h),
            n = vec3(0, 1, 0),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(h, h, -h),
            n = vec3(0, 1, 0),
            t = vec2(1, 1)
        }
        triangle {16, 17, 18}
        triangle {19, 18, 17}

        -- Bottom face.
        vertex {
            v = vec3(-h, -h, -h),
            n = vec3(0, -1, 0),
            t = vec2(0, 0)
        }
        vertex {
            v = vec3(-h, -h, h),
            n = vec3(0, -1, 0),
            t = vec2(0, 1)
        }
        vertex {
            v = vec3(h, -h, -h),
            n = vec3(0, -1, 0),
            t = vec2(1, 0)
        }
        vertex {
            v = vec3(h, -h, h),
            n = vec3(0, -1, 0),
            t = vec2(1, 1)
        }
        triangle {20, 21, 22}
        triangle {23, 22, 21}
    
    end
end

-- Module exports.
return {
    plane = plane,
    cube = cube
}
