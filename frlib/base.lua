local common = {}
local vec2 = {}
local vec3 = {}
local vec4 = {}

-- Common forwarding methods for all base types.
function common.__call(parent, ...)
    local obj = parent.new(...)
    setmetatable(obj, parent)
    return obj
end

function common.__index(table, key)
    local mt = getmetatable(table)
    local func = mt.getters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    return func(table)
end

function common.__newindex(table, key, value)
    local mt = getmetatable(table)
    local func = mt.setters[key]
    if func == nil then
        error("no member named '" .. key .. "'", 2)
    end
    func(table, value)
end

-- 2D Vector
vec2.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    u = function(v) return v[1] end,
    v = function(v) return v[2] end,
}

vec2.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    u = function(v, val) v[1] = val end,
    v = function(v, val) v[2] = val end,
}

vec2.__index = common.__index
vec2.__newindex = common.__newindex

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

function vec2.frnew(x, y)
    local v = {x, y}
    setmetatable(v, vec2)
    return v
end

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

function vec2.__tostring(v)
    return table.concat {"vec2<", v[1], ", ", v[2], ">"}
end

function vec2.__add(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] + b[1], a[2] + b[2])
end

function vec2.__sub(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] - b[1], a[2] - b[2])
end

function vec2.__mul(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] * b[1], a[2] * b[2])
end

function vec2.__div(a, b)
    a = vec2.frcoerce(a)
    b = vec2.frcoerce(b)
    return vec2.frnew(a[1] / b[1], a[2] / b[2])
end

function vec2.__unm(v)
    return vec2.frnew(-v[1], -v[2])
end

-- 3D Vector
vec3.getters = {
    x = function(v) return v[1] end,
    y = function(v) return v[2] end,
    z = function(v) return v[3] end,
    r = function(v) return v[1] end,
    g = function(v) return v[2] end,
    b = function(v) return v[3] end
}

vec3.setters = {
    x = function(v, val) v[1] = val end,
    y = function(v, val) v[2] = val end,
    z = function(v, val) v[3] = val end,
    r = function(v, val) v[1] = val end,
    g = function(v, val) v[2] = val end,
    b = function(v, val) v[3] = val end
}

vec3.__index = common.__index
vec3.__newindex = common.__newindex

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

function vec3.frnew(x, y, z)
    local v = {x, y, z}
    setmetatable(v, vec3)
    return v
end

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

function vec3.__tostring(v)
    return table.concat {"vec3<", v[1], ", ", v[2], ", ", v[3], ">"}
end

function vec3.__add(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] + b[1], a[2] + b[2], a[3] + b[3])
end

function vec3.__sub(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] - b[1], a[2] - b[2], a[3] - b[3])
end

function vec3.__mul(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] * b[1], a[2] * b[2], a[3] * b[3])
end

function vec3.__div(a, b)
    a = vec3.frcoerce(a)
    b = vec3.frcoerce(b)
    return vec3.frnew(a[1] / b[1], a[2] / b[2], a[3] / b[3])
end

function vec3.__unm(v)
    return vec3.frnew(-v[1], -v[2], -v[3])
end

-- Parent base types to common type.
setmetatable(vec2, common)
setmetatable(vec3, common)

-- Module exports.
return {
    vec2 = vec2,
    vec3 = vec3,
    vec4 = vec4
}
