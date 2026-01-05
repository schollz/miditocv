import * as luaparse from 'luaparse';
const luamin = require('lua-format')
import luaScript from './static/globals.lua';
import { formatText } from 'lua-fmt';
import markdownit from 'markdown-it'
import CodeMirror from 'codemirror';
import 'codemirror/lib/codemirror.css';
import 'codemirror/mode/lua/lua.js';
import 'codemirror/addon/comment/comment.js';
import '@fortawesome/fontawesome-free/css/all.min.css';

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
