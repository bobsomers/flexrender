-- Common forwarding methods for all base types.
local common = {
    __call = function(parent, ...)
        local obj = parent.new(parent, ...)
        setmetatable(obj, parent)
        return obj
    end,

    __index = function(table, key)
        local mt = getmetatable(table)
        local func = mt.getters[key]
        if func == nil then
            error("no member named '" .. key .. "'", 2)
        end
        return func(table)
    end,

    __newindex = function(table, key, value)
        local mt = getmetatable(table)
        local func = mt.setters[key]
        if func == nil then
            error("no member named '" .. key .. "'", 2)
        end
        func(table, value)
    end
}

-- 3D Vector
local vec3 = {
    new = function(self, ...)
        local vec = nil

        local nargs = select("#", ...)
        if nargs == 1 then
            local val = select(1, ...)
            vec = {val, val, val}
        elseif nargs == 3 then
            vec = {select(1, ...), select(2, ...), select(3, ...)}
        else
            error("expected 1 or 3 arguments to vec3 constructor", 2)
        end

        setmetatable(vec, self)
        return vec
    end,

    getters = {
        x = function(a) return a[1] end,
        y = function(a) return a[2] end,
        z = function(a) return a[3] end,
        r = function(a) return a[1] end,
        g = function(a) return a[2] end,
        b = function(a) return a[3] end
    },

    setters = {
        x = function(a, val) a[1] = val end,
        y = function(a, val) a[2] = val end,
        z = function(a, val) a[3] = val end,
        r = function(a, val) a[1] = val end,
        g = function(a, val) a[2] = val end,
        b = function(a, val) a[3] = val end
    },

    __add = function(a, b)
        -- TODO
    end,

    __sub = function(a, b)
        -- TODO
    end,

    __mul = function(a, b)
        -- TODO
    end,

    __div = function(a, b)
        -- TODO
    end,

    __unm = function(a)
        a[1] = -a[1]
        a[2] = -a[2]
        a[3] = -a[3]
    end,

    __index = common.__index,
    __newindex = common.__newindex
}

-- Parent all base types to the common type.
setmetatable(vec3, common)

-- Module exports.
return {
    vec3 = vec3
}
