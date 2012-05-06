-- Creates a cube with sides of length n centered at the origin.
local function cube(n)
    return function()
        local half_n = n / 2

        -- Front face.
        triangle {{
            v = {-half_n, -half_n, half_n},
            n = {-1, -1, 1},
            t = {0, 0}
        }, {
            v = {-half_n, half_n, half_n},
            n = {-1, 1, 1},
            t = {0, 1}
        }, {
            v = {half_n, -half_n, half_n},
            n = {1, -1, 1},
            t = {1, 0}
        }}
        triangle {{
            v = {half_n, half_n, half_n},
            n = {1, 1, 1},
            t = {1, 1}
        }, {
            v = {half_n, -half_n, half_n},
            n = {1, -1, 1},
            t = {1, 0}
        }, {
            v = {-half_n, half_n, half_n},
            n = {-1, 1, 1},
            t = {0, 1}
        }}

        -- Back face.
        triangle {{
            v = {half_n, -half_n, -half_n},
            n = {1, -1, -1},
            t = {0, 0}
        }, {
            v = {half_n, half_n, -half_n},
            n = {1, 1, -1},
            t = {0, 1}
        }, {
            v = {-half_n, -half_n, -half_n},
            n = {-1, -1, -1},
            t = {1, 0}
        }}
        triangle {{
            v = {-half_n, half_n, -half_n},
            n = {-1, 1, -1},
            t = {1, 1}
        }, {
            v = {-half_n, -half_n, -half_n},
            n = {-1, -1, -1},
            t = {1, 0}
        }, {
            v = {half_n, half_n, -half_n},
            n = {1, 1, -1},
            t = {0, 1}
        }}

        -- Left face.
        triangle {{
            v = {-half_n, -half_n, -half_n},
            n = {-1, -1, -1},
            t = {0, 0}
        }, {
            v = {-half_n, half_n, -half_n},
            n = {-1, 1, -1},
            t = {0, 1}
        }, {
            v = {-half_n, -half_n, half_n},
            n = {-1, -1, 1},
            t = {1, 0}
        }}
        triangle {{
            v = {-half_n, half_n, half_n},
            n = {-1, 1, 1},
            t = {1, 1}
        }, {
            v = {-half_n, -half_n, half_n},
            n = {-1, -1, 1},
            t = {1, 0}
        }, {
            v = {-half_n, half_n, -half_n},
            n = {-1, 1, -1},
            t = {0, 1}
        }}

        -- Right face.
        triangle {{
            v = {half_n, -half_n, half_n},
            n = {1, -1, 1},
            t = {0, 0}
        }, {
            v = {half_n, half_n, half_n},
            n = {1, 1, 1},
            t = {0, 1}
        }, {
            v = {half_n, -half_n, -half_n},
            n = {1, -1, -1},
            t = {1, 0}
        }}
        triangle {{
            v = {half_n, half_n, -half_n},
            n = {1, 1, -1},
            t = {1, 1}
        }, {
            v = {half_n, -half_n, -half_n},
            n = {1, -1, -1},
            t = {1, 0}
        }, {
            v = {half_n, half_n, half_n},
            n = {1, 1, 1},
            t = {0, 1}
        }}

        -- Top face.
        triangle {{
            v = {-half_n, half_n, half_n},
            n = {-1, 1, 1},
            t = {0, 0}
        }, {
            v = {-half_n, half_n, -half_n},
            n = {-1, 1, -1},
            t = {0, 1}
        }, {
            v = {half_n, half_n, half_n},
            n = {1, 1, 1},
            t = {1, 0}
        }}
        triangle {{
            v = {half_n, half_n, -half_n},
            n = {1, 1, -1},
            t = {1, 1}
        }, {
            v = {half_n, half_n, half_n},
            n = {1, 1, 1},
            t = {1, 0}
        }, {
            v = {-half_n, half_n, -half_n},
            n = {-1, 1, -1},
            t = {0, 1}
        }}

        -- Bottom face.
        triangle {{
            v = {-half_n, -half_n, -half_n},
            n = {-1, -1, -1},
            t = {0, 0}
        }, {
            v = {-half_n, -half_n, half_n},
            n = {-1, -1, 1},
            t = {0, 1}
        }, {
            v = {half_n, -half_n, -half_n},
            n = {1, -1, -1},
            t = {1, 0}
        }}
        triangle {{
            v = {half_n, -half_n, half_n},
            n = {1, -1, 1},
            t = {1, 1}
        }, {
            v = {half_n, -half_n, -half_n},
            n = {1, -1, -1},
            t = {1, 0}
        }, {
            v = {-half_n, -half_n, half_n},
            n = {-1, -1, 1},
            t = {0, 1}
        }}
    end
end

-- Module exports.
return {
    cube = cube
}
