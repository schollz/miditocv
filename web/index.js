import * as luaparse from 'luaparse';
import luamin from 'luamin';
import luaScript from './static/globals.lua';
import { formatText } from 'lua-fmt';

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

// Attach `luamin` and `luaparse` to the `window` object
window.luamin = {
  parse: luaparse.parse,
  minify: luamin.minify,
};

window.globalsLua = luaScript;
