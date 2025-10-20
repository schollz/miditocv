-- Example Lua script demonstrating the gate parameter
-- This script shows how to use gate to control trigger duration

-- Example 1: Basic gate usage with on_beat
-- Gate = 0 means immediate trigger (default behavior)
-- Gate = 0.5 means trigger stays high for half a beat
-- Gate = 1.0 means trigger stays high for full beat

bpm = 120

function on_beat(on)
  if on then
    volts = 3.3
    trigger = true
    gate = 0.5  -- Trigger stays high for half a beat
  end
end

-- Example 2: Using gate for ADSR envelope control
-- This is useful for controlling the release stage of an envelope

function on_note_on(channel, note, velocity)
  volts = (note - 60) / 12.0  -- Convert to CV
  trigger = true
  gate = velocity / 127.0      -- Use velocity to control gate time
end

function on_note_off(channel, note)
  trigger = false
  gate = 0  -- Immediate release
end

-- Example 3: Dynamic gate based on knob value

function on_knob(value)
  volts = value * 5.0
  trigger = value > 0.1
  gate = value  -- Gate duration controlled by knob (0.0 to 1.0)
end

-- Example 4: Using out[] table to set gate for specific outputs

function on_button(pressed)
  if pressed then
    -- Set gate for output 1
    out[1].volts = 5.0
    out[1].trigger = true
    out[1].gate = 0.25  -- Quarter beat duration
    
    -- Set gate for output 2
    out[2].volts = 3.3
    out[2].trigger = true
    out[2].gate = 0.75  -- Three-quarter beat duration
  end
end

-- Example 5: Trigger only on low to high transition
-- The gate parameter works in conjunction with proper trigger management

last_state = false

function on_cc(cc, value)
  local new_state = value > 64
  
  -- Only trigger on low to high transition
  if new_state and not last_state then
    volts = value / 127.0 * 5.0
    trigger = true
    gate = 0.5  -- Half beat gate
  end
  
  last_state = new_state
end
