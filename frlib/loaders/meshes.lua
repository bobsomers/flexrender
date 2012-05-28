-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3

-- Cache for OBJ models.
local obj_cache = {}

-- Loads a mesh in OBJ format, caching it for later reloading.
local function obj(filename, scale)
    if obj_cache[filename] then
        return obj_cache[filename]
    end

    scale = scale or false

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

    local mesh = function()
        for _, face in ipairs(faces) do
            local v1 = vertices[face[1][1]]
            local v2 = vertices[face[2][1]]
            local v3 = vertices[face[3][1]]
            local n1 = normals[face[1][3]]
            local n2 = normals[face[2][3]]
            local n3 = normals[face[3][3]]
            local t1 = face[1][2]
            if t1 < 0 then t1 = vec2(0, 0) else t1 = texcoords[t1] end
            local t2 = face[2][2]
            if t2 < 0 then t2 = vec2(0, 1) else t2 = texcoords[t2] end
            local t3 = face[3][2]
            if t3 < 0 then t3 = vec2(1, 0) else t3 = texcoords[t3] end

            triangle {
                {v = v1, n = n1, t = t1},
                {v = v2, n = n2, t = t2},
                {v = v3, n = n3, t = t3}
            }
        end
    end

    obj_cache[filename] = mesh
    return mesh
end

-- Module exports.
return {
    obj = obj
}
