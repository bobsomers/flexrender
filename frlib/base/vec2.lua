-- Import relevant base types.
local types = require "base.types"
require "base.common"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4

-- Forward index getter to common type.
vec2.__index = types.common.__index

-- Forward index setter to common type.
vec2.__newindex = types.common.__newindex

-- Getters.
vec2.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    u = function(v) return v[1] end,
    v = function(v) return v[2] end
}

-- Setters.
vec2.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    u = function(v, val) v[1] = val end,
    v = function(v, val) v[2] = val end
}

-- Constructor that provides the following forms:
--  vec2(x) -> <x, x>                   (x is a number)
--  vec2(v) -> <v.x, v.y>               (v is a vec2/vec3/vec4)
--  vec2(x, y) -> <x, y>                (x, y are numbers)
function vec2.new(...)
    local v = nil

    local nargs = select("#", ...)
    if nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            v = {arg, arg}
        elseif argmt == vec2 or argmt == vec3 or argmt == vec4 then
            v = {arg[1], arg[2]}
        else
            error("invalid construction of vec2", 3)
        end
    elseif nargs == 2 then
        local args = {select(1, ...), select(2, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" then
            error("invalid construction of vec2", 3)
        end
        v = {args[1], args[2]}
    else
        error("invalid construction of vec2", 3)
    end

    setmetatable(v, vec2)
    return v
end

-- Fast constructor for internal use only.
function vec2.frnew(x, y)
    local v = {x, y}
    setmetatable(v, vec2)
    return v
end

-- Attempts to coerce val to a vec2 by construction.
function vec2.frcoerce(val)
    if getmetatable(val) == vec2 then
        return val
    end

    local result, v = pcall(vec2.new, val)
    if result then
        return v
    end

    error("no known conversion to vec2", 3)
end

-- Converts the vector to a string representation.
function vec2.__tostring(v)
    return table.concat {"vec2<", v[1], ", ", v[2], ">"}
end

-- Adds two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec2.
function vec2.__add(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] + b[1], a[2] + b[2])
end

-- Subtracts two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec2.
function vec2.__sub(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] - b[1], a[2] - b[2])
end

-- Multiplies two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec2.
function vec2.__mul(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] * b[1], a[2] * b[2])
end

-- Divides two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec2.
function vec2.__div(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] / b[1], a[2] / b[2])
end

-- Negates a vector component-wise.
-- Always produces a vec2.
function vec2.__unm(v)
    return vec2.frnew(-v[1], -v[2])
end

-- Module exports.
return nil
