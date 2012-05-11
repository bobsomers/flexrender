-- Needs lpack to unpack binary data.
require "pack"

-- Import relevant base types.
local types = require "base.types"
require "base.vec2"
require "base.vec3"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3

-- Loads a procedural texture source file into a FlexRender texture.
-- Returns the resource ID of the texture.
local function procedural(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return texture {
        kind = "procedural",
        code = procedure
    }
end

-- Loads an uncompressed Targa image, assuming origin at bottom-left.
local function targa(filename)
    local f = assert(io.open(filename, "rb"))

    local _ = nil
    local kind = nil
    local width = nil
    local height = nil
    local depth = nil
    local red = nil
    local green = nil
    local blue = nil
    local rtex = {}
    local gtex = {}
    local btex = {}

    -- Unpack the header data.
    _ = assert(f:read(2))
    _, kind = string.unpack(assert(f:read(1)), "b")
    _ = assert(f:read(9))
    _, width = string.unpack(assert(f:read(2)), "<H")
    _, height = string.unpack(assert(f:read(2)), "<H")
    _, depth = string.unpack(assert(f:read(1)), "b")
    _ = assert(f:read(1))

    -- Verify that we know how to load the image data.
    if kind ~= 2 or depth ~= 24 then
        error("Expected uncompressed Targa with 24-bit color.", 2)
    end

    -- Read in the image data.
    for y = 0, height - 1 do
        for x = 0, width - 1 do
            _, blue, green, red = string.unpack(assert(f:read(3)), "bbb")
            local index = y * width + x
            rtex[index] = red / 255
            gtex[index] = green / 255
            btex[index] = blue / 255
        end
    end

    f:close()

    rtex.kind = "image"
    gtex.kind = "image"
    btex.kind = "image"

    rtex.size = vec2(width, height)
    gtex.size = vec2(width, height)
    btex.size = vec2(width, height)

    return texture(rtex), texture(gtex), texture(btex)
end

-- Module exports.
return {
    procedural = procedural,
    targa = targa
}
