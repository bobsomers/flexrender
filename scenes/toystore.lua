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

local mat_unique = 0
function next_mat()
    mat_unique = mat_unique + 1
    return "mat-" .. mat_unique
end

math.randomseed(42)

--[[
camera {
    eye = vec3(50, 30, 50),
    look = vec3(0, 0, 0)
}
]]

camera {
    eye = vec3(7, 7, 7),
    look = vec3(0, 2, 0)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(0, 50, 50)) * rotate(radians(135), vec3(1, 0, 0)),
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

function draw_toy(xform, i, j, shelf)
    local color = vec3(math.random() * 0.5 + 0.25,
                       math.random() * 0.5 + 0.25,
                       math.random() * 0.5 + 0.25)
    local shader = fre.phong(0.6, color, 0.2, color, 0.2, 8)
    local mat = next_mat()

    local size = 1
    local data = nil
    local facing = radians(math.random(0, 359))
    local model = math.random(1, 3)
    if model == 1 then
        data = fre.obj("scenes/assets/bunny-lo.obj", true)
        size = 0.3
    elseif model == 2 then
        data = fre.obj("scenes/assets/buddha-lo.obj", true)
        size = 0.6
    else
        data = fre.obj("scenes/assets/dragon-lo.obj", true)
        size = 0.3
    end

    material {
        name = mat,
        emissive = false,
        shader = shader
    }

    mesh {
        material = mat,
        transform = translate(vec3(i - 2, shelf * 2 + 0.5 + size, j - 0.5)) * rotate(facing, vec3(0, 1, 0)) * scale(size),
        data = data
    }
end

function draw_shelf()
    -- Bottom shelf.
    mesh {
        material = "red",
        transform = translate(vec3(0, 0.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Middle shelf.
    mesh {
        material = "red",
        transform = translate(vec3(0, 2.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Top shelf.
    mesh {
        material = "red",
        transform = translate(vec3(0, 4.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Posts.
    for i = -1, 1, 2 do
        for j = -1, 1, 2 do
            mesh {
                material = "red",
                transform = translate(vec3(i * 2.5, 2.5, j)) * scale(vec3(0.25, 5, 0.25)),
                data = fre.cube(1)
            }
        end
    end
end

draw_shelf()
for i = 0, 4 do
    for j = 0, 1 do
        for shelf = 0, 2 do
            draw_toy(mat4(), i, j, shelf)
        end
    end
end

--[[

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

]]
