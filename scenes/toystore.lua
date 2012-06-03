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

camera {
    eye = vec3(50, 25, 50),
    look = vec3(0, 0, 0)
}

--material {
--    name = "light",
--    emissive = true,
--    shader = fre.light(vec3(1/9, 1/9, 1/9))
--}
--
--mesh {
--    material = "light",
--    transform = translate(vec3(0, 50, 50)) * rotate(radians(135), vec3(1, 0, 0)),
--    data = fre.plane(1)
--}

material {
    name = "red",
    emissive = false,
    shader = fre.phong(0.6, vec3(1, 0, 0),
                       0.2, vec3(1, 0, 0),
                       0.2, 8)
}

function place_light(x, y, floor)
    local mat = next_mat()

    material {
        name = mat,
        emissive = true,
        shader = fre.light(vec3(1/9, 1/9, 1/9))
    }

    mesh {
        material = mat,
        transform = translate(vec3(x, 10 * floor - 0.25, y)) * rotate(radians(90), vec3(1, 0, 0)) * scale(vec3(1, 5, 1)),
        data = fre.plane(1)
    }
end

-- Lights.
place_light(7, 8, 1)
place_light(7, 16, 1)
place_light(7, 24, 1)
place_light(15, 9, 1)
place_light(15, 17, 1)
place_light(15, 25, 1)
place_light(23, 8, 1)
place_light(23, 16, 1)
place_light(23, 24, 1)

local tile_r, tile_g, tile_b = fre.targa("scenes/assets/tile1.tga")
local wood_r, wood_g, wood_b = fre.targa("scenes/assets/wood1.tga")
local marble_r, marble_g, marble_b = fre.targa("scenes/assets/marble1.tga")

function draw_floor_tile(i, j, y)
    local mat = next_mat()

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
        transform = translate(vec3(3 * i + 1.5, y, 3 * j + 1.5)) * rotate(radians(-90), vec3(1, 0, 0)),
        data = fre.plane(3)
    }
end

function draw_stair(n, y)
    local mat = next_mat()

    material {
        name = mat,
        emissive = false,
        shader = fre.frsl("scenes/wood_shader.lua"),
        textures = {
            wood_r = marble_r,
            wood_g = marble_g,
            wood_b = marble_b,
        }
    }

    mesh {
        material = mat,
        transform = translate(vec3(27.5, y + 0.5 * n - 0.25, 21 - 0.5 * n - 0.25)) * scale(vec3(4, 0.5, 0.5)),
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
        transform = xform * translate(vec3(i - 2, shelf * 2 + 0.5 + size, j - 0.5)) * rotate(facing, vec3(0, 1, 0)) * scale(size),
        data = data
    }
end

function draw_shelf(xform)
    local mat = next_mat()

    material {
        name = mat,
        emissive = false,
        shader = fre.frsl("scenes/wood_shader.lua"),
        textures = {
            wood_r = wood_r,
            wood_g = wood_g,
            wood_b = wood_b,
        }
    }

    -- Bottom shelf.
    mesh {
        material = mat,
        transform = xform * translate(vec3(0, 0.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Middle shelf.
    mesh {
        material = mat,
        transform = xform * translate(vec3(0, 2.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Top shelf.
    mesh {
        material = mat,
        transform = xform * translate(vec3(0, 4.5, 0)) * rotate(radians(-90), vec3(1, 0, 0)) * scale(vec3(5, 2, 2)),
        data = fre.plane(1)
    }

    -- Posts.
    for i = -1, 1, 2 do
        for j = -1, 1, 2 do
            mesh {
                material = mat,
                transform = xform * translate(vec3(i * 2.5, 2.5, j)) * scale(vec3(0.25, 5, 0.25)),
                data = fre.cube(1)
            }
        end
    end

    -- Draw toys.
    --[[
    for i = 0, 4 do
        for j = 0, 1 do
            for shelf = 0, 2 do
                draw_toy(xform, i, j, shelf)
            end
        end
    end
    --]]
end

-- ==== FIRST FLOOR ====

-- Floor.
for i = 0, 9 do
    for j = 0, 10 do
        draw_floor_tile(i, j, 0)
    end
end

-- Shelves.
for j = 0, 6 do
    draw_shelf(translate(vec3(3, 0, j * 4 + 4)))
end
for j = 0, 6 do
    draw_shelf(translate(vec3(11, 0, j * 4 + 3)))
end
for j = 0, 6 do
    draw_shelf(translate(vec3(19, 0, j * 4 + 4)))
end
for j = 0, 1 do
    draw_shelf(translate(vec3(27, 0, j * 4 + 3)))
end

-- ==== STAIRS ====

for i = 1, 20 do
    draw_stair(i, 0)
end

-- ==== FLOORS ====

-- Second floor.
--for i = 0, 15 do
--    for j = 0, 30 do
--        draw_floor_tile(i, j, 10)
--    end
--end
--for i = 16, 30 do
--    for j = 0, 10 do
--        draw_floor_tile(i, j, 10)
--    end
--end
