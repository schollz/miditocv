import * as luaparse from 'luaparse';
const luamin = require('lua-format')
import luaScript from './static/globals.lua';
import { formatText } from 'lua-fmt';
import markdownit from 'markdown-it'
import CodeMirror from 'codemirror';
import 'codemirror/lib/codemirror.css';
import 'codemirror/mode/lua/lua.js';
import 'codemirror/addon/comment/comment.js';
import 'codemirror/addon/hint/show-hint.js';
import 'codemirror/addon/hint/show-hint.css';
import '@fortawesome/fontawesome-free/css/all.min.css';

// Lua autocomplete hints for miditocv
const luaHints = [
  // Callback functions (user defines these)
  { text: 'function on_beat(on)\n  \nend', displayText: 'on_beat(on)', hint: 'Called on each beat' },
  { text: 'function on_button(value, shift)\n  \nend', displayText: 'on_button(value, shift)', hint: 'Called when button pressed' },
  { text: 'function on_knob(x)\n  \nend', displayText: 'on_knob(x)', hint: 'Called when knob turned (0-127)' },
  { text: 'function on_clock(division)\n  \nend', displayText: 'on_clock(division)', hint: 'Called on clock division' },
  { text: 'function on_note(note, velocity)\n  \nend', displayText: 'on_note(note, velocity)', hint: 'Called on MIDI note' },
  { text: 'function on_cc(cc, value)\n  \nend', displayText: 'on_cc(cc, value)', hint: 'Called on MIDI CC' },
  // Built-in functions
  { text: 'to_cv()', displayText: 'to_cv(value)', hint: 'Convert note/midi/voltage to CV' },
  { text: 'midi_to_cv()', displayText: 'midi_to_cv(midi_note, root, v_oct)', hint: 'Convert MIDI note to CV' },
  { text: 'note_to_midi()', displayText: 'note_to_midi(note)', hint: 'Convert note string to MIDI (e.g. "c4")' },
  { text: 'linlin()', displayText: 'linlin(x, in_min, in_max, out_min, out_max)', hint: 'Linear interpolation' },
  { text: 'er()', displayText: 'er(k, n, w)', hint: 'Euclidean rhythm as Sequins' },
  { text: 'S {}', displayText: 'S { }', hint: 'Create a Sequins sequence' },
  // Variables
  { text: 'volts', displayText: 'volts', hint: 'Output voltage' },
  { text: 'trigger', displayText: 'trigger', hint: 'Trigger output (boolean)' },
  { text: 'gate', displayText: 'gate', hint: 'Gate output' },
  { text: 'out[].volts', displayText: 'out[n].volts', hint: 'Per-output voltage' },
  { text: 'out[].trigger', displayText: 'out[n].trigger', hint: 'Per-output trigger' },
  { text: 'shift', displayText: 'shift', hint: 'Shift button state' },
  { text: 'button[]', displayText: 'button[n]', hint: 'Button state array' },
];

CodeMirror.registerHelper('hint', 'lua', function(editor) {
  const cur = editor.getCursor();
  const token = editor.getTokenAt(cur);
  const start = token.start;
  const end = cur.ch;
  const word = token.string.slice(0, end - start);

  const list = luaHints.filter(item => {
    const searchText = item.displayText.toLowerCase();
    return searchText.startsWith(word.toLowerCase()) ||
           item.text.toLowerCase().startsWith(word.toLowerCase());
  }).map(item => ({
    text: item.text,
    displayText: item.displayText + ' - ' + item.hint,
  }));

  return {
    list: list,
    from: CodeMirror.Pos(cur.line, start),
    to: CodeMirror.Pos(cur.line, end)
  };
});

// configure luamin so it doesn't rewrite variable names
luamin.options = { renameVariables: false };

// Beautify Lua code
function beautifyLua(luaCode) {
  try {
    return formatText(luaCode, { useTabs: false, indentCount: 2 });
  } catch (error) {
    console.error('Error beautifying Lua code:', error.message);
    return 'Error: Unable to beautify code.';
  }
}

// Attach to the global window object for browser usage
window.luaBeautifier = {
  beautify: beautifyLua,
};

// Attach CodeMirror to the global window object for browser usage
window.CodeMirror = CodeMirror;

const minify = (luaCode) => {
  const Settings = {
    RenameVariables: false,
    RenameGlobals: false,
    SolveMath: false,
    Indentation: '\t'
  };
  const minified_code = luamin.Minify(luaCode, Settings);
  if (minified_code.includes('\n\n\n\n')) {
    return minified_code.split('\n\n\n\n')[1];
  }
  return minified_code;
}

// Attach `luamin` and `luaparse` to the `window` object
window.luamin = {
  parse: luaparse.parse,
  minify: minify,
};

window.globalsLua = luaScript;

window.markdownParser = markdownit();

// find any class marked `markdown` and render it
document.querySelectorAll('.markydown').forEach((el) => {
  // remove <pre> and </pre> tags
  let text = el.innerText.replace(/<pre>/g, '').replace(/<\/pre>/g, '');
  console.log(text);
  el.innerHTML = markdownParser.render(text);
});

// Initialize LuaJS - must be in same module as globalsLua to ensure correct ordering
async function initializeLuaJS() {
  const emscriptenInit = (await import('./static/dist/luajs.mjs')).default;
  const LuaJS = await emscriptenInit();
  window.LuaJS = LuaJS;
  try {
    const response = await fetch(luaScript);
    const data = await response.text();
    window.luaState = window.LuaJS.newState();
    window.luaState.then(async (L) => {
      await L.run(data);
    });
    await window.luaState;
    console.log(`[LuaJS] created lua environments`);
  } catch (error) {
    console.error('Error fetching or setting up Lua environment:', error);
  }
}
initializeLuaJS();
