---------------------
------ SEQUINS ------
---------------------
S = {}
local function a(b)
    if type(b) == 'string' then
        local c = {}
        b:gsub('.', function(d)
            table.insert(c, d)
        end)
        return c
    end
    return b
end
function S.new(b)
    b = a(b)
    local e = {
        data = b,
        length = #b,
        ix = 1,
        qix = 1,
        n = 1,
        flw = {},
        fun = {}
    }
    return setmetatable(e, S)
end
local function f(e, g)
    return (g - 1) % e.length + 1
end
function S:is_sequins()
    return getmetatable(self) == S
end
function S:setdata(b)
    if S.is_sequins(b) then
        do
            local h = b.flw;
            local i = self.flw;
            for j, k in pairs(i) do
                if h[j] then
                    i[j].n = h[j].n
                else
                    i[j] = nil
                end
            end
            for j, k in pairs(h) do
                if not i[j] then
                    i[j] = {
                        ix = k.ix,
                        n = k.n
                    }
                end
            end
        end
        do
            local h = b.fun;
            local i = self.fun;
            if i[1] then
                i[1] = h[1]
                if h[2] then
                    for j, k in ipairs(h[2]) do
                        if S.is_sequins(k) and i[2] and S.is_sequins(i[2][j]) then
                            i[2][j]:settable(k)
                        else
                            i[2][j] = k
                        end
                    end
                end
            elseif h[1] then
                print 'new transformer'
                self.fun = b.fun
            end
        end
        b = b.data
    end
    b = a(b)
    for l = 1, #b do
        if S.is_sequins(b[l]) and S.is_sequins(self.data[l]) then
            self.data[l]:settable(b[l])
        else
            self.data[l] = b[l]
        end
    end
    self.data[#b + 1] = nil;
    self.length = #b;
    self.ix = f(self, self.ix)
end
function S:copy()
    if type(self) == 'table' then
        local m = {}
        for j, k in pairs(self) do
            m[j] = S.copy(k)
        end
        return setmetatable(m, getmetatable(self))
    else
        return self
    end
end
function S:peek()
    return self.data[self.ix]
end
function S:bake(n)
    n = n or #self;
    local b = {}
    for l = 1, n do
        b[l] = self()
    end
    return S.new(b)
end
local function o(b, p)
    p = p or S.next;
    if S.is_sequins(b) then
        return p(b)
    end
    return b
end
function S:func(p, ...)
    self.fun = {p, {...}}
    return self
end
local function q(e, k)
    if e.fun[1] then
        if #e.fun[2] > 0 then
            return e.fun[1](k, table.unpack(e.fun[2]))
        else
            return e.fun[1](k)
        end
    else
        return k
    end
end
S._fns = {
    add = function(n, r)
        return n + o(r)
    end,
    sub = function(n, r)
        return n - o(r)
    end,
    mul = function(n, r)
        return n * o(r)
    end,
    div = function(n, r)
        return n / o(r)
    end,
    mod = function(n, r)
        return n % o(r)
    end
}
S.__add = function(s, r)
    return S.func(s, S._fns.add, r)
end;
S.__sub = function(s, r)
    return S.func(s, S._fns.sub, r)
end;
S.__mul = function(s, r)
    return S.func(s, S._fns.mul, r)
end;
S.__div = function(s, r)
    return S.func(s, S._fns.div, r)
end;
S.__mod = function(s, r)
    return S.func(s, S._fns.mod, r)
end;
local function t(e)
    local u = f(e, e.qix or e.ix + o(e.n))
    local v, w = o(e.data[u])
    if w ~= 'again' then
        e.ix = u;
        e.qix = nil
    end
    if v == 'skip' or v == 'dead' then
        return S.next(e)
    end
    return v, w
end
S.flows = {
    every = function(x, n)
        return x.ix % n ~= 0
    end,
    times = function(x, n)
        return x.ix > n
    end,
    count = function(x, n)
        if x.ix < n then
            return 'again'
        else
            x.ix = 0
        end
    end
}
local function y(e, j)
    local x = e.flw[j]
    if x then
        x.ix = x.ix + 1;
        return S.flows[j](x, o(x.n))
    end
end
function S.next(e)
    if y(e, 'every') then
        return 'skip'
    end
    if y(e, 'times') then
        return 'dead'
    end
    local z = y(e, 'count')
    if z then
        local A = e.flw.every;
        if A then
            A.ix = A.ix - 1
        end
    end
    return q(e, t(e)), z
end
function S:step(n)
    self.n = n;
    return self
end
function S.flow(e, j, n)
    e.flw[j] = {
        n = n,
        ix = 0
    }
    return e
end
function S:every(n)
    return self:flow('every', n)
end
function S:count(n)
    return self:flow('count', n)
end
function S:times(n)
    return self:flow('times', n)
end
function S:all()
    return self:flow('count', #self)
end
function S:select(g)
    rawset(self, 'qix', g)
    return self
end
function S:reset()
    self:select(1)
    for B, k in ipairs(self.data) do
        o(k, S.reset)
    end
    for B, k in pairs(self.flw) do
        k.ix = 0;
        o(k.n, S.reset)
    end
    if self.fun[1] and #self.fun[2] > 0 then
        for B, k in pairs(self.fun[2]) do
            o(k, S.reset)
        end
    end
end
S.__call = function(self, ...)
    return self == S and S.new(...) or S.next(self)
end;
S.metaix = {
    settable = S.setdata,
    step = S.step,
    flow = S.flow,
    every = S.every,
    times = S.times,
    count = S.count,
    all = S.all,
    reset = S.reset,
    select = S.select,
    peek = S.peek,
    copy = S.copy,
    map = S.func,
    bake = S.bake
}
S.__index = function(self, g)
    if type(g) == 'number' then
        return self.data[g]
    else
        return S.metaix[g]
    end
end;
S.__newindex = function(self, g, k)
    if type(g) == 'number' then
        self.data[g] = k
    elseif g == 'n' then
        rawset(self, g, k)
    end
end;
S.__len = function(b)
    return b.length
end;
setmetatable(S, S)

function random_number()
    return math.random(100, 150)
end

---------------------------
----- MUSIC LIBRARY -------
---------------------------
-- convert strings like 'c4' to midi notes
-- if the string does not have an octave, use the previous octave
previous_octave = 4
function note_to_midi(note)
    local notes = {
        c = 0,
        d = 2,
        e = 4,
        f = 5,
        g = 7,
        a = 9,
        b = 11
    }

    if type(note) ~= "string" or tonumber(note) then
        error("note_to_midi: 'note' must be musical note string.")
        do
            return
        end
    end

    local octave = note:match("%d+") or previous_octave
    previous_octave = octave
    midi = 12 * (octave + 1) + notes[note:sub(1, 1)] + (note:sub(2, 2) == "#" and 1 or 0) +
               (note:sub(2, 2) == "b" and -1 or 0) + (note:sub(2, 2) == "s" and 1 or 0)
    return midi
end

-- assert(note_to_midi("c4") == 60)
-- assert(note_to_midi("d") == 62)
-- assert(note_to_midi("d#4") == 63)
-- assert(note_to_midi("db4") == 61)

function midi_to_cv(midi_note, root, v_oct)
    if (midi_note <= 10) then
        error("midi_to_cv: 'midi_note' must be a number > 10")
        do
            return
        end
    end
    root = root or 60
    v_oct = v_oct or 1
    return (midi_note - root) / (12.0 * v_oct)
end

-- assert(midi_to_cv(60) == 0)
-- assert(midi_to_cv(61) == 1 / 12)
-- assert(midi_to_cv(62) == 2 / 12)

function to_cv(val)
    -- converts a note, midi, or a voltage to a voltage
    local success, result = pcall(function()
        return midi_to_cv(val)
    end)
    if success then
        return result
    end
    success, result = pcall(function()
        return midi_to_cv(note_to_midi(val))
    end)
    if success then
        return result
    end
    success, result = pcall(function()
        return tonumber(val)
    end)
    if success then
        return result
    end
    return -15
end

-- print(to_cv(62))
-- print(to_cv("c5"))
-- print(to_cv(1.2))

------------------------
-- environment blocks --
------------------------
function new_env(code)
    local env = {}
    setmetatable(env, {
        __index = function(_, key)
            return _G[key] or 0
        end,
        __newindex = function(_, key, value)
            rawset(env, key, value)
        end
    })
    local chunk = load(code, "chunk", "t", env)
    chunk()
    return env
end

envs = {}
for i = 1, 8 do
    table.insert(envs, new_env(""))
end

function update_env(i, code)
    envs[i] = new_env([[
volts = 0
trigger = 0
iteration_num = 0

function on_beat_call(beat)
    if type(on_beat) == "function" then
        local success, result = pcall(function()
            return on_beat(beat)
        end)
        if success then
            return result
        end
    end
end

function on_knob_call(value,shift,button)
    if type(on_knob) == "function" then
        local success, result = pcall(function()
            return on_knob(value,shift,button)
        end)
        if success then
            return result
        end
    end
end

function on_button_call(value,shift)
    if type(on_button) == "function" then
        local success, result = pcall(function()
            return on_button(value,shift)
        end)
        if success then
            return result
        end
    end
end
]] .. code)
end

function test_on_beat(i)
    envs[i].iteration_num = envs[i].iteration_num + 1
    local v = envs[i].on_beat_call(envs[i].iteration_num)
    if v then
        return string.format("%d) %f volts, %s, %s", envs[i].iteration_num, envs[i].volts, envs[i].trigger, v)
    end
end

math.randomseed(os.time())

-- testing
update_env(1, [[
a = S{60,62,S{70,75},67}
b = S{1,1,1,0}
c = S{10,13,15,S{17,20}}
function on_beat(beat)
    local v = a() + b() + c()
    volts = to_cv(v)
    trigger = b()>0
    return v
end

local notes = S{"c4", "d4", "e4", "f4", "g4", "a4", "b4", "c5"}
function on_button(value, shift)
    if value then 
        local note = notes()
        return note
    end
end
]])

for i = 1, 10 do
    print(test_on_beat(1))
end
for i = 1, 10 do
    print(envs[1].on_button_call(1, 0), envs[1].volts)
end
