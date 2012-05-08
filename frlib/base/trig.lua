-- Convert degrees to radians.
local function radians(d)
    return math.pi / 180 * d
end

-- Convert radians to degrees.
local function degrees(r)
    return 180 / math.pi * r
end

-- Module exports.
return {
    radians = radians,
    degrees = degrees
}
