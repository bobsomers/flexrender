local function unitbox()
    return function()
        -- Front face.
        flexrender_triangle {{
            vertex = {-0.5, -0.5, 0.5},
            normal = {-1, -1, 1},
            texcoord = {0, 0}
        }, {
            vertex = {-0.5, 0.5, 0.5},
            normal = {-1, 1, 1},
            texcoord = {0, 1}
        }, {
            vertex = {0.5, -0.5, 0.5},
            normal = {1, -1, 1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {0.5, 0.5, 0.5},
            normal = {1, 1, 1},
            texcoord = {1, 1}
        }, {
            vertex = {0.5, -0.5, 0.5},
            normal = {1, -1, 1},
            texcoord = {1, 0}
        }, {
            vertex = {-0.5, 0.5, 0.5},
            normal = {-1, 1, 1},
            texcoord = {0, 1}
        }}

        -- Back face.
        flexrender_triangle {{
            vertex = {0.5, -0.5, -0.5},
            normal = {1, -1, -1},
            texcoord = {0, 0}
        }, {
            vertex = {0.5, 0.5, -0.5},
            normal = {1, 1, -1},
            texcoord = {0, 1}
        }, {
            vertex = {-0.5, -0.5, -0.5},
            normal = {-1, -1, -1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {-0.5, 0.5, -0.5},
            normal = {-1, 1, -1},
            texcoord = {1, 1}
        }, {
            vertex = {-0.5, -0.5, -0.5},
            normal = {-1, -1, -1},
            texcoord = {1, 0}
        }, {
            vertex = {0.5, 0.5, -0.5},
            normal = {1, 1, -1},
            texcoord = {0, 1}
        }}

        -- Left face.
        flexrender_triangle {{
            vertex = {-0.5, -0.5, -0.5},
            normal = {-1, -1, -1},
            texcoord = {0, 0}
        }, {
            vertex = {-0.5, 0.5, -0.5},
            normal = {-1, 1, -1},
            texcoord = {0, 1}
        }, {
            vertex = {-0.5, -0.5, 0.5},
            normal = {-1, -1, 1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {-0.5, 0.5, 0.5},
            normal = {-1, 1, 1},
            texcoord = {1, 1}
        }, {
            vertex = {-0.5, -0.5, 0.5},
            normal = {-1, -1, 1},
            texcoord = {1, 0}
        }, {
            vertex = {-0.5, 0.5, -0.5},
            normal = {-1, 1, -1},
            texcoord = {0, 1}
        }}

        -- Right face.
        flexrender_triangle {{
            vertex = {0.5, -0.5, 0.5},
            normal = {1, -1, 1},
            texcoord = {0, 0}
        }, {
            vertex = {0.5, 0.5, 0.5},
            normal = {1, 1, 1},
            texcoord = {0, 1}
        }, {
            vertex = {0.5, -0.5, -0.5},
            normal = {1, -1, -1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {0.5, 0.5, -0.5},
            normal = {1, 1, -1},
            texcoord = {1, 1}
        }, {
            vertex = {0.5, -0.5, -0.5},
            normal = {1, -1, -1},
            texcoord = {1, 0}
        }, {
            vertex = {0.5, 0.5, 0.5},
            normal = {1, 1, 1},
            texcoord = {0, 1}
        }}

        -- Top face.
        flexrender_triangle {{
            vertex = {-0.5, 0.5, 0.5},
            normal = {-1, 1, 1},
            texcoord = {0, 0}
        }, {
            vertex = {-0.5, 0.5, -0.5},
            normal = {-1, 1, -1},
            texcoord = {0, 1}
        }, {
            vertex = {0.5, 0.5, 0.5},
            normal = {1, 1, 1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {0.5, 0.5, -0.5},
            normal = {1, 1, -1},
            texcoord = {1, 1}
        }, {
            vertex = {0.5, 0.5, 0.5},
            normal = {1, 1, 1},
            texcoord = {1, 0}
        }, {
            vertex = {-0.5, 0.5, -0.5},
            normal = {-1, 1, -1},
            texcoord = {0, 1}
        }}

        -- Bottom face.
        flexrender_triangle {{
            vertex = {-0.5, -0.5, -0.5},
            normal = {-1, -1, -1},
            texcoord = {0, 0}
        }, {
            vertex = {-0.5, -0.5, 0.5},
            normal = {-1, -1, 1},
            texcoord = {0, 1}
        }, {
            vertex = {0.5, -0.5, -0.5},
            normal = {1, -1, -1},
            texcoord = {1, 0}
        }}
        flexrender_triangle {{
            vertex = {0.5, -0.5, 0.5},
            normal = {1, -1, 1},
            texcoord = {1, 1}
        }, {
            vertex = {0.5, -0.5, -0.5},
            normal = {1, -1, -1},
            texcoord = {1, 0}
        }, {
            vertex = {-0.5, -0.5, 0.5},
            normal = {-1, -1, 1},
            texcoord = {0, 1}
        }}
    end
end

return unitbox
