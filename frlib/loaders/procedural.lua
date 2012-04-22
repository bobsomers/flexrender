local function procedural(filename)
    return function()
        local f = assert(io.open(filename, "r"))
        local procedure = f:read("*a")
        f:close()

        flexrender_texture {
            name = filename,
            kind = "procedural",
            code = procedure
        }

        return filename
    end
end

return procedural
