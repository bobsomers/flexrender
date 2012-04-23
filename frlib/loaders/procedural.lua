local function procedural(filename)
    local f = assert(io.open(filename, "r"))
    local procedure = f:read("*a")
    f:close()

    return texture {
        kind = "procedural",
        code = procedure
    }
end

return procedural
