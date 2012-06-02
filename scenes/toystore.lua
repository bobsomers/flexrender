-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local mat4 = fr.mat4
local radians = fr.radians
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

camera {
    eye = vec3(50, 30, 50),
    look = vec3(0, 0, 0)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(0, 50, 0)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(1)
}

material {
    name = "red",
    emissive = false,
    shader = fre.phong(0.6, vec3(1, 0, 0),
                       0.2, vec3(1, 0, 0),
                       0.2, 8)
}

local tile_r, tile_g, tile_b = fre.targa("scenes/assets/tile1.tga")

function draw_floor_tile(i, j, y)
    local mat = table.concat {"floor-", y, "-", i, "-", j}

    material {
        name = mat,
        emissive = false,
        shader = fre.frsl("scenes/tile_shader.lua"),
        textures = {
            tile_r = tile_r,
            tile_g = tile_g,
            tile_b = tile_b,
        }
    }

    mesh {
        material = mat,
        transform = translate(vec3(i, y, j)) * rotate(radians(-90), vec3(1, 0, 0)),
        data = fre.plane(1)
    }
end

function draw_stair(n, y)
    local mat = table.concat {"stair-", y, "-", n}

    material {
        name = mat,
        emissive = false,
        shader = fre.phong(0.6, vec3(1, 0, 0), 0.2, vec3(1, 0, 0), 0.2, 8)
    }

    mesh {
        material = mat,
        transform = translate(vec3(27.5, y + n - 0.5, 21 - n)) * scale(vec3(5, 1, 1)),
        data = fre.cube(1)
    }
end

-- ==== STAIRS ====

for i = 1, 10 do
    draw_stair(i, 0)
end

-- ==== FLOORS ====

-- Second floor.
for i = 0, 15 do
    for j = 0, 30 do
        draw_floor_tile(i, j, 10)
    end
end
for i = 16, 30 do
    for j = 0, 10 do
        draw_floor_tile(i, j, 10)
    end
end

-- First floor.
for i = 0, 30 do
    for j = 0, 30 do
        draw_floor_tile(i, j, 0)
    end
end
