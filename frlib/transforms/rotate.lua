local function rotate(angle, about)
    return {
        kind = "rotate",
        scalar = angle,
        vector = about
    }
end

return rotate
