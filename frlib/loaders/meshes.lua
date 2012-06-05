-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"
require "base.vec4"

-- Import utility functions.
local transform = require "base.transform"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4
local scale = transform.scale
local translate = transform.translate

-- Cache for OBJ models.
local obj_cache = {}

-- Loads a mesh in OBJ format, caching it for later reloading.
local function obj(filename, adjust)
    if obj_cache[filename] then
        return obj_cache[filename]
    end

    adjust = adjust or false

    local vertices = {}
    local normals = {}
    local texcoords = {}
    local faces = {}

    local f = assert(io.open(filename, "r"))

    local line = f:read("*l")
    while line ~= nil do
        if string.len(line) == 0 or string.sub(line, 1, 1) == "#" then
            -- Ignore, blank line or comment.
        else
            -- Match vertex definitions.
            local v1, v2, v3 = string.match(line, "v%s+([%d%-%.]+)%s+([%d%-%.]+)%s+([%d%-%.]+)")
            if v1 ~= nil and v2 ~= nil and v3 ~= nil then
                table.insert(vertices, vec3(tonumber(v1), tonumber(v2), tonumber(v3)))
            else
                -- Match normal definitions.
                local n1, n2, n3 = string.match(line, "vn%s+([%d%-%.]+)%s+([%d%-%.]+)%s+([%d%-%.]+)")
                if n1 ~= nil and n2 ~= nil and n3 ~= nil then
                    table.insert(normals, vec3(tonumber(n1), tonumber(n2), tonumber(n3)))
                else
                    -- Match texcoord definitions.
                    local t1, t2 = string.match(line, "vt%s+([%d%-%.]+)%s+([%d%-%.]+)")
                    if t1 ~= nil and t2 ~= nil then
                        table.insert(texcoords, vec2(tonumber(t1), tonumber(t2)))
                    else
                        -- Match face definitions.
                        local oneV, oneT, oneN, twoV, twoT, twoN, threeV, threeT, threeN = string.match(line, "f%s+(%d+)/(%d*)/(%d+)%s+(%d+)/(%d*)/(%d+)%s+(%d+)/(%d*)/(%d+)")
                        if oneV ~= nil and oneT ~= nil and oneN ~= nil and twoV ~= nil and twoT ~= nil and twoN ~= nil and threeV ~= nil and threeT ~= nil and threeN ~= nil then
                            if oneT == "" then oneT = "-1" end
                            if twoT == "" then twoT = "-2" end
                            if threeT == "" then threeT = "-3" end
                            table.insert(faces, {
                                {tonumber(oneV), tonumber(oneT), tonumber(oneN)},
                                {tonumber(twoV), tonumber(twoT), tonumber(twoN)},
                                {tonumber(threeV), tonumber(threeT), tonumber(threeN)}
                            })
                        end
                    end
                end
            end
        end

        -- Read next line.
        line = f:read("*l")
    end

    f:close()

    if adjust then
        -- Find the centroid and the min Y.
        local centroid = vec3(0, 0, 0)
        local minY = vertices[1].y
        for _, vertex in ipairs(vertices) do
            if vertex.y < minY then
                minY = vertex.y
            end
            centroid = centroid + vertex
        end
        centroid = centroid / #vertices

        local xform = scale(1 / (centroid.y - minY)) * translate(-centroid)

        local adjusted = {}
        for _, vertex in ipairs(vertices) do
            table.insert(adjusted, vec3(xform * vec4(vertex, 1)))
        end

        vertices = adjusted
    end

    local mesh = function()
        for i, vert in ipairs(vertices) do
            vertex {
                v = vertices[i],
                n = normals[i],
                t = texcoords[i]
            }
        end

        for _, face in ipairs(faces) do
            local v1 = face[1][1] - 1
            local v2 = face[2][1] - 1
            local v3 = face[3][1] - 1

            triangle {v1, v2, v3}
        end
    end

    obj_cache[filename] = mesh
    return mesh
end

-- Module exports.
return {
    obj = obj
}
