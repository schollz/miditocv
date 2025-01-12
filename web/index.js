import * as luaparse from 'luaparse';
import luamin from 'luamin';
import luaScript from './static/globals.lua';
import { formatText } from 'lua-fmt';
import markdownit from 'markdown-it'
const md = markdownit()
const result = md.render('# markdown-it rulezz!');
console.log(result);

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

window.markdownParser = md;

// find any class marked `markdown` and render it
document.querySelectorAll('.markydown').forEach((el) => {
  // remove <pre> and </pre> tags
  let text = el.innerText.replace(/<pre>/g, '').replace(/<\/pre>/g, '');
  console.log(text);
  el.innerHTML = markdownParser.render(text);
});
