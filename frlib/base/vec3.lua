-- Import relevant base types.
local types = require "base.types"
require "base.common"

-- Module aliases.
local vec2 = types.vec2
local vec3 = types.vec3
local vec4 = types.vec4

-- Forward index getter to common type.
vec3.__index = types.common.__index

-- Forward index setter to common type.
vec3.__newindex = types.common.__newindex

-- Getters.
vec3.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    z = function(v) return v[3] end,
    r = function(v) return v[1] end,
    g = function(v) return v[2] end,
    b = function(v) return v[3] end
}

-- Setters.
vec3.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    z = function(v, val) v[3] = val end,
    r = function(v, val) v[1] = val end,
    g = function(v, val) v[2] = val end,
    b = function(v, val) v[3] = val end
}

-- Constructor that provides the following forms:
--  vec3(x) -> <x, x, x>                (x is a number)
--  vec3(v) -> <v.x, v.y, v.z>          (v is a vec3/vec4)
--  vec3(v, z) -> <v.x, v.y, z>         (v is a vec2, z is a number)
--  vec3(x, y, z) -> <x, y, z>          (x, y, z are numbers)
function vec3.new(...)
    local v = nil

    local nargs = select("#", ...)
    if nargs == 1 then
        local arg = select(1, ...)
        local argmt = getmetatable(arg)
        if type(arg) == "number" then
            v = {arg, arg, arg}
        elseif argmt == vec3 or argmt == vec4 then
            v = {arg[1], arg[2], arg[3]}
        else
            error("invalid construction of vec3", 3)
        end
    elseif nargs == 2 then
        local args = {select(1, ...), select(2, ...)}
        local arg1mt = getmetatable(args[1])
        if arg1mt ~= vec2 or type(args[2]) ~= "number" then
            error("invalid construction of vec3", 3)
        end
        local v2 = args[1]
        v = {v2[1], v2[2], args[2]}
    elseif nargs == 3 then
        local args = {select(1, ...), select(2, ...), select(3, ...)}
        if type(args[1]) ~= "number" or type(args[2]) ~= "number" or type(args[3]) ~= "number" then
            error("invalid construction of vec3", 3)
        end
        v = {args[1], args[2], args[3]}
    else
        error("invalid construction of vec3", 3)
    end

    setmetatable(v, vec3)
    return v
end

-- Fast constructor for internal use only.
function vec3.frnew(x, y, z)
    local v = {x, y, z}
    setmetatable(v, vec3)
    return v
end

-- Attempts to coerce val to a vec3 by construction.
function vec3.frcoerce(val)
    if getmetatable(val) == vec3 then
        return val
    end

    local result, v = pcall(vec3.new, val)
    if result then
        return v
    end

    error("no known conversion to vec3", 3)
end

-- Converts the vector to a string representation.
function vec3.__tostring(v)
    return table.concat {"vec3<", v[1], ", ", v[2], ", ", v[3], ">"}
end

-- Adds two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec3.
function vec3.__add(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3])
end

-- Subtracts two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec3.
function vec3.__sub(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3])
end

-- Multiplies two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec3.
function vec3.__mul(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3])
end

-- Divides two vectors component-wise, coercing the arguments as necessary.
-- Always produces a vec3.
function vec3.__div(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3])
end

-- Negates a vector component-wise.
-- Always produces a vec3.
function vec3.__unm(v)
    return vec3.frnew(-v[1], -v[2], -v[3])
end

-- Module exports.
return nil
