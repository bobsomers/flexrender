-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local mat4 = fr.mat4
local radians = fr.radians
local normalize = fr.normalize
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

local width = 10

camera {
    eye = vec3(20, 20, 20),
    look = vec3(0, 0, 0)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(width, 20, width * 2)) * rotate(radians(135), vec3(1, 0, 0)),
    data = fre.plane(10)
}

local tile_r, tile_g, tile_b = fre.targa("scenes/assets/tile1.tga")

function draw_model(model, xform, suffix)
    local color = vec3(math.random() * 0.5 + 0.25,
                       math.random() * 0.5 + 0.25,
                       math.random() * 0.5 + 0.25)
    local data = nil

    if model == 1 then
        data = fre.obj("scenes/assets/bunny-lo.obj", true)
    elseif model == 2 then
        data = fre.obj("scenes/assets/buddha-lo.obj", true)
    else
        data = fre.obj("scenes/assets/dragon-lo.obj", true)
    end

    local mesh_mat = "mesh" .. suffix

    material {
        name = mesh_mat,
        emissive = false,
        shader = fre.phong(0.6, color, 0.2, color, 0.2, 8)
    }

    mesh {
        material = mesh_mat,
        transform = xform,
        data = data
    }
end

function draw_tile(xform, suffix)
    local tile_mat = "mat" .. suffix

    material {
        name = tile_mat,
        emissive = false,
        shader = fre.frsl("scenes/tile_shader.lua"),
        textures = {
            tile_r = tile_r,
            tile_g = tile_g,
            tile_b = tile_b,
        }
    }

    mesh {
        material = tile_mat,
        transform = xform * rotate(radians(-90), vec3(1, 0, 0)),
        data = fre.plane(2)
    }
end

for i = -width, width do
    for j = -width, width do
        math.randomseed(j * width + i)

        local color = vec3(math.random() * 0.5 + 0.25,
                           math.random() * 0.5 + 0.25,
                           math.random() * 0.5 + 0.25)
        local facing = math.random(0, 359)
        local size = 1
        local model = math.random(1, 3)
        if model == 1 then
            size = math.random() * 0.5 + 0.5
        elseif model == 2 then
            size = math.random() * 0.5 + 1
        else
            size = math.random() * 0.25 + 0.5
        end
        local suffix = table.concat {
            "-", tostring(i),
            "-", tostring(j)
        }

        local model_xform = translate(vec3(i * 2, size, j * 2)) * rotate(radians(facing), vec3(0, 1, 0)) * scale(size)
        draw_model(model, model_xform, suffix)

        local tile_xform = translate(vec3(i * 2, 0, j * 2))
        draw_tile(tile_xform, suffix)
    end
end
