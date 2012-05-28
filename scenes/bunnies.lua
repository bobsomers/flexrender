-- Import frlib and extras.
package.cpath = "3p/build/lib/?.so;" .. package.cpath
package.path = "frlib/?.lua;" .. package.path
local fr = require "flexrender"
local fre = require "extras"

-- Handy aliases.
local vec3 = fr.vec3
local radians = fr.radians
local normalize = fr.normalize
local scale = fr.scale
local rotate = fr.rotate
local translate = fr.translate

local width = 10
local height = 10

camera {
    eye = vec3(17, 8, 20),
    look = vec3(-5, 0, -5)
}

material {
    name = "light",
    emissive = true,
    shader = fre.light(vec3(1, 1, 1))
}

mesh {
    material = "light",
    transform = translate(vec3(-width * 2, 15, height * 2)) * rotate(radians(90), vec3(1, 0, 0)),
    data = fre.plane(10)
}

local bg_r, bg_g, bg_b = fre.targa("scenes/assets/bg1.tga")

material {
    name = "backdrop",
    emissive = false,
    shader = fre.frsl("scenes/backdrop_shader.lua"),
    textures = {
        bg_r = bg_r,
        bg_g = bg_g,
        bg_b = bg_b
    }
}

mesh {
    material = "backdrop",
    transform = translate(vec3(-width * 2, 10, -height * 2)) * rotate(radians(42.5), vec3(0, 1, 0)) * scale(vec3(16, 9, 1)),
    data = fre.plane(7)
}

local tile_r, tile_g, tile_b = fre.targa("scenes/assets/tile1.tga")

for i = -width, width do
    for j = -height, height do
        math.randomseed(j * width + i)

        local mat_suffix = table.concat {
            "-", tostring(i),
            "-", tostring(j)
        }

        local color = vec3(math.random() * 0.5 + 0.25, math.random() * 0.5 + 0.25, math.random() * 0.5 + 0.25)
        local facing = math.random(0, 359)
        local size = 1
        local data = nil

        local model = math.random(1, 3)
        if model == 1 then
            size = math.random() * 0.5 + 0.5
            data = fre.obj("scenes/assets/bunny-lo.obj", true)
        elseif model == 2 then
            size = math.random() * 0.5 + 1
            data = fre.obj("scenes/assets/buddha-lo.obj", true)
        else
            size = math.random() * 0.5 + 0.25
            data = fre.obj("scenes/assets/dragon-lo.obj", true)
        end

        local mesh_mat = "mesh" .. mat_suffix
        local tile_mat = "tile" .. mat_suffix

        material {
            name = mesh_mat,
            emissive = false,
            shader = fre.phong(0.6, color, 0.2, color, 0.2, 8)
        }

        mesh {
            material = mesh_mat,
            transform = translate(vec3(i * 2, size, j * 2)) * rotate(radians(facing), vec3(0, 1, 0)) * scale(size),
            data = data
        }

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
            transform = translate(vec3(i * 2, 0, j * 2)) * rotate(radians(-90), vec3(1, 0, 0)),
            data = fre.plane(2)
        }
    end
end
