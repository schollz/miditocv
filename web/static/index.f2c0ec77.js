// modules are defined as an array
// [ module function, map of requires ]
//
// map of requires is short require name -> numeric require
//
// anything defined in a previous bundle is accessed via the
// orig method which is the require for previous bundles

(function (modules, entry, mainEntry, parcelRequireName, globalName) {
  /* eslint-disable no-undef */
  var globalObject =
    typeof globalThis !== 'undefined'
      ? globalThis
      : typeof self !== 'undefined'
      ? self
      : typeof window !== 'undefined'
      ? window
      : typeof global !== 'undefined'
      ? global
      : {};
  /* eslint-enable no-undef */

  // Save the require from previous bundle to this closure if any
  var previousRequire =
    typeof globalObject[parcelRequireName] === 'function' &&
    globalObject[parcelRequireName];

  var cache = previousRequire.cache || {};
  // Do not use `require` to prevent Webpack from trying to bundle this call
  var nodeRequire =
    typeof module !== 'undefined' &&
    typeof module.require === 'function' &&
    module.require.bind(module);

  function newRequire(name, jumped) {
    if (!cache[name]) {
      if (!modules[name]) {
        // if we cannot find the module within our internal map or
        // cache jump to the current global require ie. the last bundle
        // that was added to the page.
        var currentRequire =
          typeof globalObject[parcelRequireName] === 'function' &&
          globalObject[parcelRequireName];
        if (!jumped && currentRequire) {
          return currentRequire(name, true);
        }

        // If there are other bundles on this page the require from the
        // previous one is saved to 'previousRequire'. Repeat this as
        // many times as there are bundles until the module is found or
        // we exhaust the require chain.
        if (previousRequire) {
          return previousRequire(name, true);
        }

        // Try the node require function if it exists.
        if (nodeRequire && typeof name === 'string') {
          return nodeRequire(name);
        }

        var err = new Error("Cannot find module '" + name + "'");
        err.code = 'MODULE_NOT_FOUND';
        throw err;
      }

      localRequire.resolve = resolve;
      localRequire.cache = {};

      var module = (cache[name] = new newRequire.Module(name));

      modules[name][0].call(
        module.exports,
        localRequire,
        module,
        module.exports,
        globalObject
      );
    }

    return cache[name].exports;

    function localRequire(x) {
      var res = localRequire.resolve(x);
      return res === false ? {} : newRequire(res);
    }

    function resolve(x) {
      var id = modules[name][1][x];
      return id != null ? id : x;
    }
  }

  function Module(moduleName) {
    this.id = moduleName;
    this.bundle = newRequire;
    this.exports = {};
  }

  newRequire.isParcelRequire = true;
  newRequire.Module = Module;
  newRequire.modules = modules;
  newRequire.cache = cache;
  newRequire.parent = previousRequire;
  newRequire.register = function (id, exports) {
    modules[id] = [
      function (require, module) {
        module.exports = exports;
      },
      {},
    ];
  };

  Object.defineProperty(newRequire, 'root', {
    get: function () {
      return globalObject[parcelRequireName];
    },
  });

  globalObject[parcelRequireName] = newRequire;

  for (var i = 0; i < entry.length; i++) {
    newRequire(entry[i]);
  }

  if (mainEntry) {
    // Expose entry point to Node, AMD or browser globals
    // Based on https://github.com/ForbesLindesay/umd/blob/master/template.js
    var mainExports = newRequire(mainEntry);

    // CommonJS
    if (typeof exports === 'object' && typeof module !== 'undefined') {
      module.exports = mainExports;

      // RequireJS
    } else if (typeof define === 'function' && define.amd) {
      define(function () {
        return mainExports;
      });

      // <script>
    } else if (globalName) {
      this[globalName] = mainExports;
    }
  }
})({"32O6c":[function(require,module,exports,__globalThis) {
var global = arguments[3];
var HMR_HOST = null;
var HMR_PORT = null;
var HMR_SECURE = false;
var HMR_ENV_HASH = "d6ea1d42532a7575";
var HMR_USE_SSE = false;
module.bundle.HMR_BUNDLE_ID = "0906654af2c0ec77";
"use strict";
/* global HMR_HOST, HMR_PORT, HMR_ENV_HASH, HMR_SECURE, HMR_USE_SSE, chrome, browser, __parcel__import__, __parcel__importScripts__, ServiceWorkerGlobalScope */ /*::
import type {
  HMRAsset,
  HMRMessage,
} from '@parcel/reporter-dev-server/src/HMRServer.js';
interface ParcelRequire {
  (string): mixed;
  cache: {|[string]: ParcelModule|};
  hotData: {|[string]: mixed|};
  Module: any;
  parent: ?ParcelRequire;
  isParcelRequire: true;
  modules: {|[string]: [Function, {|[string]: string|}]|};
  HMR_BUNDLE_ID: string;
  root: ParcelRequire;
}
interface ParcelModule {
  hot: {|
    data: mixed,
    accept(cb: (Function) => void): void,
    dispose(cb: (mixed) => void): void,
    // accept(deps: Array<string> | string, cb: (Function) => void): void,
    // decline(): void,
    _acceptCallbacks: Array<(Function) => void>,
    _disposeCallbacks: Array<(mixed) => void>,
  |};
}
interface ExtensionContext {
  runtime: {|
    reload(): void,
    getURL(url: string): string;
    getManifest(): {manifest_version: number, ...};
  |};
}
declare var module: {bundle: ParcelRequire, ...};
declare var HMR_HOST: string;
declare var HMR_PORT: string;
declare var HMR_ENV_HASH: string;
declare var HMR_SECURE: boolean;
declare var HMR_USE_SSE: boolean;
declare var chrome: ExtensionContext;
declare var browser: ExtensionContext;
declare var __parcel__import__: (string) => Promise<void>;
declare var __parcel__importScripts__: (string) => Promise<void>;
declare var globalThis: typeof self;
declare var ServiceWorkerGlobalScope: Object;
*/ var OVERLAY_ID = '__parcel__error__overlay__';
var OldModule = module.bundle.Module;
function Module(moduleName) {
    OldModule.call(this, moduleName);
    this.hot = {
        data: module.bundle.hotData[moduleName],
        _acceptCallbacks: [],
        _disposeCallbacks: [],
        accept: function(fn) {
            this._acceptCallbacks.push(fn || function() {});
        },
        dispose: function(fn) {
            this._disposeCallbacks.push(fn);
        }
    };
    module.bundle.hotData[moduleName] = undefined;
}
module.bundle.Module = Module;
module.bundle.hotData = {};
var checkedAssets /*: {|[string]: boolean|} */ , disposedAssets /*: {|[string]: boolean|} */ , assetsToDispose /*: Array<[ParcelRequire, string]> */ , assetsToAccept /*: Array<[ParcelRequire, string]> */ ;
function getHostname() {
    return HMR_HOST || (location.protocol.indexOf('http') === 0 ? location.hostname : 'localhost');
}
function getPort() {
    return HMR_PORT || location.port;
}
// eslint-disable-next-line no-redeclare
var parent = module.bundle.parent;
if ((!parent || !parent.isParcelRequire) && typeof WebSocket !== 'undefined') {
    var hostname = getHostname();
    var port = getPort();
    var protocol = HMR_SECURE || location.protocol == 'https:' && ![
        'localhost',
        '127.0.0.1',
        '0.0.0.0'
    ].includes(hostname) ? 'wss' : 'ws';
    var ws;
    if (HMR_USE_SSE) ws = new EventSource('/__parcel_hmr');
    else try {
        ws = new WebSocket(protocol + '://' + hostname + (port ? ':' + port : '') + '/');
    } catch (err) {
        if (err.message) console.error(err.message);
        ws = {};
    }
    // Web extension context
    var extCtx = typeof browser === 'undefined' ? typeof chrome === 'undefined' ? null : chrome : browser;
    // Safari doesn't support sourceURL in error stacks.
    // eval may also be disabled via CSP, so do a quick check.
    var supportsSourceURL = false;
    try {
        (0, eval)('throw new Error("test"); //# sourceURL=test.js');
    } catch (err) {
        supportsSourceURL = err.stack.includes('test.js');
    }
    // $FlowFixMe
    ws.onmessage = async function(event /*: {data: string, ...} */ ) {
        checkedAssets = {} /*: {|[string]: boolean|} */ ;
        disposedAssets = {} /*: {|[string]: boolean|} */ ;
        assetsToAccept = [];
        assetsToDispose = [];
        var data /*: HMRMessage */  = JSON.parse(event.data);
        if (data.type === 'reload') fullReload();
        else if (data.type === 'update') {
            // Remove error overlay if there is one
            if (typeof document !== 'undefined') removeErrorOverlay();
            let assets = data.assets.filter((asset)=>asset.envHash === HMR_ENV_HASH);
            // Handle HMR Update
            let handled = assets.every((asset)=>{
                return asset.type === 'css' || asset.type === 'js' && hmrAcceptCheck(module.bundle.root, asset.id, asset.depsByBundle);
            });
            if (handled) {
                console.clear();
                // Dispatch custom event so other runtimes (e.g React Refresh) are aware.
                if (typeof window !== 'undefined' && typeof CustomEvent !== 'undefined') window.dispatchEvent(new CustomEvent('parcelhmraccept'));
                await hmrApplyUpdates(assets);
                hmrDisposeQueue();
                // Run accept callbacks. This will also re-execute other disposed assets in topological order.
                let processedAssets = {};
                for(let i = 0; i < assetsToAccept.length; i++){
                    let id = assetsToAccept[i][1];
                    if (!processedAssets[id]) {
                        hmrAccept(assetsToAccept[i][0], id);
                        processedAssets[id] = true;
                    }
                }
            } else fullReload();
        }
        if (data.type === 'error') {
            // Log parcel errors to console
            for (let ansiDiagnostic of data.diagnostics.ansi){
                let stack = ansiDiagnostic.codeframe ? ansiDiagnostic.codeframe : ansiDiagnostic.stack;
                console.error("\uD83D\uDEA8 [parcel]: " + ansiDiagnostic.message + '\n' + stack + '\n\n' + ansiDiagnostic.hints.join('\n'));
            }
            if (typeof document !== 'undefined') {
                // Render the fancy html overlay
                removeErrorOverlay();
                var overlay = createErrorOverlay(data.diagnostics.html);
                // $FlowFixMe
                document.body.appendChild(overlay);
            }
        }
    };
    if (ws instanceof WebSocket) {
        ws.onerror = function(e) {
            if (e.message) console.error(e.message);
        };
        ws.onclose = function() {
            console.warn("[parcel] \uD83D\uDEA8 Connection to the HMR server was lost");
        };
    }
}
function removeErrorOverlay() {
    var overlay = document.getElementById(OVERLAY_ID);
    if (overlay) {
        overlay.remove();
        console.log("[parcel] \u2728 Error resolved");
    }
}
function createErrorOverlay(diagnostics) {
    var overlay = document.createElement('div');
    overlay.id = OVERLAY_ID;
    let errorHTML = '<div style="background: black; opacity: 0.85; font-size: 16px; color: white; position: fixed; height: 100%; width: 100%; top: 0px; left: 0px; padding: 30px; font-family: Menlo, Consolas, monospace; z-index: 9999;">';
    for (let diagnostic of diagnostics){
        let stack = diagnostic.frames.length ? diagnostic.frames.reduce((p, frame)=>{
            return `${p}
<a href="/__parcel_launch_editor?file=${encodeURIComponent(frame.location)}" style="text-decoration: underline; color: #888" onclick="fetch(this.href); return false">${frame.location}</a>
${frame.code}`;
        }, '') : diagnostic.stack;
        errorHTML += `
      <div>
        <div style="font-size: 18px; font-weight: bold; margin-top: 20px;">
          \u{1F6A8} ${diagnostic.message}
        </div>
        <pre>${stack}</pre>
        <div>
          ${diagnostic.hints.map((hint)=>"<div>\uD83D\uDCA1 " + hint + '</div>').join('')}
        </div>
        ${diagnostic.documentation ? `<div>\u{1F4DD} <a style="color: violet" href="${diagnostic.documentation}" target="_blank">Learn more</a></div>` : ''}
      </div>
    `;
    }
    errorHTML += '</div>';
    overlay.innerHTML = errorHTML;
    return overlay;
}
function fullReload() {
    if ('reload' in location) location.reload();
    else if (extCtx && extCtx.runtime && extCtx.runtime.reload) extCtx.runtime.reload();
}
function getParents(bundle, id) /*: Array<[ParcelRequire, string]> */ {
    var modules = bundle.modules;
    if (!modules) return [];
    var parents = [];
    var k, d, dep;
    for(k in modules)for(d in modules[k][1]){
        dep = modules[k][1][d];
        if (dep === id || Array.isArray(dep) && dep[dep.length - 1] === id) parents.push([
            bundle,
            k
        ]);
    }
    if (bundle.parent) parents = parents.concat(getParents(bundle.parent, id));
    return parents;
}
function updateLink(link) {
    var href = link.getAttribute('href');
    if (!href) return;
    var newLink = link.cloneNode();
    newLink.onload = function() {
        if (link.parentNode !== null) // $FlowFixMe
        link.parentNode.removeChild(link);
    };
    newLink.setAttribute('href', // $FlowFixMe
    href.split('?')[0] + '?' + Date.now());
    // $FlowFixMe
    link.parentNode.insertBefore(newLink, link.nextSibling);
}
var cssTimeout = null;
function reloadCSS() {
    if (cssTimeout) return;
    cssTimeout = setTimeout(function() {
        var links = document.querySelectorAll('link[rel="stylesheet"]');
        for(var i = 0; i < links.length; i++){
            // $FlowFixMe[incompatible-type]
            var href /*: string */  = links[i].getAttribute('href');
            var hostname = getHostname();
            var servedFromHMRServer = hostname === 'localhost' ? new RegExp('^(https?:\\/\\/(0.0.0.0|127.0.0.1)|localhost):' + getPort()).test(href) : href.indexOf(hostname + ':' + getPort());
            var absolute = /^https?:\/\//i.test(href) && href.indexOf(location.origin) !== 0 && !servedFromHMRServer;
            if (!absolute) updateLink(links[i]);
        }
        cssTimeout = null;
    }, 50);
}
function hmrDownload(asset) {
    if (asset.type === 'js') {
        if (typeof document !== 'undefined') {
            let script = document.createElement('script');
            script.src = asset.url + '?t=' + Date.now();
            if (asset.outputFormat === 'esmodule') script.type = 'module';
            return new Promise((resolve, reject)=>{
                var _document$head;
                script.onload = ()=>resolve(script);
                script.onerror = reject;
                (_document$head = document.head) === null || _document$head === void 0 || _document$head.appendChild(script);
            });
        } else if (typeof importScripts === 'function') {
            // Worker scripts
            if (asset.outputFormat === 'esmodule') return import(asset.url + '?t=' + Date.now());
            else return new Promise((resolve, reject)=>{
                try {
                    importScripts(asset.url + '?t=' + Date.now());
                    resolve();
                } catch (err) {
                    reject(err);
                }
            });
        }
    }
}
async function hmrApplyUpdates(assets) {
    global.parcelHotUpdate = Object.create(null);
    let scriptsToRemove;
    try {
        // If sourceURL comments aren't supported in eval, we need to load
        // the update from the dev server over HTTP so that stack traces
        // are correct in errors/logs. This is much slower than eval, so
        // we only do it if needed (currently just Safari).
        // https://bugs.webkit.org/show_bug.cgi?id=137297
        // This path is also taken if a CSP disallows eval.
        if (!supportsSourceURL) {
            let promises = assets.map((asset)=>{
                var _hmrDownload;
                return (_hmrDownload = hmrDownload(asset)) === null || _hmrDownload === void 0 ? void 0 : _hmrDownload.catch((err)=>{
                    // Web extension fix
                    if (extCtx && extCtx.runtime && extCtx.runtime.getManifest().manifest_version == 3 && typeof ServiceWorkerGlobalScope != 'undefined' && global instanceof ServiceWorkerGlobalScope) {
                        extCtx.runtime.reload();
                        return;
                    }
                    throw err;
                });
            });
            scriptsToRemove = await Promise.all(promises);
        }
        assets.forEach(function(asset) {
            hmrApply(module.bundle.root, asset);
        });
    } finally{
        delete global.parcelHotUpdate;
        if (scriptsToRemove) scriptsToRemove.forEach((script)=>{
            if (script) {
                var _document$head2;
                (_document$head2 = document.head) === null || _document$head2 === void 0 || _document$head2.removeChild(script);
            }
        });
    }
}
function hmrApply(bundle /*: ParcelRequire */ , asset /*:  HMRAsset */ ) {
    var modules = bundle.modules;
    if (!modules) return;
    if (asset.type === 'css') reloadCSS();
    else if (asset.type === 'js') {
        let deps = asset.depsByBundle[bundle.HMR_BUNDLE_ID];
        if (deps) {
            if (modules[asset.id]) {
                // Remove dependencies that are removed and will become orphaned.
                // This is necessary so that if the asset is added back again, the cache is gone, and we prevent a full page reload.
                let oldDeps = modules[asset.id][1];
                for(let dep in oldDeps)if (!deps[dep] || deps[dep] !== oldDeps[dep]) {
                    let id = oldDeps[dep];
                    let parents = getParents(module.bundle.root, id);
                    if (parents.length === 1) hmrDelete(module.bundle.root, id);
                }
            }
            if (supportsSourceURL) // Global eval. We would use `new Function` here but browser
            // support for source maps is better with eval.
            (0, eval)(asset.output);
            // $FlowFixMe
            let fn = global.parcelHotUpdate[asset.id];
            modules[asset.id] = [
                fn,
                deps
            ];
        }
        // Always traverse to the parent bundle, even if we already replaced the asset in this bundle.
        // This is required in case modules are duplicated. We need to ensure all instances have the updated code.
        if (bundle.parent) hmrApply(bundle.parent, asset);
    }
}
function hmrDelete(bundle, id) {
    let modules = bundle.modules;
    if (!modules) return;
    if (modules[id]) {
        // Collect dependencies that will become orphaned when this module is deleted.
        let deps = modules[id][1];
        let orphans = [];
        for(let dep in deps){
            let parents = getParents(module.bundle.root, deps[dep]);
            if (parents.length === 1) orphans.push(deps[dep]);
        }
        // Delete the module. This must be done before deleting dependencies in case of circular dependencies.
        delete modules[id];
        delete bundle.cache[id];
        // Now delete the orphans.
        orphans.forEach((id)=>{
            hmrDelete(module.bundle.root, id);
        });
    } else if (bundle.parent) hmrDelete(bundle.parent, id);
}
function hmrAcceptCheck(bundle /*: ParcelRequire */ , id /*: string */ , depsByBundle /*: ?{ [string]: { [string]: string } }*/ ) {
    if (hmrAcceptCheckOne(bundle, id, depsByBundle)) return true;
    // Traverse parents breadth first. All possible ancestries must accept the HMR update, or we'll reload.
    let parents = getParents(module.bundle.root, id);
    let accepted = false;
    while(parents.length > 0){
        let v = parents.shift();
        let a = hmrAcceptCheckOne(v[0], v[1], null);
        if (a) // If this parent accepts, stop traversing upward, but still consider siblings.
        accepted = true;
        else {
            // Otherwise, queue the parents in the next level upward.
            let p = getParents(module.bundle.root, v[1]);
            if (p.length === 0) {
                // If there are no parents, then we've reached an entry without accepting. Reload.
                accepted = false;
                break;
            }
            parents.push(...p);
        }
    }
    return accepted;
}
function hmrAcceptCheckOne(bundle /*: ParcelRequire */ , id /*: string */ , depsByBundle /*: ?{ [string]: { [string]: string } }*/ ) {
    var modules = bundle.modules;
    if (!modules) return;
    if (depsByBundle && !depsByBundle[bundle.HMR_BUNDLE_ID]) {
        // If we reached the root bundle without finding where the asset should go,
        // there's nothing to do. Mark as "accepted" so we don't reload the page.
        if (!bundle.parent) return true;
        return hmrAcceptCheck(bundle.parent, id, depsByBundle);
    }
    if (checkedAssets[id]) return true;
    checkedAssets[id] = true;
    var cached = bundle.cache[id];
    assetsToDispose.push([
        bundle,
        id
    ]);
    if (!cached || cached.hot && cached.hot._acceptCallbacks.length) {
        assetsToAccept.push([
            bundle,
            id
        ]);
        return true;
    }
}
function hmrDisposeQueue() {
    // Dispose all old assets.
    for(let i = 0; i < assetsToDispose.length; i++){
        let id = assetsToDispose[i][1];
        if (!disposedAssets[id]) {
            hmrDispose(assetsToDispose[i][0], id);
            disposedAssets[id] = true;
        }
    }
    assetsToDispose = [];
}
function hmrDispose(bundle /*: ParcelRequire */ , id /*: string */ ) {
    var cached = bundle.cache[id];
    bundle.hotData[id] = {};
    if (cached && cached.hot) cached.hot.data = bundle.hotData[id];
    if (cached && cached.hot && cached.hot._disposeCallbacks.length) cached.hot._disposeCallbacks.forEach(function(cb) {
        cb(bundle.hotData[id]);
    });
    delete bundle.cache[id];
}
function hmrAccept(bundle /*: ParcelRequire */ , id /*: string */ ) {
    // Execute the module.
    bundle(id);
    // Run the accept callbacks in the new version of the module.
    var cached = bundle.cache[id];
    if (cached && cached.hot && cached.hot._acceptCallbacks.length) {
        let assetsToAlsoAccept = [];
        cached.hot._acceptCallbacks.forEach(function(cb) {
            let additionalAssets = cb(function() {
                return getParents(module.bundle.root, id);
            });
            if (Array.isArray(additionalAssets) && additionalAssets.length) assetsToAlsoAccept.push(...additionalAssets);
        });
        if (assetsToAlsoAccept.length) {
            let handled = assetsToAlsoAccept.every(function(a) {
                return hmrAcceptCheck(a[0], a[1]);
            });
            if (!handled) return fullReload();
            hmrDisposeQueue();
        }
    }
}

},{}],"idZQs":[function(require,module,exports,__globalThis) {
var parcelHelpers = require("@parcel/transformer-js/src/esmodule-helpers.js");
var _luaparse = require("luaparse");
var _luamin = require("luamin");
var _luaminDefault = parcelHelpers.interopDefault(_luamin);
// Attach `luamin` and `luaparse` to the `window` object
window.luamin = {
    parse: _luaparse.parse,
    minify: (0, _luaminDefault.default).minify
};

},{"luaparse":"9Jza9","luamin":"axf7q","@parcel/transformer-js/src/esmodule-helpers.js":"gkKU3"}],"9Jza9":[function(require,module,exports,__globalThis) {
var global = arguments[3];
/* global exports:true, module:true, require:true, define:true, global:true */ (function(root, name, factory) {
    'use strict';
    // Used to determine if values are of the language type `Object`
    var objectTypes = {
        'function': true,
        'object': true
    }, freeExports = objectTypes["object"] && exports && !exports.nodeType && exports, freeModule = objectTypes["object"] && module && !module.nodeType && module, freeGlobal = freeExports && freeModule && typeof global === 'object' && global, moduleExports = freeModule && freeModule.exports === freeExports && freeExports;
    /* istanbul ignore else */ if (freeGlobal && (freeGlobal.global === freeGlobal || /* istanbul ignore next */ freeGlobal.window === freeGlobal || /* istanbul ignore next */ freeGlobal.self === freeGlobal)) root = freeGlobal;
    // Some AMD build optimizers, like r.js, check for specific condition
    // patterns like the following:
    /* istanbul ignore if */ if (typeof define === 'function' && /* istanbul ignore next */ typeof define.amd === 'object' && /* istanbul ignore next */ define.amd) {
        // defined as an anonymous module.
        define([
            'exports'
        ], factory);
        // In case the source has been processed and wrapped in a define module use
        // the supplied `exports` object.
        if (freeExports && moduleExports) factory(freeModule.exports);
    } else /* istanbul ignore else */ if (freeExports && freeModule) {
        // in Node.js or RingoJS v0.8.0+
        /* istanbul ignore else */ if (moduleExports) factory(freeModule.exports);
        else factory(freeExports);
    } else factory(root[name] = {});
})(this, 'luaparse', function(exports1) {
    'use strict';
    exports1.version = "0.3.1";
    var input, options, length, features, encodingMode;
    // Options can be set either globally on the parser object through
    // defaultOptions, or during the parse call.
    var defaultOptions = exports1.defaultOptions = {
        // Explicitly tell the parser when the input ends.
        wait: false,
        comments: true,
        scope: false,
        locations: false,
        ranges: false,
        onCreateNode: null,
        onCreateScope: null,
        onDestroyScope: null,
        onLocalDeclaration: null,
        luaVersion: '5.1',
        encodingMode: 'none'
    };
    function encodeUTF8(codepoint, highMask) {
        highMask = highMask || 0;
        if (codepoint < 0x80) return String.fromCharCode(codepoint);
        else if (codepoint < 0x800) return String.fromCharCode(highMask | 0xc0 | codepoint >> 6, highMask | 0x80 | codepoint & 0x3f);
        else if (codepoint < 0x10000) return String.fromCharCode(highMask | 0xe0 | codepoint >> 12, highMask | 0x80 | codepoint >> 6 & 0x3f, highMask | 0x80 | codepoint & 0x3f);
        else /* istanbul ignore else */ if (codepoint < 0x110000) return String.fromCharCode(highMask | 0xf0 | codepoint >> 18, highMask | 0x80 | codepoint >> 12 & 0x3f, highMask | 0x80 | codepoint >> 6 & 0x3f, highMask | 0x80 | codepoint & 0x3f);
        else // TODO: Lua 5.4 allows up to six-byte sequences, as in UTF-8:1993
        return null;
    }
    function toHex(num, digits) {
        var result = num.toString(16);
        while(result.length < digits)result = '0' + result;
        return result;
    }
    function checkChars(rx) {
        return function(s) {
            var m = rx.exec(s);
            if (!m) return s;
            raise(null, errors.invalidCodeUnit, toHex(m[0].charCodeAt(0), 4).toUpperCase());
        };
    }
    var encodingModes = {
        // `pseudo-latin1` encoding mode: assume the input was decoded with the latin1 encoding
        // WARNING: latin1 does **NOT** mean cp1252 here like in the bone-headed WHATWG standard;
        // it means true ISO/IEC 8859-1 identity-mapped to Basic Latin and Latin-1 Supplement blocks
        'pseudo-latin1': {
            fixup: checkChars(/[^\x00-\xff]/),
            encodeByte: function(value) {
                if (value === null) return '';
                return String.fromCharCode(value);
            },
            encodeUTF8: function(codepoint) {
                return encodeUTF8(codepoint);
            }
        },
        // `x-user-defined` encoding mode: assume the input was decoded with the WHATWG `x-user-defined` encoding
        'x-user-defined': {
            fixup: checkChars(/[^\x00-\x7f\uf780-\uf7ff]/),
            encodeByte: function(value) {
                if (value === null) return '';
                if (value >= 0x80) return String.fromCharCode(value | 0xf700);
                return String.fromCharCode(value);
            },
            encodeUTF8: function(codepoint) {
                return encodeUTF8(codepoint, 0xf700);
            }
        },
        // `none` encoding mode: disregard intrepretation of string literals, leave identifiers as-is
        'none': {
            discardStrings: true,
            fixup: function(s) {
                return s;
            },
            encodeByte: function(value) {
                return '';
            },
            encodeUTF8: function(codepoint) {
                return '';
            }
        }
    };
    // The available tokens expressed as enum flags so they can be checked with
    // bitwise operations.
    var EOF = 1, StringLiteral = 2, Keyword = 4, Identifier = 8, NumericLiteral = 16, Punctuator = 32, BooleanLiteral = 64, NilLiteral = 128, VarargLiteral = 256;
    exports1.tokenTypes = {
        EOF: EOF,
        StringLiteral: StringLiteral,
        Keyword: Keyword,
        Identifier: Identifier,
        NumericLiteral: NumericLiteral,
        Punctuator: Punctuator,
        BooleanLiteral: BooleanLiteral,
        NilLiteral: NilLiteral,
        VarargLiteral: VarargLiteral
    };
    // As this parser is a bit different from luas own, the error messages
    // will be different in some situations.
    var errors = exports1.errors = {
        unexpected: 'unexpected %1 \'%2\' near \'%3\'',
        unexpectedEOF: 'unexpected symbol near \'<eof>\'',
        expected: '\'%1\' expected near \'%2\'',
        expectedToken: '%1 expected near \'%2\'',
        unfinishedString: 'unfinished string near \'%1\'',
        malformedNumber: 'malformed number near \'%1\'',
        decimalEscapeTooLarge: 'decimal escape too large near \'%1\'',
        invalidEscape: 'invalid escape sequence near \'%1\'',
        hexadecimalDigitExpected: 'hexadecimal digit expected near \'%1\'',
        braceExpected: 'missing \'%1\' near \'%2\'',
        tooLargeCodepoint: 'UTF-8 value too large near \'%1\'',
        unfinishedLongString: 'unfinished long string (starting at line %1) near \'%2\'',
        unfinishedLongComment: 'unfinished long comment (starting at line %1) near \'%2\'',
        ambiguousSyntax: 'ambiguous syntax (function call x new statement) near \'%1\'',
        noLoopToBreak: 'no loop to break near \'%1\'',
        labelAlreadyDefined: 'label \'%1\' already defined on line %2',
        labelNotVisible: 'no visible label \'%1\' for <goto>',
        gotoJumpInLocalScope: '<goto %1> jumps into the scope of local \'%2\'',
        cannotUseVararg: 'cannot use \'...\' outside a vararg function near \'%1\'',
        invalidCodeUnit: 'code unit U+%1 is not allowed in the current encoding mode'
    };
    // ### Abstract Syntax Tree
    //
    // The default AST structure is inspired by the Mozilla Parser API but can
    // easily be customized by overriding these functions.
    var ast = exports1.ast = {
        labelStatement: function(label) {
            return {
                type: 'LabelStatement',
                label: label
            };
        },
        breakStatement: function() {
            return {
                type: 'BreakStatement'
            };
        },
        gotoStatement: function(label) {
            return {
                type: 'GotoStatement',
                label: label
            };
        },
        returnStatement: function(args) {
            return {
                type: 'ReturnStatement',
                'arguments': args
            };
        },
        ifStatement: function(clauses) {
            return {
                type: 'IfStatement',
                clauses: clauses
            };
        },
        ifClause: function(condition, body) {
            return {
                type: 'IfClause',
                condition: condition,
                body: body
            };
        },
        elseifClause: function(condition, body) {
            return {
                type: 'ElseifClause',
                condition: condition,
                body: body
            };
        },
        elseClause: function(body) {
            return {
                type: 'ElseClause',
                body: body
            };
        },
        whileStatement: function(condition, body) {
            return {
                type: 'WhileStatement',
                condition: condition,
                body: body
            };
        },
        doStatement: function(body) {
            return {
                type: 'DoStatement',
                body: body
            };
        },
        repeatStatement: function(condition, body) {
            return {
                type: 'RepeatStatement',
                condition: condition,
                body: body
            };
        },
        localStatement: function(variables, init) {
            return {
                type: 'LocalStatement',
                variables: variables,
                init: init
            };
        },
        assignmentStatement: function(variables, init) {
            return {
                type: 'AssignmentStatement',
                variables: variables,
                init: init
            };
        },
        callStatement: function(expression) {
            return {
                type: 'CallStatement',
                expression: expression
            };
        },
        functionStatement: function(identifier, parameters, isLocal, body) {
            return {
                type: 'FunctionDeclaration',
                identifier: identifier,
                isLocal: isLocal,
                parameters: parameters,
                body: body
            };
        },
        forNumericStatement: function(variable, start, end, step, body) {
            return {
                type: 'ForNumericStatement',
                variable: variable,
                start: start,
                end: end,
                step: step,
                body: body
            };
        },
        forGenericStatement: function(variables, iterators, body) {
            return {
                type: 'ForGenericStatement',
                variables: variables,
                iterators: iterators,
                body: body
            };
        },
        chunk: function(body) {
            return {
                type: 'Chunk',
                body: body
            };
        },
        identifier: function(name) {
            return {
                type: 'Identifier',
                name: name
            };
        },
        literal: function(type, value, raw) {
            type = type === StringLiteral ? 'StringLiteral' : type === NumericLiteral ? 'NumericLiteral' : type === BooleanLiteral ? 'BooleanLiteral' : type === NilLiteral ? 'NilLiteral' : 'VarargLiteral';
            return {
                type: type,
                value: value,
                raw: raw
            };
        },
        tableKey: function(key, value) {
            return {
                type: 'TableKey',
                key: key,
                value: value
            };
        },
        tableKeyString: function(key, value) {
            return {
                type: 'TableKeyString',
                key: key,
                value: value
            };
        },
        tableValue: function(value) {
            return {
                type: 'TableValue',
                value: value
            };
        },
        tableConstructorExpression: function(fields) {
            return {
                type: 'TableConstructorExpression',
                fields: fields
            };
        },
        binaryExpression: function(operator, left, right) {
            var type = 'and' === operator || 'or' === operator ? 'LogicalExpression' : 'BinaryExpression';
            return {
                type: type,
                operator: operator,
                left: left,
                right: right
            };
        },
        unaryExpression: function(operator, argument) {
            return {
                type: 'UnaryExpression',
                operator: operator,
                argument: argument
            };
        },
        memberExpression: function(base, indexer, identifier) {
            return {
                type: 'MemberExpression',
                indexer: indexer,
                identifier: identifier,
                base: base
            };
        },
        indexExpression: function(base, index) {
            return {
                type: 'IndexExpression',
                base: base,
                index: index
            };
        },
        callExpression: function(base, args) {
            return {
                type: 'CallExpression',
                base: base,
                'arguments': args
            };
        },
        tableCallExpression: function(base, args) {
            return {
                type: 'TableCallExpression',
                base: base,
                'arguments': args
            };
        },
        stringCallExpression: function(base, argument) {
            return {
                type: 'StringCallExpression',
                base: base,
                argument: argument
            };
        },
        comment: function(value, raw) {
            return {
                type: 'Comment',
                value: value,
                raw: raw
            };
        }
    };
    // Wrap up the node object.
    function finishNode(node) {
        // Pop a `Marker` off the location-array and attach its location data.
        if (trackLocations) {
            var location = locations.pop();
            location.complete();
            location.bless(node);
        }
        if (options.onCreateNode) options.onCreateNode(node);
        return node;
    }
    // Helpers
    // -------
    var slice = Array.prototype.slice, toString = Object.prototype.toString;
    var indexOf = /* istanbul ignore next */ function(array, element) {
        for(var i = 0, length = array.length; i < length; ++i){
            if (array[i] === element) return i;
        }
        return -1;
    };
    /* istanbul ignore else */ if (Array.prototype.indexOf) indexOf = function(array, element) {
        return array.indexOf(element);
    };
    // Iterate through an array of objects and return the index of an object
    // with a matching property.
    function indexOfObject(array, property, element) {
        for(var i = 0, length = array.length; i < length; ++i){
            if (array[i][property] === element) return i;
        }
        return -1;
    }
    // A sprintf implementation using %index (beginning at 1) to input
    // arguments in the format string.
    //
    // Example:
    //
    //     // Unexpected function in token
    //     sprintf('Unexpected %2 in %1.', 'token', 'function');
    function sprintf(format) {
        var args = slice.call(arguments, 1);
        format = format.replace(/%(\d)/g, function(match, index) {
            return '' + args[index - 1] || /* istanbul ignore next */ '';
        });
        return format;
    }
    // Polyfill for `Object.assign`.
    var assign = /* istanbul ignore next */ function(dest) {
        var args = slice.call(arguments, 1), src, prop;
        for(var i = 0, length = args.length; i < length; ++i){
            src = args[i];
            for(prop in src)/* istanbul ignore else */ if (Object.prototype.hasOwnProperty.call(src, prop)) dest[prop] = src[prop];
        }
        return dest;
    };
    /* istanbul ignore else */ if (Object.assign) assign = Object.assign;
    // ### Error functions
    exports1.SyntaxError = SyntaxError;
    // XXX: Eliminate this function and change the error type to be different from SyntaxError.
    // This will unfortunately be a breaking change, because some downstream users depend
    // on the error thrown being an instance of SyntaxError. For example, the Ace editor:
    // <https://github.com/ajaxorg/ace/blob/4c7e5eb3f5d5ca9434847be51834a4e41661b852/lib/ace/mode/lua_worker.js#L55>
    function fixupError(e) {
        /* istanbul ignore if */ if (!Object.create) return e;
        return Object.create(e, {
            'line': {
                'writable': true,
                value: e.line
            },
            'index': {
                'writable': true,
                value: e.index
            },
            'column': {
                'writable': true,
                value: e.column
            }
        });
    }
    // #### Raise an exception.
    //
    // Raise an exception by passing a token, a string format and its paramters.
    //
    // The passed tokens location will automatically be added to the error
    // message if it exists, if not it will default to the lexers current
    // position.
    //
    // Example:
    //
    //     // [1:0] expected [ near (
    //     raise(token, "expected %1 near %2", '[', token.value);
    function raise(token) {
        var message = sprintf.apply(null, slice.call(arguments, 1)), error, col;
        if (token === null || typeof token.line === 'undefined') {
            col = index - lineStart + 1;
            error = fixupError(new SyntaxError(sprintf('[%1:%2] %3', line, col, message)));
            error.index = index;
            error.line = line;
            error.column = col;
        } else {
            col = token.range[0] - token.lineStart;
            error = fixupError(new SyntaxError(sprintf('[%1:%2] %3', token.line, col, message)));
            error.line = token.line;
            error.index = token.range[0];
            error.column = col;
        }
        throw error;
    }
    function tokenValue(token) {
        var raw = input.slice(token.range[0], token.range[1]);
        if (raw) return raw;
        return token.value;
    }
    // #### Raise an unexpected token error.
    //
    // Example:
    //
    //     // expected <name> near '0'
    //     raiseUnexpectedToken('<name>', token);
    function raiseUnexpectedToken(type, token) {
        raise(token, errors.expectedToken, type, tokenValue(token));
    }
    // #### Raise a general unexpected error
    //
    // Usage should pass either a token object or a symbol string which was
    // expected. We can also specify a nearby token such as <eof>, this will
    // default to the currently active token.
    //
    // Example:
    //
    //     // Unexpected symbol 'end' near '<eof>'
    //     unexpected(token);
    //
    // If there's no token in the buffer it means we have reached <eof>.
    function unexpected(found) {
        var near = tokenValue(lookahead);
        if ('undefined' !== typeof found.type) {
            var type;
            switch(found.type){
                case StringLiteral:
                    type = 'string';
                    break;
                case Keyword:
                    type = 'keyword';
                    break;
                case Identifier:
                    type = 'identifier';
                    break;
                case NumericLiteral:
                    type = 'number';
                    break;
                case Punctuator:
                    type = 'symbol';
                    break;
                case BooleanLiteral:
                    type = 'boolean';
                    break;
                case NilLiteral:
                    return raise(found, errors.unexpected, 'symbol', 'nil', near);
                case EOF:
                    return raise(found, errors.unexpectedEOF);
            }
            return raise(found, errors.unexpected, type, tokenValue(found), near);
        }
        return raise(found, errors.unexpected, 'symbol', found, near);
    }
    // Lexer
    // -----
    //
    // The lexer, or the tokenizer reads the input string character by character
    // and derives a token left-right. To be as efficient as possible the lexer
    // prioritizes the common cases such as identifiers. It also works with
    // character codes instead of characters as string comparisons was the
    // biggest bottleneck of the parser.
    //
    // If `options.comments` is enabled, all comments encountered will be stored
    // in an array which later will be appended to the chunk object. If disabled,
    // they will simply be disregarded.
    //
    // When the lexer has derived a valid token, it will be returned as an object
    // containing its value and as well as its position in the input string (this
    // is always enabled to provide proper debug messages).
    //
    // `lex()` starts lexing and returns the following token in the stream.
    var index, token, previousToken, lookahead, comments, tokenStart, line, lineStart;
    exports1.lex = lex;
    function lex() {
        skipWhiteSpace();
        // Skip comments beginning with --
        while(45 === input.charCodeAt(index) && 45 === input.charCodeAt(index + 1)){
            scanComment();
            skipWhiteSpace();
        }
        if (index >= length) return {
            type: EOF,
            value: '<eof>',
            line: line,
            lineStart: lineStart,
            range: [
                index,
                index
            ]
        };
        var charCode = input.charCodeAt(index), next = input.charCodeAt(index + 1);
        // Memorize the range index where the token begins.
        tokenStart = index;
        if (isIdentifierStart(charCode)) return scanIdentifierOrKeyword();
        switch(charCode){
            case 39:
            case 34:
                return scanStringLiteral();
            case 48:
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
                return scanNumericLiteral();
            case 46:
                // If the dot is followed by a digit it's a float.
                if (isDecDigit(next)) return scanNumericLiteral();
                if (46 === next) {
                    if (46 === input.charCodeAt(index + 2)) return scanVarargLiteral();
                    return scanPunctuator('..');
                }
                return scanPunctuator('.');
            case 61:
                if (61 === next) return scanPunctuator('==');
                return scanPunctuator('=');
            case 62:
                if (features.bitwiseOperators) {
                    if (62 === next) return scanPunctuator('>>');
                }
                if (61 === next) return scanPunctuator('>=');
                return scanPunctuator('>');
            case 60:
                if (features.bitwiseOperators) {
                    if (60 === next) return scanPunctuator('<<');
                }
                if (61 === next) return scanPunctuator('<=');
                return scanPunctuator('<');
            case 126:
                if (61 === next) return scanPunctuator('~=');
                if (!features.bitwiseOperators) break;
                return scanPunctuator('~');
            case 58:
                if (features.labels) {
                    if (58 === next) return scanPunctuator('::');
                }
                return scanPunctuator(':');
            case 91:
                // Check for a multiline string, they begin with [= or [[
                if (91 === next || 61 === next) return scanLongStringLiteral();
                return scanPunctuator('[');
            case 47:
                // Check for integer division op (//)
                if (features.integerDivision) {
                    if (47 === next) return scanPunctuator('//');
                }
                return scanPunctuator('/');
            case 38:
            case 124:
                if (!features.bitwiseOperators) break;
            /* fall through */ case 42:
            case 94:
            case 37:
            case 44:
            case 123:
            case 125:
            case 93:
            case 40:
            case 41:
            case 59:
            case 35:
            case 45:
            case 43:
                return scanPunctuator(input.charAt(index));
        }
        return unexpected(input.charAt(index));
    }
    // Whitespace has no semantic meaning in lua so simply skip ahead while
    // tracking the encounted newlines. Any kind of eol sequence is counted as a
    // single line.
    function consumeEOL() {
        var charCode = input.charCodeAt(index), peekCharCode = input.charCodeAt(index + 1);
        if (isLineTerminator(charCode)) {
            // Count \n\r and \r\n as one newline.
            if (10 === charCode && 13 === peekCharCode) ++index;
            if (13 === charCode && 10 === peekCharCode) ++index;
            ++line;
            lineStart = ++index;
            return true;
        }
        return false;
    }
    function skipWhiteSpace() {
        while(index < length){
            var charCode = input.charCodeAt(index);
            if (isWhiteSpace(charCode)) ++index;
            else if (!consumeEOL()) break;
        }
    }
    // Identifiers, keywords, booleans and nil all look the same syntax wise. We
    // simply go through them one by one and defaulting to an identifier if no
    // previous case matched.
    function scanIdentifierOrKeyword() {
        var value, type;
        // Slicing the input string is prefered before string concatenation in a
        // loop for performance reasons.
        while(isIdentifierPart(input.charCodeAt(++index)));
        value = encodingMode.fixup(input.slice(tokenStart, index));
        // Decide on the token type and possibly cast the value.
        if (isKeyword(value)) type = Keyword;
        else if ('true' === value || 'false' === value) {
            type = BooleanLiteral;
            value = 'true' === value;
        } else if ('nil' === value) {
            type = NilLiteral;
            value = null;
        } else type = Identifier;
        return {
            type: type,
            value: value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Once a punctuator reaches this function it should already have been
    // validated so we simply return it as a token.
    function scanPunctuator(value) {
        index += value.length;
        return {
            type: Punctuator,
            value: value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // A vararg literal consists of three dots.
    function scanVarargLiteral() {
        index += 3;
        return {
            type: VarargLiteral,
            value: '...',
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Find the string literal by matching the delimiter marks used.
    function scanStringLiteral() {
        var delimiter = input.charCodeAt(index++), beginLine = line, beginLineStart = lineStart, stringStart = index, string = encodingMode.discardStrings ? null : '', charCode;
        for(;;){
            charCode = input.charCodeAt(index++);
            if (delimiter === charCode) break;
            // EOF or `\n` terminates a string literal. If we haven't found the
            // ending delimiter by now, raise an exception.
            if (index > length || isLineTerminator(charCode)) {
                string += input.slice(stringStart, index - 1);
                raise(null, errors.unfinishedString, input.slice(tokenStart, index - 1));
            }
            if (92 === charCode) {
                if (!encodingMode.discardStrings) {
                    var beforeEscape = input.slice(stringStart, index - 1);
                    string += encodingMode.fixup(beforeEscape);
                }
                var escapeValue = readEscapeSequence();
                if (!encodingMode.discardStrings) string += escapeValue;
                stringStart = index;
            }
        }
        if (!encodingMode.discardStrings) {
            string += encodingMode.encodeByte(null);
            string += encodingMode.fixup(input.slice(stringStart, index - 1));
        }
        return {
            type: StringLiteral,
            value: string,
            line: beginLine,
            lineStart: beginLineStart,
            lastLine: line,
            lastLineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Expect a multiline string literal and return it as a regular string
    // literal, if it doesn't validate into a valid multiline string, throw an
    // exception.
    function scanLongStringLiteral() {
        var beginLine = line, beginLineStart = lineStart, string = readLongString(false);
        // Fail if it's not a multiline literal.
        if (false === string) raise(token, errors.expected, '[', tokenValue(token));
        return {
            type: StringLiteral,
            value: encodingMode.discardStrings ? null : encodingMode.fixup(string),
            line: beginLine,
            lineStart: beginLineStart,
            lastLine: line,
            lastLineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Numeric literals will be returned as floating-point numbers instead of
    // strings. The raw value should be retrieved from slicing the input string
    // later on in the process.
    //
    // If a hexadecimal number is encountered, it will be converted.
    function scanNumericLiteral() {
        var character = input.charAt(index), next = input.charAt(index + 1);
        var literal = '0' === character && 'xX'.indexOf(next || null) >= 0 ? readHexLiteral() : readDecLiteral();
        var foundImaginaryUnit = readImaginaryUnitSuffix(), foundInt64Suffix = readInt64Suffix();
        if (foundInt64Suffix && (foundImaginaryUnit || literal.hasFractionPart)) raise(null, errors.malformedNumber, input.slice(tokenStart, index));
        return {
            type: NumericLiteral,
            value: literal.value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    function readImaginaryUnitSuffix() {
        if (!features.imaginaryNumbers) return;
        // Imaginary unit number suffix is optional.
        // See http://luajit.org/ext_ffi_api.html#literals
        if ('iI'.indexOf(input.charAt(index) || null) >= 0) {
            ++index;
            return true;
        } else return false;
    }
    function readInt64Suffix() {
        if (!features.integerSuffixes) return;
        // Int64/uint64 number suffix is optional.
        // See http://luajit.org/ext_ffi_api.html#literals
        if ('uU'.indexOf(input.charAt(index) || null) >= 0) {
            ++index;
            if ('lL'.indexOf(input.charAt(index) || null) >= 0) {
                ++index;
                if ('lL'.indexOf(input.charAt(index) || null) >= 0) {
                    ++index;
                    return 'ULL';
                } else // UL but no L
                raise(null, errors.malformedNumber, input.slice(tokenStart, index));
            } else // U but no L
            raise(null, errors.malformedNumber, input.slice(tokenStart, index));
        } else if ('lL'.indexOf(input.charAt(index) || null) >= 0) {
            ++index;
            if ('lL'.indexOf(input.charAt(index) || null) >= 0) {
                ++index;
                return 'LL';
            } else // First L but no second L
            raise(null, errors.malformedNumber, input.slice(tokenStart, index));
        }
    }
    // Lua hexadecimals have an optional fraction part and an optional binary
    // exoponent part. These are not included in JavaScript so we will compute
    // all three parts separately and then sum them up at the end of the function
    // with the following algorithm.
    //
    //     Digit := toDec(digit)
    //     Fraction := toDec(fraction) / 16 ^ fractionCount
    //     BinaryExp := 2 ^ binaryExp
    //     Number := ( Digit + Fraction ) * BinaryExp
    function readHexLiteral() {
        var fraction = 0 // defaults to 0 as it gets summed
        , binaryExponent = 1 // defaults to 1 as it gets multiplied
        , binarySign = 1 // positive
        , digit, fractionStart, exponentStart, digitStart;
        digitStart = index += 2; // Skip 0x part
        // A minimum of one hex digit is required.
        if (!isHexDigit(input.charCodeAt(index))) raise(null, errors.malformedNumber, input.slice(tokenStart, index));
        while(isHexDigit(input.charCodeAt(index)))++index;
        // Convert the hexadecimal digit to base 10.
        digit = parseInt(input.slice(digitStart, index), 16);
        // Fraction part is optional.
        var foundFraction = false;
        if ('.' === input.charAt(index)) {
            foundFraction = true;
            fractionStart = ++index;
            while(isHexDigit(input.charCodeAt(index)))++index;
            fraction = input.slice(fractionStart, index);
            // Empty fraction parts should default to 0, others should be converted
            // 0.x form so we can use summation at the end.
            fraction = fractionStart === index ? 0 : parseInt(fraction, 16) / Math.pow(16, index - fractionStart);
        }
        // Binary exponents are optional
        var foundBinaryExponent = false;
        if ('pP'.indexOf(input.charAt(index) || null) >= 0) {
            foundBinaryExponent = true;
            ++index;
            // Sign part is optional and defaults to 1 (positive).
            if ('+-'.indexOf(input.charAt(index) || null) >= 0) binarySign = '+' === input.charAt(index++) ? 1 : -1;
            exponentStart = index;
            // The binary exponent sign requires a decimal digit.
            if (!isDecDigit(input.charCodeAt(index))) raise(null, errors.malformedNumber, input.slice(tokenStart, index));
            while(isDecDigit(input.charCodeAt(index)))++index;
            binaryExponent = input.slice(exponentStart, index);
            // Calculate the binary exponent of the number.
            binaryExponent = Math.pow(2, binaryExponent * binarySign);
        }
        return {
            value: (digit + fraction) * binaryExponent,
            hasFractionPart: foundFraction || foundBinaryExponent
        };
    }
    // Decimal numbers are exactly the same in Lua and in JavaScript, because of
    // this we check where the token ends and then parse it with native
    // functions.
    function readDecLiteral() {
        while(isDecDigit(input.charCodeAt(index)))++index;
        // Fraction part is optional
        var foundFraction = false;
        if ('.' === input.charAt(index)) {
            foundFraction = true;
            ++index;
            // Fraction part defaults to 0
            while(isDecDigit(input.charCodeAt(index)))++index;
        }
        // Exponent part is optional.
        var foundExponent = false;
        if ('eE'.indexOf(input.charAt(index) || null) >= 0) {
            foundExponent = true;
            ++index;
            // Sign part is optional.
            if ('+-'.indexOf(input.charAt(index) || null) >= 0) ++index;
            // An exponent is required to contain at least one decimal digit.
            if (!isDecDigit(input.charCodeAt(index))) raise(null, errors.malformedNumber, input.slice(tokenStart, index));
            while(isDecDigit(input.charCodeAt(index)))++index;
        }
        return {
            value: parseFloat(input.slice(tokenStart, index)),
            hasFractionPart: foundFraction || foundExponent
        };
    }
    function readUnicodeEscapeSequence() {
        var sequenceStart = index++;
        if (input.charAt(index++) !== '{') raise(null, errors.braceExpected, '{', '\\' + input.slice(sequenceStart, index));
        if (!isHexDigit(input.charCodeAt(index))) raise(null, errors.hexadecimalDigitExpected, '\\' + input.slice(sequenceStart, index));
        while(input.charCodeAt(index) === 0x30)++index;
        var escStart = index;
        while(isHexDigit(input.charCodeAt(index))){
            ++index;
            if (index - escStart > 6) raise(null, errors.tooLargeCodepoint, '\\' + input.slice(sequenceStart, index));
        }
        var b = input.charAt(index++);
        if (b !== '}') {
            if (b === '"' || b === "'") raise(null, errors.braceExpected, '}', '\\' + input.slice(sequenceStart, index--));
            else raise(null, errors.hexadecimalDigitExpected, '\\' + input.slice(sequenceStart, index));
        }
        var codepoint = parseInt(input.slice(escStart, index - 1) || '0', 16);
        var frag = '\\' + input.slice(sequenceStart, index);
        if (codepoint > 0x10ffff) raise(null, errors.tooLargeCodepoint, frag);
        return encodingMode.encodeUTF8(codepoint, frag);
    }
    // Translate escape sequences to the actual characters.
    function readEscapeSequence() {
        var sequenceStart = index;
        switch(input.charAt(index)){
            // Lua allow the following escape sequences.
            case 'a':
                ++index;
                return '\x07';
            case 'n':
                ++index;
                return '\n';
            case 'r':
                ++index;
                return '\r';
            case 't':
                ++index;
                return '\t';
            case 'v':
                ++index;
                return '\x0b';
            case 'b':
                ++index;
                return '\b';
            case 'f':
                ++index;
                return '\f';
            // Backslash at the end of the line. We treat all line endings as equivalent,
            // and as representing the [LF] character (code 10). Lua 5.1 through 5.3
            // have been verified to behave the same way.
            case '\r':
            case '\n':
                consumeEOL();
                return '\n';
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                // \ddd, where ddd is a sequence of up to three decimal digits.
                while(isDecDigit(input.charCodeAt(index)) && index - sequenceStart < 3)++index;
                var frag = input.slice(sequenceStart, index);
                var ddd = parseInt(frag, 10);
                if (ddd > 255) raise(null, errors.decimalEscapeTooLarge, '\\' + ddd);
                return encodingMode.encodeByte(ddd, '\\' + frag);
            case 'z':
                if (features.skipWhitespaceEscape) {
                    ++index;
                    skipWhiteSpace();
                    return '';
                }
                break;
            case 'x':
                if (features.hexEscapes) {
                    // \xXX, where XX is a sequence of exactly two hexadecimal digits
                    if (isHexDigit(input.charCodeAt(index + 1)) && isHexDigit(input.charCodeAt(index + 2))) {
                        index += 3;
                        return encodingMode.encodeByte(parseInt(input.slice(sequenceStart + 1, index), 16), '\\' + input.slice(sequenceStart, index));
                    }
                    raise(null, errors.hexadecimalDigitExpected, '\\' + input.slice(sequenceStart, index + 2));
                }
                break;
            case 'u':
                if (features.unicodeEscapes) return readUnicodeEscapeSequence();
                break;
            case '\\':
            case '"':
            case "'":
                return input.charAt(index++);
        }
        if (features.strictEscapes) raise(null, errors.invalidEscape, '\\' + input.slice(sequenceStart, index + 1));
        return input.charAt(index++);
    }
    // Comments begin with -- after which it will be decided if they are
    // multiline comments or not.
    //
    // The multiline functionality works the exact same way as with string
    // literals so we reuse the functionality.
    function scanComment() {
        tokenStart = index;
        index += 2; // --
        var character = input.charAt(index), content = '', isLong = false, commentStart = index, lineStartComment = lineStart, lineComment = line;
        if ('[' === character) {
            content = readLongString(true);
            // This wasn't a multiline comment after all.
            if (false === content) content = character;
            else isLong = true;
        }
        // Scan until next line as long as it's not a multiline comment.
        if (!isLong) {
            while(index < length){
                if (isLineTerminator(input.charCodeAt(index))) break;
                ++index;
            }
            if (options.comments) content = input.slice(commentStart, index);
        }
        if (options.comments) {
            var node = ast.comment(content, input.slice(tokenStart, index));
            // `Marker`s depend on tokens available in the parser and as comments are
            // intercepted in the lexer all location data is set manually.
            if (options.locations) node.loc = {
                start: {
                    line: lineComment,
                    column: tokenStart - lineStartComment
                },
                end: {
                    line: line,
                    column: index - lineStart
                }
            };
            if (options.ranges) node.range = [
                tokenStart,
                index
            ];
            if (options.onCreateNode) options.onCreateNode(node);
            comments.push(node);
        }
    }
    // Read a multiline string by calculating the depth of `=` characters and
    // then appending until an equal depth is found.
    function readLongString(isComment) {
        var level = 0, content = '', terminator = false, character, stringStart, firstLine = line;
        ++index; // [
        // Calculate the depth of the comment.
        while('=' === input.charAt(index + level))++level;
        // Exit, this is not a long string afterall.
        if ('[' !== input.charAt(index + level)) return false;
        index += level + 1;
        // If the first character is a newline, ignore it and begin on next line.
        if (isLineTerminator(input.charCodeAt(index))) consumeEOL();
        stringStart = index;
        while(index < length){
            // To keep track of line numbers run the `consumeEOL()` which increments
            // its counter.
            while(isLineTerminator(input.charCodeAt(index)))consumeEOL();
            character = input.charAt(index++);
            // Once the delimiter is found, iterate through the depth count and see
            // if it matches.
            if (']' === character) {
                terminator = true;
                for(var i = 0; i < level; ++i)if ('=' !== input.charAt(index + i)) terminator = false;
                if (']' !== input.charAt(index + level)) terminator = false;
            }
            // We reached the end of the multiline string. Get out now.
            if (terminator) {
                content += input.slice(stringStart, index - 1);
                index += level + 1;
                return content;
            }
        }
        raise(null, isComment ? errors.unfinishedLongComment : errors.unfinishedLongString, firstLine, '<eof>');
    }
    // ## Lex functions and helpers.
    // Read the next token.
    //
    // This is actually done by setting the current token to the lookahead and
    // reading in the new lookahead token.
    function next() {
        previousToken = token;
        token = lookahead;
        lookahead = lex();
    }
    // Consume a token if its value matches. Once consumed or not, return the
    // success of the operation.
    function consume(value) {
        if (value === token.value) {
            next();
            return true;
        }
        return false;
    }
    // Expect the next token value to match. If not, throw an exception.
    function expect(value) {
        if (value === token.value) next();
        else raise(token, errors.expected, value, tokenValue(token));
    }
    // ### Validation functions
    function isWhiteSpace(charCode) {
        return 9 === charCode || 32 === charCode || 0xB === charCode || 0xC === charCode;
    }
    function isLineTerminator(charCode) {
        return 10 === charCode || 13 === charCode;
    }
    function isDecDigit(charCode) {
        return charCode >= 48 && charCode <= 57;
    }
    function isHexDigit(charCode) {
        return charCode >= 48 && charCode <= 57 || charCode >= 97 && charCode <= 102 || charCode >= 65 && charCode <= 70;
    }
    // From [Lua 5.2](http://www.lua.org/manual/5.2/manual.html#8.1) onwards
    // identifiers cannot use 'locale-dependent' letters (i.e. dependent on the C locale).
    // On the other hand, LuaJIT allows arbitrary octets ≥ 128 in identifiers.
    function isIdentifierStart(charCode) {
        if (charCode >= 65 && charCode <= 90 || charCode >= 97 && charCode <= 122 || 95 === charCode) return true;
        if (features.extendedIdentifiers && charCode >= 128) return true;
        return false;
    }
    function isIdentifierPart(charCode) {
        if (charCode >= 65 && charCode <= 90 || charCode >= 97 && charCode <= 122 || 95 === charCode || charCode >= 48 && charCode <= 57) return true;
        if (features.extendedIdentifiers && charCode >= 128) return true;
        return false;
    }
    // [3.1 Lexical Conventions](http://www.lua.org/manual/5.2/manual.html#3.1)
    //
    // `true`, `false` and `nil` will not be considered keywords, but literals.
    function isKeyword(id) {
        switch(id.length){
            case 2:
                return 'do' === id || 'if' === id || 'in' === id || 'or' === id;
            case 3:
                return 'and' === id || 'end' === id || 'for' === id || 'not' === id;
            case 4:
                if ('else' === id || 'then' === id) return true;
                if (features.labels && !features.contextualGoto) return 'goto' === id;
                return false;
            case 5:
                return 'break' === id || 'local' === id || 'until' === id || 'while' === id;
            case 6:
                return 'elseif' === id || 'repeat' === id || 'return' === id;
            case 8:
                return 'function' === id;
        }
        return false;
    }
    function isUnary(token) {
        if (Punctuator === token.type) return '#-~'.indexOf(token.value) >= 0;
        if (Keyword === token.type) return 'not' === token.value;
        return false;
    }
    // Check if the token syntactically closes a block.
    function isBlockFollow(token) {
        if (EOF === token.type) return true;
        if (Keyword !== token.type) return false;
        switch(token.value){
            case 'else':
            case 'elseif':
            case 'end':
            case 'until':
                return true;
            default:
                return false;
        }
    }
    // Scope
    // -----
    // Store each block scope as a an array of identifier names. Each scope is
    // stored in an FILO-array.
    var scopes, scopeDepth, globals;
    // Create a new scope inheriting all declarations from the previous scope.
    function createScope() {
        var scope = scopes[scopeDepth++].slice();
        scopes.push(scope);
        if (options.onCreateScope) options.onCreateScope();
    }
    // Exit and remove the current scope.
    function destroyScope() {
        var scope = scopes.pop();
        --scopeDepth;
        if (options.onDestroyScope) options.onDestroyScope();
    }
    // Add identifier name to the current scope if it doesnt already exist.
    function scopeIdentifierName(name) {
        if (options.onLocalDeclaration) options.onLocalDeclaration(name);
        if (-1 !== indexOf(scopes[scopeDepth], name)) return;
        scopes[scopeDepth].push(name);
    }
    // Add identifier to the current scope
    function scopeIdentifier(node) {
        scopeIdentifierName(node.name);
        attachScope(node, true);
    }
    // Attach scope information to node. If the node is global, store it in the
    // globals array so we can return the information to the user.
    function attachScope(node, isLocal) {
        if (!isLocal && -1 === indexOfObject(globals, 'name', node.name)) globals.push(node);
        node.isLocal = isLocal;
    }
    // Is the identifier name available in this scope.
    function scopeHasName(name) {
        return -1 !== indexOf(scopes[scopeDepth], name);
    }
    // Location tracking
    // -----------------
    //
    // Locations are stored in FILO-array as a `Marker` object consisting of both
    // `loc` and `range` data. Once a `Marker` is popped off the list an end
    // location is added and the data is attached to a syntax node.
    var locations = [], trackLocations;
    function createLocationMarker() {
        return new Marker(token);
    }
    function Marker(token) {
        if (options.locations) this.loc = {
            start: {
                line: token.line,
                column: token.range[0] - token.lineStart
            },
            end: {
                line: 0,
                column: 0
            }
        };
        if (options.ranges) this.range = [
            token.range[0],
            0
        ];
    }
    // Complete the location data stored in the `Marker` by adding the location
    // of the *previous token* as an end location.
    Marker.prototype.complete = function() {
        if (options.locations) {
            this.loc.end.line = previousToken.lastLine || previousToken.line;
            this.loc.end.column = previousToken.range[1] - (previousToken.lastLineStart || previousToken.lineStart);
        }
        if (options.ranges) this.range[1] = previousToken.range[1];
    };
    Marker.prototype.bless = function(node) {
        if (this.loc) {
            var loc = this.loc;
            node.loc = {
                start: {
                    line: loc.start.line,
                    column: loc.start.column
                },
                end: {
                    line: loc.end.line,
                    column: loc.end.column
                }
            };
        }
        if (this.range) node.range = [
            this.range[0],
            this.range[1]
        ];
    };
    // Create a new `Marker` and add it to the FILO-array.
    function markLocation() {
        if (trackLocations) locations.push(createLocationMarker());
    }
    // Push an arbitrary `Marker` object onto the FILO-array.
    function pushLocation(marker) {
        if (trackLocations) locations.push(marker);
    }
    // Control flow tracking
    // ---------------------
    // A context object that validates loop breaks and `goto`-based control flow.
    function FullFlowContext() {
        this.scopes = [];
        this.pendingGotos = [];
    }
    FullFlowContext.prototype.isInLoop = function() {
        var i = this.scopes.length;
        while(i-- > 0){
            if (this.scopes[i].isLoop) return true;
        }
        return false;
    };
    FullFlowContext.prototype.pushScope = function(isLoop) {
        var scope = {
            labels: {},
            locals: [],
            deferredGotos: [],
            isLoop: !!isLoop
        };
        this.scopes.push(scope);
    };
    FullFlowContext.prototype.popScope = function() {
        for(var i = 0; i < this.pendingGotos.length; ++i){
            var theGoto = this.pendingGotos[i];
            if (theGoto.maxDepth >= this.scopes.length) {
                if (--theGoto.maxDepth <= 0) raise(theGoto.token, errors.labelNotVisible, theGoto.target);
            }
        }
        this.scopes.pop();
    };
    FullFlowContext.prototype.addGoto = function(target, token) {
        var localCounts = [];
        for(var i = 0; i < this.scopes.length; ++i){
            var scope = this.scopes[i];
            localCounts.push(scope.locals.length);
            if (Object.prototype.hasOwnProperty.call(scope.labels, target)) return;
        }
        this.pendingGotos.push({
            maxDepth: this.scopes.length,
            target: target,
            token: token,
            localCounts: localCounts
        });
    };
    FullFlowContext.prototype.addLabel = function(name, token) {
        var scope = this.currentScope();
        if (Object.prototype.hasOwnProperty.call(scope.labels, name)) raise(token, errors.labelAlreadyDefined, name, scope.labels[name].line);
        else {
            var newGotos = [];
            for(var i = 0; i < this.pendingGotos.length; ++i){
                var theGoto = this.pendingGotos[i];
                if (theGoto.maxDepth >= this.scopes.length && theGoto.target === name) {
                    if (theGoto.localCounts[this.scopes.length - 1] < scope.locals.length) scope.deferredGotos.push(theGoto);
                    continue;
                }
                newGotos.push(theGoto);
            }
            this.pendingGotos = newGotos;
        }
        scope.labels[name] = {
            localCount: scope.locals.length,
            line: token.line
        };
    };
    FullFlowContext.prototype.addLocal = function(name, token) {
        this.currentScope().locals.push({
            name: name,
            token: token
        });
    };
    FullFlowContext.prototype.currentScope = function() {
        return this.scopes[this.scopes.length - 1];
    };
    FullFlowContext.prototype.raiseDeferredErrors = function() {
        var scope = this.currentScope();
        var bads = scope.deferredGotos;
        for(var i = 0; i < bads.length; ++i){
            var theGoto = bads[i];
            raise(theGoto.token, errors.gotoJumpInLocalScope, theGoto.target, scope.locals[theGoto.localCounts[this.scopes.length - 1]].name);
        }
    // Would be dead code currently, but may be useful later
    // if (bads.length)
    //   scope.deferredGotos = [];
    };
    // Simplified context that only checks the validity of loop breaks.
    function LoopFlowContext() {
        this.level = 0;
        this.loopLevels = [];
    }
    LoopFlowContext.prototype.isInLoop = function() {
        return !!this.loopLevels.length;
    };
    LoopFlowContext.prototype.pushScope = function(isLoop) {
        ++this.level;
        if (isLoop) this.loopLevels.push(this.level);
    };
    LoopFlowContext.prototype.popScope = function() {
        var levels = this.loopLevels;
        var levlen = levels.length;
        if (levlen) {
            if (levels[levlen - 1] === this.level) levels.pop();
        }
        --this.level;
    };
    LoopFlowContext.prototype.addGoto = LoopFlowContext.prototype.addLabel = /* istanbul ignore next */ function() {
        throw new Error('This should never happen');
    };
    LoopFlowContext.prototype.addLocal = LoopFlowContext.prototype.raiseDeferredErrors = function() {};
    function makeFlowContext() {
        return features.labels ? new FullFlowContext() : new LoopFlowContext();
    }
    // Parse functions
    // ---------------
    // Chunk is the main program object. Syntactically it's the same as a block.
    //
    //     chunk ::= block
    function parseChunk() {
        next();
        markLocation();
        if (options.scope) createScope();
        var flowContext = makeFlowContext();
        flowContext.allowVararg = true;
        flowContext.pushScope();
        var body = parseBlock(flowContext);
        flowContext.popScope();
        if (options.scope) destroyScope();
        if (EOF !== token.type) unexpected(token);
        // If the body is empty no previousToken exists when finishNode runs.
        if (trackLocations && !body.length) previousToken = token;
        return finishNode(ast.chunk(body));
    }
    // A block contains a list of statements with an optional return statement
    // as its last statement.
    //
    //     block ::= {stat} [retstat]
    function parseBlock(flowContext) {
        var block = [], statement;
        while(!isBlockFollow(token)){
            // Return has to be the last statement in a block.
            // Likewise 'break' in Lua older than 5.2
            if ('return' === token.value || !features.relaxedBreak && 'break' === token.value) {
                block.push(parseStatement(flowContext));
                break;
            }
            statement = parseStatement(flowContext);
            consume(';');
            // Statements are only added if they are returned, this allows us to
            // ignore some statements, such as EmptyStatement.
            if (statement) block.push(statement);
        }
        // Doesn't really need an ast node
        return block;
    }
    // There are two types of statements, simple and compound.
    //
    //     statement ::= break | goto | do | while | repeat | return
    //          | if | for | function | local | label | assignment
    //          | functioncall | ';'
    function parseStatement(flowContext) {
        markLocation();
        if (Punctuator === token.type) {
            if (consume('::')) return parseLabelStatement(flowContext);
        }
        // When a `;` is encounted, simply eat it without storing it.
        if (features.emptyStatement) {
            if (consume(';')) {
                if (trackLocations) locations.pop();
                return;
            }
        }
        flowContext.raiseDeferredErrors();
        if (Keyword === token.type) switch(token.value){
            case 'local':
                next();
                return parseLocalStatement(flowContext);
            case 'if':
                next();
                return parseIfStatement(flowContext);
            case 'return':
                next();
                return parseReturnStatement(flowContext);
            case 'function':
                next();
                var name = parseFunctionName();
                return parseFunctionDeclaration(name);
            case 'while':
                next();
                return parseWhileStatement(flowContext);
            case 'for':
                next();
                return parseForStatement(flowContext);
            case 'repeat':
                next();
                return parseRepeatStatement(flowContext);
            case 'break':
                next();
                if (!flowContext.isInLoop()) raise(token, errors.noLoopToBreak, token.value);
                return parseBreakStatement();
            case 'do':
                next();
                return parseDoStatement(flowContext);
            case 'goto':
                next();
                return parseGotoStatement(flowContext);
        }
        if (features.contextualGoto && token.type === Identifier && token.value === 'goto' && lookahead.type === Identifier && lookahead.value !== 'goto') {
            next();
            return parseGotoStatement(flowContext);
        }
        // Assignments memorizes the location and pushes it manually for wrapper nodes.
        if (trackLocations) locations.pop();
        return parseAssignmentOrCallStatement(flowContext);
    }
    // ## Statements
    //     label ::= '::' Name '::'
    function parseLabelStatement(flowContext) {
        var nameToken = token, label = parseIdentifier();
        if (options.scope) {
            scopeIdentifierName('::' + nameToken.value + '::');
            attachScope(label, true);
        }
        expect('::');
        flowContext.addLabel(nameToken.value, nameToken);
        return finishNode(ast.labelStatement(label));
    }
    //     break ::= 'break'
    function parseBreakStatement() {
        return finishNode(ast.breakStatement());
    }
    //     goto ::= 'goto' Name
    function parseGotoStatement(flowContext) {
        var name = token.value, gotoToken = previousToken, label = parseIdentifier();
        flowContext.addGoto(name, gotoToken);
        return finishNode(ast.gotoStatement(label));
    }
    //     do ::= 'do' block 'end'
    function parseDoStatement(flowContext) {
        if (options.scope) createScope();
        flowContext.pushScope();
        var body = parseBlock(flowContext);
        flowContext.popScope();
        if (options.scope) destroyScope();
        expect('end');
        return finishNode(ast.doStatement(body));
    }
    //     while ::= 'while' exp 'do' block 'end'
    function parseWhileStatement(flowContext) {
        var condition = parseExpectedExpression(flowContext);
        expect('do');
        if (options.scope) createScope();
        flowContext.pushScope(true);
        var body = parseBlock(flowContext);
        flowContext.popScope();
        if (options.scope) destroyScope();
        expect('end');
        return finishNode(ast.whileStatement(condition, body));
    }
    //     repeat ::= 'repeat' block 'until' exp
    function parseRepeatStatement(flowContext) {
        if (options.scope) createScope();
        flowContext.pushScope(true);
        var body = parseBlock(flowContext);
        expect('until');
        flowContext.raiseDeferredErrors();
        var condition = parseExpectedExpression(flowContext);
        flowContext.popScope();
        if (options.scope) destroyScope();
        return finishNode(ast.repeatStatement(condition, body));
    }
    //     retstat ::= 'return' [exp {',' exp}] [';']
    function parseReturnStatement(flowContext) {
        var expressions = [];
        if ('end' !== token.value) {
            var expression = parseExpression(flowContext);
            if (null != expression) expressions.push(expression);
            while(consume(',')){
                expression = parseExpectedExpression(flowContext);
                expressions.push(expression);
            }
            consume(';'); // grammar tells us ; is optional here.
        }
        return finishNode(ast.returnStatement(expressions));
    }
    //     if ::= 'if' exp 'then' block {elif} ['else' block] 'end'
    //     elif ::= 'elseif' exp 'then' block
    function parseIfStatement(flowContext) {
        var clauses = [], condition, body, marker;
        // IfClauses begin at the same location as the parent IfStatement.
        // It ends at the start of `end`, `else`, or `elseif`.
        if (trackLocations) {
            marker = locations[locations.length - 1];
            locations.push(marker);
        }
        condition = parseExpectedExpression(flowContext);
        expect('then');
        if (options.scope) createScope();
        flowContext.pushScope();
        body = parseBlock(flowContext);
        flowContext.popScope();
        if (options.scope) destroyScope();
        clauses.push(finishNode(ast.ifClause(condition, body)));
        if (trackLocations) marker = createLocationMarker();
        while(consume('elseif')){
            pushLocation(marker);
            condition = parseExpectedExpression(flowContext);
            expect('then');
            if (options.scope) createScope();
            flowContext.pushScope();
            body = parseBlock(flowContext);
            flowContext.popScope();
            if (options.scope) destroyScope();
            clauses.push(finishNode(ast.elseifClause(condition, body)));
            if (trackLocations) marker = createLocationMarker();
        }
        if (consume('else')) {
            // Include the `else` in the location of ElseClause.
            if (trackLocations) {
                marker = new Marker(previousToken);
                locations.push(marker);
            }
            if (options.scope) createScope();
            flowContext.pushScope();
            body = parseBlock(flowContext);
            flowContext.popScope();
            if (options.scope) destroyScope();
            clauses.push(finishNode(ast.elseClause(body)));
        }
        expect('end');
        return finishNode(ast.ifStatement(clauses));
    }
    // There are two types of for statements, generic and numeric.
    //
    //     for ::= Name '=' exp ',' exp [',' exp] 'do' block 'end'
    //     for ::= namelist 'in' explist 'do' block 'end'
    //     namelist ::= Name {',' Name}
    //     explist ::= exp {',' exp}
    function parseForStatement(flowContext) {
        var variable = parseIdentifier(), body;
        // The start-identifier is local.
        if (options.scope) {
            createScope();
            scopeIdentifier(variable);
        }
        // If the first expression is followed by a `=` punctuator, this is a
        // Numeric For Statement.
        if (consume('=')) {
            // Start expression
            var start = parseExpectedExpression(flowContext);
            expect(',');
            // End expression
            var end = parseExpectedExpression(flowContext);
            // Optional step expression
            var step = consume(',') ? parseExpectedExpression(flowContext) : null;
            expect('do');
            flowContext.pushScope(true);
            body = parseBlock(flowContext);
            flowContext.popScope();
            expect('end');
            if (options.scope) destroyScope();
            return finishNode(ast.forNumericStatement(variable, start, end, step, body));
        } else {
            // The namelist can contain one or more identifiers.
            var variables = [
                variable
            ];
            while(consume(',')){
                variable = parseIdentifier();
                // Each variable in the namelist is locally scoped.
                if (options.scope) scopeIdentifier(variable);
                variables.push(variable);
            }
            expect('in');
            var iterators = [];
            // One or more expressions in the explist.
            do {
                var expression = parseExpectedExpression(flowContext);
                iterators.push(expression);
            }while (consume(','));
            expect('do');
            flowContext.pushScope(true);
            body = parseBlock(flowContext);
            flowContext.popScope();
            expect('end');
            if (options.scope) destroyScope();
            return finishNode(ast.forGenericStatement(variables, iterators, body));
        }
    }
    // Local statements can either be variable assignments or function
    // definitions. If a function definition is found, it will be delegated to
    // `parseFunctionDeclaration()` with the isLocal flag.
    //
    // This AST structure might change into a local assignment with a function
    // child.
    //
    //     local ::= 'local' 'function' Name funcdecl
    //        | 'local' Name {',' Name} ['=' exp {',' exp}]
    function parseLocalStatement(flowContext) {
        var name, declToken = previousToken;
        if (Identifier === token.type) {
            var variables = [], init = [];
            do {
                name = parseIdentifier();
                variables.push(name);
                flowContext.addLocal(name.name, declToken);
            }while (consume(','));
            if (consume('=')) do {
                var expression = parseExpectedExpression(flowContext);
                init.push(expression);
            }while (consume(','));
            // Declarations doesn't exist before the statement has been evaluated.
            // Therefore assignments can't use their declarator. And the identifiers
            // shouldn't be added to the scope until the statement is complete.
            if (options.scope) for(var i = 0, l = variables.length; i < l; ++i)scopeIdentifier(variables[i]);
            return finishNode(ast.localStatement(variables, init));
        }
        if (consume('function')) {
            name = parseIdentifier();
            flowContext.addLocal(name.name, declToken);
            if (options.scope) {
                scopeIdentifier(name);
                createScope();
            }
            // MemberExpressions are not allowed in local function statements.
            return parseFunctionDeclaration(name, true);
        } else raiseUnexpectedToken('<name>', token);
    }
    //     assignment ::= varlist '=' explist
    //     var ::= Name | prefixexp '[' exp ']' | prefixexp '.' Name
    //     varlist ::= var {',' var}
    //     explist ::= exp {',' exp}
    //
    //     call ::= callexp
    //     callexp ::= prefixexp args | prefixexp ':' Name args
    function parseAssignmentOrCallStatement(flowContext) {
        // Keep a reference to the previous token for better error messages in case
        // of invalid statement
        var previous = token, marker, startMarker;
        var lvalue, base, name;
        var targets = [];
        if (trackLocations) startMarker = createLocationMarker();
        do {
            if (trackLocations) marker = createLocationMarker();
            if (Identifier === token.type) {
                name = token.value;
                base = parseIdentifier();
                // Set the parent scope.
                if (options.scope) attachScope(base, scopeHasName(name));
                lvalue = true;
            } else if ('(' === token.value) {
                next();
                base = parseExpectedExpression(flowContext);
                expect(')');
                lvalue = false;
            } else return unexpected(token);
            both: for(;;){
                var newBase;
                switch(StringLiteral === token.type ? '"' : token.value){
                    case '.':
                    case '[':
                        lvalue = true;
                        break;
                    case ':':
                    case '(':
                    case '{':
                    case '"':
                        lvalue = null;
                        break;
                    default:
                        break both;
                }
                base = parsePrefixExpressionPart(base, marker, flowContext);
            }
            targets.push(base);
            if (',' !== token.value) break;
            if (!lvalue) return unexpected(token);
            next();
        }while (true);
        if (targets.length === 1 && lvalue === null) {
            pushLocation(marker);
            return finishNode(ast.callStatement(targets[0]));
        } else if (!lvalue) return unexpected(token);
        expect('=');
        var values = [];
        do values.push(parseExpectedExpression(flowContext));
        while (consume(','));
        pushLocation(startMarker);
        return finishNode(ast.assignmentStatement(targets, values));
    }
    // ### Non-statements
    //     Identifier ::= Name
    function parseIdentifier() {
        markLocation();
        var identifier = token.value;
        if (Identifier !== token.type) raiseUnexpectedToken('<name>', token);
        next();
        return finishNode(ast.identifier(identifier));
    }
    // Parse the functions parameters and body block. The name should already
    // have been parsed and passed to this declaration function. By separating
    // this we allow for anonymous functions in expressions.
    //
    // For local functions there's a boolean parameter which needs to be set
    // when parsing the declaration.
    //
    //     funcdecl ::= '(' [parlist] ')' block 'end'
    //     parlist ::= Name {',' Name} | [',' '...'] | '...'
    function parseFunctionDeclaration(name, isLocal) {
        var flowContext = makeFlowContext();
        flowContext.pushScope();
        var parameters = [];
        expect('(');
        // The declaration has arguments
        if (!consume(')')) // Arguments are a comma separated list of identifiers, optionally ending
        // with a vararg.
        while(true){
            if (Identifier === token.type) {
                var parameter = parseIdentifier();
                // Function parameters are local.
                if (options.scope) scopeIdentifier(parameter);
                parameters.push(parameter);
                if (consume(',')) continue;
            } else if (VarargLiteral === token.type) {
                flowContext.allowVararg = true;
                parameters.push(parsePrimaryExpression(flowContext));
            } else raiseUnexpectedToken('<name> or \'...\'', token);
            expect(')');
            break;
        }
        var body = parseBlock(flowContext);
        flowContext.popScope();
        expect('end');
        if (options.scope) destroyScope();
        isLocal = isLocal || false;
        return finishNode(ast.functionStatement(name, parameters, isLocal, body));
    }
    // Parse the function name as identifiers and member expressions.
    //
    //     Name {'.' Name} [':' Name]
    function parseFunctionName() {
        var base, name, marker;
        if (trackLocations) marker = createLocationMarker();
        base = parseIdentifier();
        if (options.scope) {
            attachScope(base, scopeHasName(base.name));
            createScope();
        }
        while(consume('.')){
            pushLocation(marker);
            name = parseIdentifier();
            base = finishNode(ast.memberExpression(base, '.', name));
        }
        if (consume(':')) {
            pushLocation(marker);
            name = parseIdentifier();
            base = finishNode(ast.memberExpression(base, ':', name));
            if (options.scope) scopeIdentifierName('self');
        }
        return base;
    }
    //     tableconstructor ::= '{' [fieldlist] '}'
    //     fieldlist ::= field {fieldsep field} fieldsep
    //     field ::= '[' exp ']' '=' exp | Name = 'exp' | exp
    //
    //     fieldsep ::= ',' | ';'
    function parseTableConstructor(flowContext) {
        var fields = [], key, value;
        while(true){
            markLocation();
            if (Punctuator === token.type && consume('[')) {
                key = parseExpectedExpression(flowContext);
                expect(']');
                expect('=');
                value = parseExpectedExpression(flowContext);
                fields.push(finishNode(ast.tableKey(key, value)));
            } else if (Identifier === token.type) {
                if ('=' === lookahead.value) {
                    key = parseIdentifier();
                    next();
                    value = parseExpectedExpression(flowContext);
                    fields.push(finishNode(ast.tableKeyString(key, value)));
                } else {
                    value = parseExpectedExpression(flowContext);
                    fields.push(finishNode(ast.tableValue(value)));
                }
            } else {
                if (null == (value = parseExpression(flowContext))) {
                    locations.pop();
                    break;
                }
                fields.push(finishNode(ast.tableValue(value)));
            }
            if (',;'.indexOf(token.value) >= 0) {
                next();
                continue;
            }
            break;
        }
        expect('}');
        return finishNode(ast.tableConstructorExpression(fields));
    }
    // Expression parser
    // -----------------
    //
    // Expressions are evaluated and always return a value. If nothing is
    // matched null will be returned.
    //
    //     exp ::= (unop exp | primary | prefixexp ) { binop exp }
    //
    //     primary ::= nil | false | true | Number | String | '...'
    //          | functiondef | tableconstructor
    //
    //     prefixexp ::= (Name | '(' exp ')' ) { '[' exp ']'
    //          | '.' Name | ':' Name args | args }
    //
    function parseExpression(flowContext) {
        var expression = parseSubExpression(0, flowContext);
        return expression;
    }
    // Parse an expression expecting it to be valid.
    function parseExpectedExpression(flowContext) {
        var expression = parseExpression(flowContext);
        if (null == expression) raiseUnexpectedToken('<expression>', token);
        else return expression;
    }
    // Return the precedence priority of the operator.
    //
    // As unary `-` can't be distinguished from binary `-`, unary precedence
    // isn't described in this table but in `parseSubExpression()` itself.
    //
    // As this function gets hit on every expression it's been optimized due to
    // the expensive CompareICStub which took ~8% of the parse time.
    function binaryPrecedence(operator) {
        var charCode = operator.charCodeAt(0), length = operator.length;
        if (1 === length) switch(charCode){
            case 94:
                return 12; // ^
            case 42:
            case 47:
            case 37:
                return 10; // * / %
            case 43:
            case 45:
                return 9; // + -
            case 38:
                return 6; // &
            case 126:
                return 5; // ~
            case 124:
                return 4; // |
            case 60:
            case 62:
                return 3; // < >
        }
        else if (2 === length) switch(charCode){
            case 47:
                return 10; // //
            case 46:
                return 8; // ..
            case 60:
            case 62:
                if ('<<' === operator || '>>' === operator) return 7; // << >>
                return 3; // <= >=
            case 61:
            case 126:
                return 3; // == ~=
            case 111:
                return 1; // or
        }
        else if (97 === charCode && 'and' === operator) return 2;
        return 0;
    }
    // Implement an operator-precedence parser to handle binary operator
    // precedence.
    //
    // We use this algorithm because it's compact, it's fast and Lua core uses
    // the same so we can be sure our expressions are parsed in the same manner
    // without excessive amounts of tests.
    //
    //     exp ::= (unop exp | primary | prefixexp ) { binop exp }
    function parseSubExpression(minPrecedence, flowContext) {
        var operator = token.value, expression, marker;
        if (trackLocations) marker = createLocationMarker();
        // UnaryExpression
        if (isUnary(token)) {
            markLocation();
            next();
            var argument = parseSubExpression(10, flowContext);
            if (argument == null) raiseUnexpectedToken('<expression>', token);
            expression = finishNode(ast.unaryExpression(operator, argument));
        }
        if (null == expression) {
            // PrimaryExpression
            expression = parsePrimaryExpression(flowContext);
            // PrefixExpression
            if (null == expression) expression = parsePrefixExpression(flowContext);
        }
        // This is not a valid left hand expression.
        if (null == expression) return null;
        var precedence;
        while(true){
            operator = token.value;
            precedence = Punctuator === token.type || Keyword === token.type ? binaryPrecedence(operator) : 0;
            if (precedence === 0 || precedence <= minPrecedence) break;
            // Right-hand precedence operators
            if ('^' === operator || '..' === operator) --precedence;
            next();
            var right = parseSubExpression(precedence, flowContext);
            if (null == right) raiseUnexpectedToken('<expression>', token);
            // Push in the marker created before the loop to wrap its entirety.
            if (trackLocations) locations.push(marker);
            expression = finishNode(ast.binaryExpression(operator, expression, right));
        }
        return expression;
    }
    //     prefixexp ::= prefix {suffix}
    //     prefix ::= Name | '(' exp ')'
    //     suffix ::= '[' exp ']' | '.' Name | ':' Name args | args
    //
    //     args ::= '(' [explist] ')' | tableconstructor | String
    function parsePrefixExpressionPart(base, marker, flowContext) {
        var expression, identifier;
        if (Punctuator === token.type) switch(token.value){
            case '[':
                pushLocation(marker);
                next();
                expression = parseExpectedExpression(flowContext);
                expect(']');
                return finishNode(ast.indexExpression(base, expression));
            case '.':
                pushLocation(marker);
                next();
                identifier = parseIdentifier();
                return finishNode(ast.memberExpression(base, '.', identifier));
            case ':':
                pushLocation(marker);
                next();
                identifier = parseIdentifier();
                base = finishNode(ast.memberExpression(base, ':', identifier));
                // Once a : is found, this has to be a CallExpression, otherwise
                // throw an error.
                pushLocation(marker);
                return parseCallExpression(base, flowContext);
            case '(':
            case '{':
                pushLocation(marker);
                return parseCallExpression(base, flowContext);
        }
        else if (StringLiteral === token.type) {
            pushLocation(marker);
            return parseCallExpression(base, flowContext);
        }
        return null;
    }
    function parsePrefixExpression(flowContext) {
        var base, name, marker;
        if (trackLocations) marker = createLocationMarker();
        // The prefix
        if (Identifier === token.type) {
            name = token.value;
            base = parseIdentifier();
            // Set the parent scope.
            if (options.scope) attachScope(base, scopeHasName(name));
        } else if (consume('(')) {
            base = parseExpectedExpression(flowContext);
            expect(')');
        } else return null;
        // The suffix
        for(;;){
            var newBase = parsePrefixExpressionPart(base, marker, flowContext);
            if (newBase === null) break;
            base = newBase;
        }
        return base;
    }
    //     args ::= '(' [explist] ')' | tableconstructor | String
    function parseCallExpression(base, flowContext) {
        if (Punctuator === token.type) switch(token.value){
            case '(':
                if (!features.emptyStatement) {
                    if (token.line !== previousToken.line) raise(null, errors.ambiguousSyntax, token.value);
                }
                next();
                // List of expressions
                var expressions = [];
                var expression = parseExpression(flowContext);
                if (null != expression) expressions.push(expression);
                while(consume(',')){
                    expression = parseExpectedExpression(flowContext);
                    expressions.push(expression);
                }
                expect(')');
                return finishNode(ast.callExpression(base, expressions));
            case '{':
                markLocation();
                next();
                var table = parseTableConstructor(flowContext);
                return finishNode(ast.tableCallExpression(base, table));
        }
        else if (StringLiteral === token.type) return finishNode(ast.stringCallExpression(base, parsePrimaryExpression(flowContext)));
        raiseUnexpectedToken('function arguments', token);
    }
    //     primary ::= String | Numeric | nil | true | false
    //          | functiondef | tableconstructor | '...'
    function parsePrimaryExpression(flowContext) {
        var literals = StringLiteral | NumericLiteral | BooleanLiteral | NilLiteral | VarargLiteral, value = token.value, type = token.type, marker;
        if (trackLocations) marker = createLocationMarker();
        if (type === VarargLiteral && !flowContext.allowVararg) raise(token, errors.cannotUseVararg, token.value);
        if (type & literals) {
            pushLocation(marker);
            var raw = input.slice(token.range[0], token.range[1]);
            next();
            return finishNode(ast.literal(type, value, raw));
        } else if (Keyword === type && 'function' === value) {
            pushLocation(marker);
            next();
            if (options.scope) createScope();
            return parseFunctionDeclaration(null);
        } else if (consume('{')) {
            pushLocation(marker);
            return parseTableConstructor(flowContext);
        }
    }
    // Parser
    // ------
    // Export the main parser.
    //
    //   - `wait` Hold parsing until end() is called. Defaults to false
    //   - `comments` Store comments. Defaults to true.
    //   - `scope` Track identifier scope. Defaults to false.
    //   - `locations` Store location information. Defaults to false.
    //   - `ranges` Store the start and end character locations. Defaults to
    //     false.
    //   - `onCreateNode` Callback which will be invoked when a syntax node is
    //     created.
    //   - `onCreateScope` Callback which will be invoked when a new scope is
    //     created.
    //   - `onDestroyScope` Callback which will be invoked when the current scope
    //     is destroyed.
    //
    // Example:
    //
    //     var parser = require('luaparser');
    //     parser.parse('i = 0');
    exports1.parse = parse;
    var versionFeatures = {
        '5.1': {},
        '5.2': {
            labels: true,
            emptyStatement: true,
            hexEscapes: true,
            skipWhitespaceEscape: true,
            strictEscapes: true,
            relaxedBreak: true
        },
        '5.3': {
            labels: true,
            emptyStatement: true,
            hexEscapes: true,
            skipWhitespaceEscape: true,
            strictEscapes: true,
            unicodeEscapes: true,
            bitwiseOperators: true,
            integerDivision: true,
            relaxedBreak: true
        },
        'LuaJIT': {
            // XXX: LuaJIT language features may depend on compilation options; may need to
            // rethink how to handle this. Specifically, there is a LUAJIT_ENABLE_LUA52COMPAT
            // that removes contextual goto. Maybe add 'LuaJIT-5.2compat' as well?
            labels: true,
            contextualGoto: true,
            hexEscapes: true,
            skipWhitespaceEscape: true,
            strictEscapes: true,
            unicodeEscapes: true,
            imaginaryNumbers: true,
            integerSuffixes: true
        }
    };
    function parse(_input, _options) {
        if ('undefined' === typeof _options && 'object' === typeof _input) {
            _options = _input;
            _input = undefined;
        }
        if (!_options) _options = {};
        input = _input || '';
        options = assign({}, defaultOptions, _options);
        // Rewind the lexer
        index = 0;
        line = 1;
        lineStart = 0;
        length = input.length;
        // When tracking identifier scope, initialize with an empty scope.
        scopes = [
            []
        ];
        scopeDepth = 0;
        globals = [];
        locations = [];
        if (!Object.prototype.hasOwnProperty.call(versionFeatures, options.luaVersion)) throw new Error(sprintf("Lua version '%1' not supported", options.luaVersion));
        features = assign({}, versionFeatures[options.luaVersion]);
        if (options.extendedIdentifiers !== void 0) features.extendedIdentifiers = !!options.extendedIdentifiers;
        if (!Object.prototype.hasOwnProperty.call(encodingModes, options.encodingMode)) throw new Error(sprintf("Encoding mode '%1' not supported", options.encodingMode));
        encodingMode = encodingModes[options.encodingMode];
        if (options.comments) comments = [];
        if (!options.wait) return end();
        return exports1;
    }
    // Write to the source code buffer without beginning the parse.
    exports1.write = write;
    function write(_input) {
        input += String(_input);
        length = input.length;
        return exports1;
    }
    // Send an EOF and begin parsing.
    exports1.end = end;
    function end(_input) {
        if ('undefined' !== typeof _input) write(_input);
        // Ignore shebangs.
        if (input && input.substr(0, 2) === '#!') input = input.replace(/^.*/, function(line) {
            return line.replace(/./g, ' ');
        });
        length = input.length;
        trackLocations = options.locations || options.ranges;
        // Initialize with a lookahead token.
        lookahead = lex();
        var chunk = parseChunk();
        if (options.comments) chunk.comments = comments;
        if (options.scope) chunk.globals = globals;
        /* istanbul ignore if */ if (locations.length > 0) throw new Error('Location tracking failed. This is most likely a bug in luaparse');
        return chunk;
    }
}); /* vim: set sw=2 ts=2 et tw=79 : */ 

},{}],"axf7q":[function(require,module,exports,__globalThis) {
var global = arguments[3];
(function(root) {
    // Detect free variables `exports`
    var freeExports = exports;
    // Detect free variable `module`
    var freeModule = module && module.exports == freeExports && module;
    // Detect free variable `global`, from Node.js or Browserified code,
    // and use it as `root`
    var freeGlobal = typeof global == 'object' && global;
    if (freeGlobal.global === freeGlobal || freeGlobal.window === freeGlobal) root = freeGlobal;
    /*--------------------------------------------------------------------------*/ var luaparse = root.luaparse || require("87163b6bb8c1132");
    luaparse.defaultOptions.comments = false;
    luaparse.defaultOptions.scope = true;
    var parse = luaparse.parse;
    var regexAlphaUnderscore = /[a-zA-Z_]/;
    var regexAlphaNumUnderscore = /[a-zA-Z0-9_]/;
    var regexDigits = /[0-9]/;
    // http://www.lua.org/manual/5.2/manual.html#3.4.7
    // http://www.lua.org/source/5.2/lparser.c.html#priority
    var PRECEDENCE = {
        'or': 1,
        'and': 2,
        '<': 3,
        '>': 3,
        '<=': 3,
        '>=': 3,
        '~=': 3,
        '==': 3,
        '..': 5,
        '+': 6,
        '-': 6,
        '*': 7,
        '/': 7,
        '%': 7,
        'unarynot': 8,
        'unary#': 8,
        'unary-': 8,
        '^': 10
    };
    var IDENTIFIER_PARTS = [
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f',
        'g',
        'h',
        'i',
        'j',
        'k',
        'l',
        'm',
        'n',
        'o',
        'p',
        'q',
        'r',
        's',
        't',
        'u',
        'v',
        'w',
        'x',
        'y',
        'z',
        'A',
        'B',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'J',
        'K',
        'L',
        'M',
        'N',
        'O',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'U',
        'V',
        'W',
        'X',
        'Y',
        'Z',
        '_'
    ];
    var IDENTIFIER_PARTS_MAX = IDENTIFIER_PARTS.length - 1;
    var each = function(array, fn) {
        var index = -1;
        var length = array.length;
        var max = length - 1;
        while(++index < length)fn(array[index], index < max);
    };
    var indexOf = function(array, value) {
        var index = -1;
        var length = array.length;
        while(++index < length){
            if (array[index] == value) return index;
        }
    };
    var hasOwnProperty = {}.hasOwnProperty;
    var extend = function(destination, source) {
        var key;
        if (source) {
            for(key in source)if (hasOwnProperty.call(source, key)) destination[key] = source[key];
        }
        return destination;
    };
    var generateZeroes = function(length) {
        var zero = '0';
        var result = '';
        if (length < 1) return result;
        if (length == 1) return zero;
        while(length){
            if (length & 1) result += zero;
            if (length >>= 1) zero += zero;
        }
        return result;
    };
    // http://www.lua.org/manual/5.2/manual.html#3.1
    function isKeyword(id) {
        switch(id.length){
            case 2:
                return 'do' == id || 'if' == id || 'in' == id || 'or' == id;
            case 3:
                return 'and' == id || 'end' == id || 'for' == id || 'nil' == id || 'not' == id;
            case 4:
                return 'else' == id || 'goto' == id || 'then' == id || 'true' == id;
            case 5:
                return 'break' == id || 'false' == id || 'local' == id || 'until' == id || 'while' == id;
            case 6:
                return 'elseif' == id || 'repeat' == id || 'return' == id;
            case 8:
                return 'function' == id;
        }
        return false;
    }
    var currentIdentifier;
    var identifierMap;
    var identifiersInUse;
    var generateIdentifier = function(originalName) {
        // Preserve `self` in methods
        if (originalName == 'self') return originalName;
        if (hasOwnProperty.call(identifierMap, originalName)) return identifierMap[originalName];
        var length = currentIdentifier.length;
        var position = length - 1;
        var character;
        var index;
        while(position >= 0){
            character = currentIdentifier.charAt(position);
            index = indexOf(IDENTIFIER_PARTS, character);
            if (index != IDENTIFIER_PARTS_MAX) {
                currentIdentifier = currentIdentifier.substring(0, position) + IDENTIFIER_PARTS[index + 1] + generateZeroes(length - (position + 1));
                if (isKeyword(currentIdentifier) || indexOf(identifiersInUse, currentIdentifier) > -1) return generateIdentifier(originalName);
                identifierMap[originalName] = currentIdentifier;
                return currentIdentifier;
            }
            --position;
        }
        currentIdentifier = 'a' + generateZeroes(length);
        if (indexOf(identifiersInUse, currentIdentifier) > -1) return generateIdentifier(originalName);
        identifierMap[originalName] = currentIdentifier;
        return currentIdentifier;
    };
    /*--------------------------------------------------------------------------*/ var joinStatements = function(a, b, separator) {
        separator || (separator = ' ');
        var lastCharA = a.slice(-1);
        var firstCharB = b.charAt(0);
        if (lastCharA == '' || firstCharB == '') return a + b;
        if (regexAlphaUnderscore.test(lastCharA)) {
            if (regexAlphaNumUnderscore.test(firstCharB)) // e.g. `while` + `1`
            // e.g. `local a` + `local b`
            return a + separator + b;
            else // e.g. `not` + `(2>3 or 3<2)`
            // e.g. `x` + `^`
            return a + b;
        }
        if (regexDigits.test(lastCharA)) {
            if (firstCharB == '(' || !(firstCharB == '.' || regexAlphaUnderscore.test(firstCharB))) // e.g. `1` + `+`
            // e.g. `1` + `==`
            return a + b;
            else // e.g. `1` + `..`
            // e.g. `1` + `and`
            return a + separator + b;
        }
        if (lastCharA == firstCharB && lastCharA == '-') // e.g. `1-` + `-2`
        return a + separator + b;
        return a + b;
    };
    var formatBase = function(base) {
        var result = '';
        var type = base.type;
        var needsParens = base.inParens && (type == 'BinaryExpression' || type == 'FunctionDeclaration' || type == 'TableConstructorExpression' || type == 'LogicalExpression' || type == 'StringLiteral');
        if (needsParens) result += '(';
        result += formatExpression(base);
        if (needsParens) result += ')';
        return result;
    };
    var formatExpression = function(expression, options) {
        options = extend({
            'precedence': 0,
            'preserveIdentifiers': false
        }, options);
        var result = '';
        var currentPrecedence;
        var associativity;
        var operator;
        var expressionType = expression.type;
        if (expressionType == 'Identifier') result = expression.isLocal && !options.preserveIdentifiers ? generateIdentifier(expression.name) : expression.name;
        else if (expressionType == 'StringLiteral' || expressionType == 'NumericLiteral' || expressionType == 'BooleanLiteral' || expressionType == 'NilLiteral' || expressionType == 'VarargLiteral') result = expression.raw;
        else if (expressionType == 'LogicalExpression' || expressionType == 'BinaryExpression') {
            // If an expression with precedence x
            // contains an expression with precedence < x,
            // the inner expression must be wrapped in parens.
            operator = expression.operator;
            currentPrecedence = PRECEDENCE[operator];
            associativity = 'left';
            result = formatExpression(expression.left, {
                'precedence': currentPrecedence,
                'direction': 'left',
                'parent': operator
            });
            result = joinStatements(result, operator);
            result = joinStatements(result, formatExpression(expression.right, {
                'precedence': currentPrecedence,
                'direction': 'right',
                'parent': operator
            }));
            if (operator == '^' || operator == '..') associativity = "right";
            if (currentPrecedence < options.precedence || currentPrecedence == options.precedence && associativity != options.direction && options.parent != '+' && !(options.parent == '*' && (operator == '/' || operator == '*'))) // The most simple case here is that of
            // protecting the parentheses on the RHS of
            // `1 - (2 - 3)` but deleting them from `(1 - 2) - 3`.
            // This is generally the right thing to do. The
            // semantics of `+` are special however: `1 + (2 - 3)`
            // == `1 + 2 - 3`. `-` and `+` are the only two operators
            // who share their precedence level. `*` also can
            // commute in such a way with `/`, but not with `%`
            // (all three share a precedence). So we test for
            // all of these conditions and avoid emitting
            // parentheses in the cases where we don’t have to.
            result = '(' + result + ')';
        } else if (expressionType == 'UnaryExpression') {
            operator = expression.operator;
            currentPrecedence = PRECEDENCE['unary' + operator];
            result = joinStatements(operator, formatExpression(expression.argument, {
                'precedence': currentPrecedence
            }));
            if (currentPrecedence < options.precedence && // In principle, we should parenthesize the RHS of an
            // expression like `3^-2`, because `^` has higher precedence
            // than unary `-` according to the manual. But that is
            // misleading on the RHS of `^`, since the parser will
            // always try to find a unary operator regardless of
            // precedence.
            !(options.parent == '^' && options.direction == 'right')) result = '(' + result + ')';
        } else if (expressionType == 'CallExpression') {
            result = formatBase(expression.base) + '(';
            each(expression.arguments, function(argument, needsComma) {
                result += formatExpression(argument);
                if (needsComma) result += ',';
            });
            result += ')';
        } else if (expressionType == 'TableCallExpression') result = formatExpression(expression.base) + formatExpression(expression.arguments);
        else if (expressionType == 'StringCallExpression') result = formatExpression(expression.base) + formatExpression(expression.argument);
        else if (expressionType == 'IndexExpression') result = formatBase(expression.base) + '[' + formatExpression(expression.index) + ']';
        else if (expressionType == 'MemberExpression') result = formatBase(expression.base) + expression.indexer + formatExpression(expression.identifier, {
            'preserveIdentifiers': true
        });
        else if (expressionType == 'FunctionDeclaration') {
            result = 'function(';
            if (expression.parameters.length) each(expression.parameters, function(parameter, needsComma) {
                // `Identifier`s have a `name`, `VarargLiteral`s have a `value`
                result += parameter.name ? generateIdentifier(parameter.name) : parameter.value;
                if (needsComma) result += ',';
            });
            result += ')';
            result = joinStatements(result, formatStatementList(expression.body));
            result = joinStatements(result, 'end');
        } else if (expressionType == 'TableConstructorExpression') {
            result = '{';
            each(expression.fields, function(field, needsComma) {
                if (field.type == 'TableKey') result += '[' + formatExpression(field.key) + ']=' + formatExpression(field.value);
                else if (field.type == 'TableValue') result += formatExpression(field.value);
                else result += formatExpression(field.key, {
                    // TODO: keep track of nested scopes (#18)
                    'preserveIdentifiers': true
                }) + '=' + formatExpression(field.value);
                if (needsComma) result += ',';
            });
            result += '}';
        } else throw TypeError('Unknown expression type: `' + expressionType + '`');
        return result;
    };
    var formatStatementList = function(body) {
        var result = '';
        each(body, function(statement) {
            result = joinStatements(result, formatStatement(statement), ';');
        });
        return result;
    };
    var formatStatement = function(statement) {
        var result = '';
        var statementType = statement.type;
        if (statementType == 'AssignmentStatement') {
            // left-hand side
            each(statement.variables, function(variable, needsComma) {
                result += formatExpression(variable);
                if (needsComma) result += ',';
            });
            // right-hand side
            result += '=';
            each(statement.init, function(init, needsComma) {
                result += formatExpression(init);
                if (needsComma) result += ',';
            });
        } else if (statementType == 'LocalStatement') {
            result = 'local ';
            // left-hand side
            each(statement.variables, function(variable, needsComma) {
                // Variables in a `LocalStatement` are always local, duh
                result += generateIdentifier(variable.name);
                if (needsComma) result += ',';
            });
            // right-hand side
            if (statement.init.length) {
                result += '=';
                each(statement.init, function(init, needsComma) {
                    result += formatExpression(init);
                    if (needsComma) result += ',';
                });
            }
        } else if (statementType == 'CallStatement') result = formatExpression(statement.expression);
        else if (statementType == 'IfStatement') {
            result = joinStatements('if', formatExpression(statement.clauses[0].condition));
            result = joinStatements(result, 'then');
            result = joinStatements(result, formatStatementList(statement.clauses[0].body));
            each(statement.clauses.slice(1), function(clause) {
                if (clause.condition) {
                    result = joinStatements(result, 'elseif');
                    result = joinStatements(result, formatExpression(clause.condition));
                    result = joinStatements(result, 'then');
                } else result = joinStatements(result, 'else');
                result = joinStatements(result, formatStatementList(clause.body));
            });
            result = joinStatements(result, 'end');
        } else if (statementType == 'WhileStatement') {
            result = joinStatements('while', formatExpression(statement.condition));
            result = joinStatements(result, 'do');
            result = joinStatements(result, formatStatementList(statement.body));
            result = joinStatements(result, 'end');
        } else if (statementType == 'DoStatement') {
            result = joinStatements('do', formatStatementList(statement.body));
            result = joinStatements(result, 'end');
        } else if (statementType == 'ReturnStatement') {
            result = 'return';
            each(statement.arguments, function(argument, needsComma) {
                result = joinStatements(result, formatExpression(argument));
                if (needsComma) result += ',';
            });
        } else if (statementType == 'BreakStatement') result = 'break';
        else if (statementType == 'RepeatStatement') {
            result = joinStatements('repeat', formatStatementList(statement.body));
            result = joinStatements(result, 'until');
            result = joinStatements(result, formatExpression(statement.condition));
        } else if (statementType == 'FunctionDeclaration') {
            result = (statement.isLocal ? 'local ' : '') + 'function ';
            result += formatExpression(statement.identifier);
            result += '(';
            if (statement.parameters.length) each(statement.parameters, function(parameter, needsComma) {
                // `Identifier`s have a `name`, `VarargLiteral`s have a `value`
                result += parameter.name ? generateIdentifier(parameter.name) : parameter.value;
                if (needsComma) result += ',';
            });
            result += ')';
            result = joinStatements(result, formatStatementList(statement.body));
            result = joinStatements(result, 'end');
        } else if (statementType == 'ForGenericStatement') {
            // see also `ForNumericStatement`
            result = 'for ';
            each(statement.variables, function(variable, needsComma) {
                // The variables in a `ForGenericStatement` are always local
                result += generateIdentifier(variable.name);
                if (needsComma) result += ',';
            });
            result += ' in';
            each(statement.iterators, function(iterator, needsComma) {
                result = joinStatements(result, formatExpression(iterator));
                if (needsComma) result += ',';
            });
            result = joinStatements(result, 'do');
            result = joinStatements(result, formatStatementList(statement.body));
            result = joinStatements(result, 'end');
        } else if (statementType == 'ForNumericStatement') {
            // The variables in a `ForNumericStatement` are always local
            result = 'for ' + generateIdentifier(statement.variable.name) + '=';
            result += formatExpression(statement.start) + ',' + formatExpression(statement.end);
            if (statement.step) result += ',' + formatExpression(statement.step);
            result = joinStatements(result, 'do');
            result = joinStatements(result, formatStatementList(statement.body));
            result = joinStatements(result, 'end');
        } else if (statementType == 'LabelStatement') // The identifier names in a `LabelStatement` can safely be renamed
        result = '::' + generateIdentifier(statement.label.name) + '::';
        else if (statementType == 'GotoStatement') // The identifier names in a `GotoStatement` can safely be renamed
        result = 'goto ' + generateIdentifier(statement.label.name);
        else throw TypeError('Unknown statement type: `' + statementType + '`');
        return result;
    };
    var minify = function(argument) {
        // `argument` can be a Lua code snippet (string)
        // or a luaparse-compatible AST (object)
        var ast = typeof argument == 'string' ? parse(argument) : argument;
        // (Re)set temporary identifier values
        identifierMap = {};
        identifiersInUse = [];
        // This is a shortcut to help generate the first identifier (`a`) faster
        currentIdentifier = '9';
        // Make sure global variable names aren't renamed
        if (ast.globals) each(ast.globals, function(object) {
            var name = object.name;
            identifierMap[name] = name;
            identifiersInUse.push(name);
        });
        else throw Error('Missing required AST property: `globals`');
        return formatStatementList(ast.body);
    };
    /*--------------------------------------------------------------------------*/ var luamin = {
        'version': '1.0.4',
        'minify': minify
    };
    // Some AMD build optimizers, like r.js, check for specific condition patterns
    // like the following:
    if (typeof define == 'function' && typeof define.amd == 'object' && define.amd) define(function() {
        return luamin;
    });
    else if (freeExports && !freeExports.nodeType) {
        if (freeModule) freeModule.exports = luamin;
        else extend(freeExports, luamin);
    } else root.luamin = luamin;
})(this);

},{"87163b6bb8c1132":"koo84"}],"koo84":[function(require,module,exports,__globalThis) {
var global = arguments[3];
/* global exports:true, module:true, require:true, define:true, global:true */ (function(root, name, factory) {
    /* jshint eqeqeq:false */ 'use strict';
    // Used to determine if values are of the language type `Object`
    var objectTypes = {
        'function': true,
        'object': true
    }, freeExports = objectTypes["object"] && exports && !exports.nodeType && exports, freeModule = objectTypes["object"] && module && !module.nodeType && module, freeGlobal = freeExports && freeModule && typeof global == 'object' && global, moduleExports = freeModule && freeModule.exports === freeExports && freeExports;
    if (freeGlobal && (freeGlobal.global === freeGlobal || freeGlobal.window === freeGlobal || freeGlobal.self === freeGlobal)) root = freeGlobal;
    // Some AMD build optimizers, like r.js, check for specific condition
    // patterns like the following:
    if (typeof define == 'function' && typeof define.amd == 'object' && define.amd) {
        // defined as an anonymous module.
        define([
            'exports'
        ], factory);
        // In case the source has been processed and wrapped in a define module use
        // the supplied `exports` object.
        if (freeExports && moduleExports) factory(freeModule.exports);
    } else if (freeExports && freeModule) {
        // in Node.js or RingoJS v0.8.0+
        if (moduleExports) factory(freeModule.exports);
        else factory(freeExports);
    } else factory(root[name] = {});
})(this, 'luaparse', function(exports1) {
    'use strict';
    exports1.version = '0.2.1';
    var input, options, length;
    // Options can be set either globally on the parser object through
    // defaultOptions, or during the parse call.
    var defaultOptions = exports1.defaultOptions = {
        // Explicitly tell the parser when the input ends.
        wait: false,
        comments: true,
        scope: false,
        locations: false,
        ranges: false,
        onCreateNode: null,
        onCreateScope: null,
        onDestroyScope: null
    };
    // The available tokens expressed as enum flags so they can be checked with
    // bitwise operations.
    var EOF = 1, StringLiteral = 2, Keyword = 4, Identifier = 8, NumericLiteral = 16, Punctuator = 32, BooleanLiteral = 64, NilLiteral = 128, VarargLiteral = 256;
    exports1.tokenTypes = {
        EOF: EOF,
        StringLiteral: StringLiteral,
        Keyword: Keyword,
        Identifier: Identifier,
        NumericLiteral: NumericLiteral,
        Punctuator: Punctuator,
        BooleanLiteral: BooleanLiteral,
        NilLiteral: NilLiteral,
        VarargLiteral: VarargLiteral
    };
    // As this parser is a bit different from luas own, the error messages
    // will be different in some situations.
    var errors = exports1.errors = {
        unexpected: 'unexpected %1 \'%2\' near \'%3\'',
        expected: '\'%1\' expected near \'%2\'',
        expectedToken: '%1 expected near \'%2\'',
        unfinishedString: 'unfinished string near \'%1\'',
        malformedNumber: 'malformed number near \'%1\'',
        invalidVar: 'invalid left-hand side of assignment near \'%1\''
    };
    // ### Abstract Syntax Tree
    //
    // The default AST structure is inspired by the Mozilla Parser API but can
    // easily be customized by overriding these functions.
    var ast = exports1.ast = {
        labelStatement: function(label) {
            return {
                type: 'LabelStatement',
                label: label
            };
        },
        breakStatement: function() {
            return {
                type: 'BreakStatement'
            };
        },
        gotoStatement: function(label) {
            return {
                type: 'GotoStatement',
                label: label
            };
        },
        returnStatement: function(args) {
            return {
                type: 'ReturnStatement',
                'arguments': args
            };
        },
        ifStatement: function(clauses) {
            return {
                type: 'IfStatement',
                clauses: clauses
            };
        },
        ifClause: function(condition, body) {
            return {
                type: 'IfClause',
                condition: condition,
                body: body
            };
        },
        elseifClause: function(condition, body) {
            return {
                type: 'ElseifClause',
                condition: condition,
                body: body
            };
        },
        elseClause: function(body) {
            return {
                type: 'ElseClause',
                body: body
            };
        },
        whileStatement: function(condition, body) {
            return {
                type: 'WhileStatement',
                condition: condition,
                body: body
            };
        },
        doStatement: function(body) {
            return {
                type: 'DoStatement',
                body: body
            };
        },
        repeatStatement: function(condition, body) {
            return {
                type: 'RepeatStatement',
                condition: condition,
                body: body
            };
        },
        localStatement: function(variables, init) {
            return {
                type: 'LocalStatement',
                variables: variables,
                init: init
            };
        },
        assignmentStatement: function(variables, init) {
            return {
                type: 'AssignmentStatement',
                variables: variables,
                init: init
            };
        },
        callStatement: function(expression) {
            return {
                type: 'CallStatement',
                expression: expression
            };
        },
        functionStatement: function(identifier, parameters, isLocal, body) {
            return {
                type: 'FunctionDeclaration',
                identifier: identifier,
                isLocal: isLocal,
                parameters: parameters,
                body: body
            };
        },
        forNumericStatement: function(variable, start, end, step, body) {
            return {
                type: 'ForNumericStatement',
                variable: variable,
                start: start,
                end: end,
                step: step,
                body: body
            };
        },
        forGenericStatement: function(variables, iterators, body) {
            return {
                type: 'ForGenericStatement',
                variables: variables,
                iterators: iterators,
                body: body
            };
        },
        chunk: function(body) {
            return {
                type: 'Chunk',
                body: body
            };
        },
        identifier: function(name) {
            return {
                type: 'Identifier',
                name: name
            };
        },
        literal: function(type, value, raw) {
            type = type === StringLiteral ? 'StringLiteral' : type === NumericLiteral ? 'NumericLiteral' : type === BooleanLiteral ? 'BooleanLiteral' : type === NilLiteral ? 'NilLiteral' : 'VarargLiteral';
            return {
                type: type,
                value: value,
                raw: raw
            };
        },
        tableKey: function(key, value) {
            return {
                type: 'TableKey',
                key: key,
                value: value
            };
        },
        tableKeyString: function(key, value) {
            return {
                type: 'TableKeyString',
                key: key,
                value: value
            };
        },
        tableValue: function(value) {
            return {
                type: 'TableValue',
                value: value
            };
        },
        tableConstructorExpression: function(fields) {
            return {
                type: 'TableConstructorExpression',
                fields: fields
            };
        },
        binaryExpression: function(operator, left, right) {
            var type = 'and' === operator || 'or' === operator ? 'LogicalExpression' : 'BinaryExpression';
            return {
                type: type,
                operator: operator,
                left: left,
                right: right
            };
        },
        unaryExpression: function(operator, argument) {
            return {
                type: 'UnaryExpression',
                operator: operator,
                argument: argument
            };
        },
        memberExpression: function(base, indexer, identifier) {
            return {
                type: 'MemberExpression',
                indexer: indexer,
                identifier: identifier,
                base: base
            };
        },
        indexExpression: function(base, index) {
            return {
                type: 'IndexExpression',
                base: base,
                index: index
            };
        },
        callExpression: function(base, args) {
            return {
                type: 'CallExpression',
                base: base,
                'arguments': args
            };
        },
        tableCallExpression: function(base, args) {
            return {
                type: 'TableCallExpression',
                base: base,
                'arguments': args
            };
        },
        stringCallExpression: function(base, argument) {
            return {
                type: 'StringCallExpression',
                base: base,
                argument: argument
            };
        },
        comment: function(value, raw) {
            return {
                type: 'Comment',
                value: value,
                raw: raw
            };
        }
    };
    // Wrap up the node object.
    function finishNode(node) {
        // Pop a `Marker` off the location-array and attach its location data.
        if (trackLocations) {
            var location = locations.pop();
            location.complete();
            if (options.locations) node.loc = location.loc;
            if (options.ranges) node.range = location.range;
        }
        if (options.onCreateNode) options.onCreateNode(node);
        return node;
    }
    // Helpers
    // -------
    var slice = Array.prototype.slice, toString = Object.prototype.toString, indexOf = function indexOf(array, element) {
        for(var i = 0, length = array.length; i < length; i++){
            if (array[i] === element) return i;
        }
        return -1;
    };
    // Iterate through an array of objects and return the index of an object
    // with a matching property.
    function indexOfObject(array, property, element) {
        for(var i = 0, length = array.length; i < length; i++){
            if (array[i][property] === element) return i;
        }
        return -1;
    }
    // A sprintf implementation using %index (beginning at 1) to input
    // arguments in the format string.
    //
    // Example:
    //
    //     // Unexpected function in token
    //     sprintf('Unexpected %2 in %1.', 'token', 'function');
    function sprintf(format) {
        var args = slice.call(arguments, 1);
        format = format.replace(/%(\d)/g, function(match, index) {
            return '' + args[index - 1] || '';
        });
        return format;
    }
    // Returns a new object with the properties from all objectes passed as
    // arguments. Last argument takes precedence.
    //
    // Example:
    //
    //     this.options = extend(options, { output: false });
    function extend() {
        var args = slice.call(arguments), dest = {}, src, prop;
        for(var i = 0, length = args.length; i < length; i++){
            src = args[i];
            for(prop in src)if (src.hasOwnProperty(prop)) dest[prop] = src[prop];
        }
        return dest;
    }
    // ### Error functions
    // #### Raise an exception.
    //
    // Raise an exception by passing a token, a string format and its paramters.
    //
    // The passed tokens location will automatically be added to the error
    // message if it exists, if not it will default to the lexers current
    // position.
    //
    // Example:
    //
    //     // [1:0] expected [ near (
    //     raise(token, "expected %1 near %2", '[', token.value);
    function raise(token) {
        var message = sprintf.apply(null, slice.call(arguments, 1)), error, col;
        if ('undefined' !== typeof token.line) {
            col = token.range[0] - token.lineStart;
            error = new SyntaxError(sprintf('[%1:%2] %3', token.line, col, message));
            error.line = token.line;
            error.index = token.range[0];
            error.column = col;
        } else {
            col = index - lineStart + 1;
            error = new SyntaxError(sprintf('[%1:%2] %3', line, col, message));
            error.index = index;
            error.line = line;
            error.column = col;
        }
        throw error;
    }
    // #### Raise an unexpected token error.
    //
    // Example:
    //
    //     // expected <name> near '0'
    //     raiseUnexpectedToken('<name>', token);
    function raiseUnexpectedToken(type, token) {
        raise(token, errors.expectedToken, type, token.value);
    }
    // #### Raise a general unexpected error
    //
    // Usage should pass either a token object or a symbol string which was
    // expected. We can also specify a nearby token such as <eof>, this will
    // default to the currently active token.
    //
    // Example:
    //
    //     // Unexpected symbol 'end' near '<eof>'
    //     unexpected(token);
    //
    // If there's no token in the buffer it means we have reached <eof>.
    function unexpected(found, near) {
        if ('undefined' === typeof near) near = lookahead.value;
        if ('undefined' !== typeof found.type) {
            var type;
            switch(found.type){
                case StringLiteral:
                    type = 'string';
                    break;
                case Keyword:
                    type = 'keyword';
                    break;
                case Identifier:
                    type = 'identifier';
                    break;
                case NumericLiteral:
                    type = 'number';
                    break;
                case Punctuator:
                    type = 'symbol';
                    break;
                case BooleanLiteral:
                    type = 'boolean';
                    break;
                case NilLiteral:
                    return raise(found, errors.unexpected, 'symbol', 'nil', near);
            }
            return raise(found, errors.unexpected, type, found.value, near);
        }
        return raise(found, errors.unexpected, 'symbol', found, near);
    }
    // Lexer
    // -----
    //
    // The lexer, or the tokenizer reads the input string character by character
    // and derives a token left-right. To be as efficient as possible the lexer
    // prioritizes the common cases such as identifiers. It also works with
    // character codes instead of characters as string comparisons was the
    // biggest bottleneck of the parser.
    //
    // If `options.comments` is enabled, all comments encountered will be stored
    // in an array which later will be appended to the chunk object. If disabled,
    // they will simply be disregarded.
    //
    // When the lexer has derived a valid token, it will be returned as an object
    // containing its value and as well as its position in the input string (this
    // is always enabled to provide proper debug messages).
    //
    // `lex()` starts lexing and returns the following token in the stream.
    var index, token, previousToken, lookahead, comments, tokenStart, line, lineStart;
    exports1.lex = lex;
    function lex() {
        skipWhiteSpace();
        // Skip comments beginning with --
        while(45 === input.charCodeAt(index) && 45 === input.charCodeAt(index + 1)){
            scanComment();
            skipWhiteSpace();
        }
        if (index >= length) return {
            type: EOF,
            value: '<eof>',
            line: line,
            lineStart: lineStart,
            range: [
                index,
                index
            ]
        };
        var charCode = input.charCodeAt(index), next = input.charCodeAt(index + 1);
        // Memorize the range index where the token begins.
        tokenStart = index;
        if (isIdentifierStart(charCode)) return scanIdentifierOrKeyword();
        switch(charCode){
            case 39:
            case 34:
                return scanStringLiteral();
            // 0-9
            case 48:
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
            case 57:
                return scanNumericLiteral();
            case 46:
                // If the dot is followed by a digit it's a float.
                if (isDecDigit(next)) return scanNumericLiteral();
                if (46 === next) {
                    if (46 === input.charCodeAt(index + 2)) return scanVarargLiteral();
                    return scanPunctuator('..');
                }
                return scanPunctuator('.');
            case 61:
                if (61 === next) return scanPunctuator('==');
                return scanPunctuator('=');
            case 62:
                if (61 === next) return scanPunctuator('>=');
                if (62 === next) return scanPunctuator('>>');
                return scanPunctuator('>');
            case 60:
                if (60 === next) return scanPunctuator('<<');
                if (61 === next) return scanPunctuator('<=');
                return scanPunctuator('<');
            case 126:
                if (61 === next) return scanPunctuator('~=');
                return scanPunctuator('~');
            case 58:
                if (58 === next) return scanPunctuator('::');
                return scanPunctuator(':');
            case 91:
                // Check for a multiline string, they begin with [= or [[
                if (91 === next || 61 === next) return scanLongStringLiteral();
                return scanPunctuator('[');
            case 47:
                // Check for integer division op (//)
                if (47 === next) return scanPunctuator('//');
                return scanPunctuator('/');
            // * ^ % , { } ] ( ) ; & # - + |
            case 42:
            case 94:
            case 37:
            case 44:
            case 123:
            case 124:
            case 125:
            case 93:
            case 40:
            case 41:
            case 59:
            case 38:
            case 35:
            case 45:
            case 43:
                return scanPunctuator(input.charAt(index));
        }
        return unexpected(input.charAt(index));
    }
    // Whitespace has no semantic meaning in lua so simply skip ahead while
    // tracking the encounted newlines. Any kind of eol sequence is counted as a
    // single line.
    function consumeEOL() {
        var charCode = input.charCodeAt(index), peekCharCode = input.charCodeAt(index + 1);
        if (isLineTerminator(charCode)) {
            // Count \n\r and \r\n as one newline.
            if (10 === charCode && 13 === peekCharCode) index++;
            if (13 === charCode && 10 === peekCharCode) index++;
            line++;
            lineStart = ++index;
            return true;
        }
        return false;
    }
    function skipWhiteSpace() {
        while(index < length){
            var charCode = input.charCodeAt(index);
            if (isWhiteSpace(charCode)) index++;
            else if (!consumeEOL()) break;
        }
    }
    // Identifiers, keywords, booleans and nil all look the same syntax wise. We
    // simply go through them one by one and defaulting to an identifier if no
    // previous case matched.
    function scanIdentifierOrKeyword() {
        var value, type;
        // Slicing the input string is prefered before string concatenation in a
        // loop for performance reasons.
        while(isIdentifierPart(input.charCodeAt(++index)));
        value = input.slice(tokenStart, index);
        // Decide on the token type and possibly cast the value.
        if (isKeyword(value)) type = Keyword;
        else if ('true' === value || 'false' === value) {
            type = BooleanLiteral;
            value = 'true' === value;
        } else if ('nil' === value) {
            type = NilLiteral;
            value = null;
        } else type = Identifier;
        return {
            type: type,
            value: value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Once a punctuator reaches this function it should already have been
    // validated so we simply return it as a token.
    function scanPunctuator(value) {
        index += value.length;
        return {
            type: Punctuator,
            value: value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // A vararg literal consists of three dots.
    function scanVarargLiteral() {
        index += 3;
        return {
            type: VarargLiteral,
            value: '...',
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Find the string literal by matching the delimiter marks used.
    function scanStringLiteral() {
        var delimiter = input.charCodeAt(index++), stringStart = index, string = '', charCode;
        while(index < length){
            charCode = input.charCodeAt(index++);
            if (delimiter === charCode) break;
            if (92 === charCode) {
                string += input.slice(stringStart, index - 1) + readEscapeSequence();
                stringStart = index;
            } else if (index >= length || isLineTerminator(charCode)) {
                string += input.slice(stringStart, index - 1);
                raise({}, errors.unfinishedString, string + String.fromCharCode(charCode));
            }
        }
        string += input.slice(stringStart, index - 1);
        return {
            type: StringLiteral,
            value: string,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Expect a multiline string literal and return it as a regular string
    // literal, if it doesn't validate into a valid multiline string, throw an
    // exception.
    function scanLongStringLiteral() {
        var string = readLongString();
        // Fail if it's not a multiline literal.
        if (false === string) raise(token, errors.expected, '[', token.value);
        return {
            type: StringLiteral,
            value: string,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Numeric literals will be returned as floating-point numbers instead of
    // strings. The raw value should be retrieved from slicing the input string
    // later on in the process.
    //
    // If a hexadecimal number is encountered, it will be converted.
    function scanNumericLiteral() {
        var character = input.charAt(index), next = input.charAt(index + 1);
        var value = '0' === character && 'xX'.indexOf(next || null) >= 0 ? readHexLiteral() : readDecLiteral();
        return {
            type: NumericLiteral,
            value: value,
            line: line,
            lineStart: lineStart,
            range: [
                tokenStart,
                index
            ]
        };
    }
    // Lua hexadecimals have an optional fraction part and an optional binary
    // exoponent part. These are not included in JavaScript so we will compute
    // all three parts separately and then sum them up at the end of the function
    // with the following algorithm.
    //
    //     Digit := toDec(digit)
    //     Fraction := toDec(fraction) / 16 ^ fractionCount
    //     BinaryExp := 2 ^ binaryExp
    //     Number := ( Digit + Fraction ) * BinaryExp
    function readHexLiteral() {
        var fraction = 0 // defaults to 0 as it gets summed
        , binaryExponent = 1 // defaults to 1 as it gets multiplied
        , binarySign = 1 // positive
        , digit, fractionStart, exponentStart, digitStart;
        digitStart = index += 2; // Skip 0x part
        // A minimum of one hex digit is required.
        if (!isHexDigit(input.charCodeAt(index))) raise({}, errors.malformedNumber, input.slice(tokenStart, index));
        while(isHexDigit(input.charCodeAt(index)))index++;
        // Convert the hexadecimal digit to base 10.
        digit = parseInt(input.slice(digitStart, index), 16);
        // Fraction part i optional.
        if ('.' === input.charAt(index)) {
            fractionStart = ++index;
            while(isHexDigit(input.charCodeAt(index)))index++;
            fraction = input.slice(fractionStart, index);
            // Empty fraction parts should default to 0, others should be converted
            // 0.x form so we can use summation at the end.
            fraction = fractionStart === index ? 0 : parseInt(fraction, 16) / Math.pow(16, index - fractionStart);
        }
        // Binary exponents are optional
        if ('pP'.indexOf(input.charAt(index) || null) >= 0) {
            index++;
            // Sign part is optional and defaults to 1 (positive).
            if ('+-'.indexOf(input.charAt(index) || null) >= 0) binarySign = '+' === input.charAt(index++) ? 1 : -1;
            exponentStart = index;
            // The binary exponent sign requires a decimal digit.
            if (!isDecDigit(input.charCodeAt(index))) raise({}, errors.malformedNumber, input.slice(tokenStart, index));
            while(isDecDigit(input.charCodeAt(index)))index++;
            binaryExponent = input.slice(exponentStart, index);
            // Calculate the binary exponent of the number.
            binaryExponent = Math.pow(2, binaryExponent * binarySign);
        }
        return (digit + fraction) * binaryExponent;
    }
    // Decimal numbers are exactly the same in Lua and in JavaScript, because of
    // this we check where the token ends and then parse it with native
    // functions.
    function readDecLiteral() {
        while(isDecDigit(input.charCodeAt(index)))index++;
        // Fraction part is optional
        if ('.' === input.charAt(index)) {
            index++;
            // Fraction part defaults to 0
            while(isDecDigit(input.charCodeAt(index)))index++;
        }
        // Exponent part is optional.
        if ('eE'.indexOf(input.charAt(index) || null) >= 0) {
            index++;
            // Sign part is optional.
            if ('+-'.indexOf(input.charAt(index) || null) >= 0) index++;
            // An exponent is required to contain at least one decimal digit.
            if (!isDecDigit(input.charCodeAt(index))) raise({}, errors.malformedNumber, input.slice(tokenStart, index));
            while(isDecDigit(input.charCodeAt(index)))index++;
        }
        return parseFloat(input.slice(tokenStart, index));
    }
    // Translate escape sequences to the actual characters.
    function readEscapeSequence() {
        var sequenceStart = index;
        switch(input.charAt(index)){
            // Lua allow the following escape sequences.
            // We don't escape the bell sequence.
            case 'n':
                index++;
                return '\n';
            case 'r':
                index++;
                return '\r';
            case 't':
                index++;
                return '\t';
            case 'v':
                index++;
                return '\x0B';
            case 'b':
                index++;
                return '\b';
            case 'f':
                index++;
                return '\f';
            // Skips the following span of white-space.
            case 'z':
                index++;
                skipWhiteSpace();
                return '';
            // Byte representation should for now be returned as is.
            case 'x':
                // \xXX, where XX is a sequence of exactly two hexadecimal digits
                if (isHexDigit(input.charCodeAt(index + 1)) && isHexDigit(input.charCodeAt(index + 2))) {
                    index += 3;
                    // Return it as is, without translating the byte.
                    return '\\' + input.slice(sequenceStart, index);
                }
                return '\\' + input.charAt(index++);
            default:
                // \ddd, where ddd is a sequence of up to three decimal digits.
                if (isDecDigit(input.charCodeAt(index))) {
                    while(isDecDigit(input.charCodeAt(++index)));
                    return '\\' + input.slice(sequenceStart, index);
                }
                // Simply return the \ as is, it's not escaping any sequence.
                return input.charAt(index++);
        }
    }
    // Comments begin with -- after which it will be decided if they are
    // multiline comments or not.
    //
    // The multiline functionality works the exact same way as with string
    // literals so we reuse the functionality.
    function scanComment() {
        tokenStart = index;
        index += 2; // --
        var character = input.charAt(index), content = '', isLong = false, commentStart = index, lineStartComment = lineStart, lineComment = line;
        if ('[' === character) {
            content = readLongString();
            // This wasn't a multiline comment after all.
            if (false === content) content = character;
            else isLong = true;
        }
        // Scan until next line as long as it's not a multiline comment.
        if (!isLong) {
            while(index < length){
                if (isLineTerminator(input.charCodeAt(index))) break;
                index++;
            }
            if (options.comments) content = input.slice(commentStart, index);
        }
        if (options.comments) {
            var node = ast.comment(content, input.slice(tokenStart, index));
            // `Marker`s depend on tokens available in the parser and as comments are
            // intercepted in the lexer all location data is set manually.
            if (options.locations) node.loc = {
                start: {
                    line: lineComment,
                    column: tokenStart - lineStartComment
                },
                end: {
                    line: line,
                    column: index - lineStart
                }
            };
            if (options.ranges) node.range = [
                tokenStart,
                index
            ];
            if (options.onCreateNode) options.onCreateNode(node);
            comments.push(node);
        }
    }
    // Read a multiline string by calculating the depth of `=` characters and
    // then appending until an equal depth is found.
    function readLongString() {
        var level = 0, content = '', terminator = false, character, stringStart;
        index++; // [
        // Calculate the depth of the comment.
        while('=' === input.charAt(index + level))level++;
        // Exit, this is not a long string afterall.
        if ('[' !== input.charAt(index + level)) return false;
        index += level + 1;
        // If the first character is a newline, ignore it and begin on next line.
        if (isLineTerminator(input.charCodeAt(index))) consumeEOL();
        stringStart = index;
        while(index < length){
            // To keep track of line numbers run the `consumeEOL()` which increments
            // its counter.
            if (isLineTerminator(input.charCodeAt(index))) consumeEOL();
            character = input.charAt(index++);
            // Once the delimiter is found, iterate through the depth count and see
            // if it matches.
            if (']' === character) {
                terminator = true;
                for(var i = 0; i < level; i++)if ('=' !== input.charAt(index + i)) terminator = false;
                if (']' !== input.charAt(index + level)) terminator = false;
            }
            // We reached the end of the multiline string. Get out now.
            if (terminator) break;
        }
        content += input.slice(stringStart, index - 1);
        index += level + 1;
        return content;
    }
    // ## Lex functions and helpers.
    // Read the next token.
    //
    // This is actually done by setting the current token to the lookahead and
    // reading in the new lookahead token.
    function next() {
        previousToken = token;
        token = lookahead;
        lookahead = lex();
    }
    // Consume a token if its value matches. Once consumed or not, return the
    // success of the operation.
    function consume(value) {
        if (value === token.value) {
            next();
            return true;
        }
        return false;
    }
    // Expect the next token value to match. If not, throw an exception.
    function expect(value) {
        if (value === token.value) next();
        else raise(token, errors.expected, value, token.value);
    }
    // ### Validation functions
    function isWhiteSpace(charCode) {
        return 9 === charCode || 32 === charCode || 0xB === charCode || 0xC === charCode;
    }
    function isLineTerminator(charCode) {
        return 10 === charCode || 13 === charCode;
    }
    function isDecDigit(charCode) {
        return charCode >= 48 && charCode <= 57;
    }
    function isHexDigit(charCode) {
        return charCode >= 48 && charCode <= 57 || charCode >= 97 && charCode <= 102 || charCode >= 65 && charCode <= 70;
    }
    // From [Lua 5.2](http://www.lua.org/manual/5.2/manual.html#8.1) onwards
    // identifiers cannot use locale-dependet letters.
    function isIdentifierStart(charCode) {
        return charCode >= 65 && charCode <= 90 || charCode >= 97 && charCode <= 122 || 95 === charCode;
    }
    function isIdentifierPart(charCode) {
        return charCode >= 65 && charCode <= 90 || charCode >= 97 && charCode <= 122 || 95 === charCode || charCode >= 48 && charCode <= 57;
    }
    // [3.1 Lexical Conventions](http://www.lua.org/manual/5.2/manual.html#3.1)
    //
    // `true`, `false` and `nil` will not be considered keywords, but literals.
    function isKeyword(id) {
        switch(id.length){
            case 2:
                return 'do' === id || 'if' === id || 'in' === id || 'or' === id;
            case 3:
                return 'and' === id || 'end' === id || 'for' === id || 'not' === id;
            case 4:
                return 'else' === id || 'goto' === id || 'then' === id;
            case 5:
                return 'break' === id || 'local' === id || 'until' === id || 'while' === id;
            case 6:
                return 'elseif' === id || 'repeat' === id || 'return' === id;
            case 8:
                return 'function' === id;
        }
        return false;
    }
    function isUnary(token) {
        if (Punctuator === token.type) return '#-~'.indexOf(token.value) >= 0;
        if (Keyword === token.type) return 'not' === token.value;
        return false;
    }
    // @TODO this needs to be rethought.
    function isCallExpression(expression) {
        switch(expression.type){
            case 'CallExpression':
            case 'TableCallExpression':
            case 'StringCallExpression':
                return true;
        }
        return false;
    }
    // Check if the token syntactically closes a block.
    function isBlockFollow(token) {
        if (EOF === token.type) return true;
        if (Keyword !== token.type) return false;
        switch(token.value){
            case 'else':
            case 'elseif':
            case 'end':
            case 'until':
                return true;
            default:
                return false;
        }
    }
    // Scope
    // -----
    // Store each block scope as a an array of identifier names. Each scope is
    // stored in an FILO-array.
    var scopes, scopeDepth, globals;
    // Create a new scope inheriting all declarations from the previous scope.
    function createScope() {
        var scope = Array.apply(null, scopes[scopeDepth++]);
        scopes.push(scope);
        if (options.onCreateScope) options.onCreateScope();
    }
    // Exit and remove the current scope.
    function destroyScope() {
        var scope = scopes.pop();
        scopeDepth--;
        if (options.onDestroyScope) options.onDestroyScope();
    }
    // Add identifier name to the current scope if it doesnt already exist.
    function scopeIdentifierName(name) {
        if (-1 !== indexOf(scopes[scopeDepth], name)) return;
        scopes[scopeDepth].push(name);
    }
    // Add identifier to the current scope
    function scopeIdentifier(node) {
        scopeIdentifierName(node.name);
        attachScope(node, true);
    }
    // Attach scope information to node. If the node is global, store it in the
    // globals array so we can return the information to the user.
    function attachScope(node, isLocal) {
        if (!isLocal && -1 === indexOfObject(globals, 'name', node.name)) globals.push(node);
        node.isLocal = isLocal;
    }
    // Is the identifier name available in this scope.
    function scopeHasName(name) {
        return -1 !== indexOf(scopes[scopeDepth], name);
    }
    // Location tracking
    // -----------------
    //
    // Locations are stored in FILO-array as a `Marker` object consisting of both
    // `loc` and `range` data. Once a `Marker` is popped off the list an end
    // location is added and the data is attached to a syntax node.
    var locations = [], trackLocations;
    function createLocationMarker() {
        return new Marker(token);
    }
    function Marker(token) {
        if (options.locations) this.loc = {
            start: {
                line: token.line,
                column: token.range[0] - token.lineStart
            },
            end: {
                line: 0,
                column: 0
            }
        };
        if (options.ranges) this.range = [
            token.range[0],
            0
        ];
    }
    // Complete the location data stored in the `Marker` by adding the location
    // of the *previous token* as an end location.
    Marker.prototype.complete = function() {
        if (options.locations) {
            this.loc.end.line = previousToken.line;
            this.loc.end.column = previousToken.range[1] - previousToken.lineStart;
        }
        if (options.ranges) this.range[1] = previousToken.range[1];
    };
    // Create a new `Marker` and add it to the FILO-array.
    function markLocation() {
        if (trackLocations) locations.push(createLocationMarker());
    }
    // Push an arbitrary `Marker` object onto the FILO-array.
    function pushLocation(marker) {
        if (trackLocations) locations.push(marker);
    }
    // Parse functions
    // ---------------
    // Chunk is the main program object. Syntactically it's the same as a block.
    //
    //     chunk ::= block
    function parseChunk() {
        next();
        markLocation();
        if (options.scope) createScope();
        var body = parseBlock();
        if (options.scope) destroyScope();
        if (EOF !== token.type) unexpected(token);
        // If the body is empty no previousToken exists when finishNode runs.
        if (trackLocations && !body.length) previousToken = token;
        return finishNode(ast.chunk(body));
    }
    // A block contains a list of statements with an optional return statement
    // as its last statement.
    //
    //     block ::= {stat} [retstat]
    function parseBlock(terminator) {
        var block = [], statement;
        while(!isBlockFollow(token)){
            // Return has to be the last statement in a block.
            if ('return' === token.value) {
                block.push(parseStatement());
                break;
            }
            statement = parseStatement();
            // Statements are only added if they are returned, this allows us to
            // ignore some statements, such as EmptyStatement.
            if (statement) block.push(statement);
        }
        // Doesn't really need an ast node
        return block;
    }
    // There are two types of statements, simple and compound.
    //
    //     statement ::= break | goto | do | while | repeat | return
    //          | if | for | function | local | label | assignment
    //          | functioncall | ';'
    function parseStatement() {
        markLocation();
        if (Keyword === token.type) switch(token.value){
            case 'local':
                next();
                return parseLocalStatement();
            case 'if':
                next();
                return parseIfStatement();
            case 'return':
                next();
                return parseReturnStatement();
            case 'function':
                next();
                var name = parseFunctionName();
                return parseFunctionDeclaration(name);
            case 'while':
                next();
                return parseWhileStatement();
            case 'for':
                next();
                return parseForStatement();
            case 'repeat':
                next();
                return parseRepeatStatement();
            case 'break':
                next();
                return parseBreakStatement();
            case 'do':
                next();
                return parseDoStatement();
            case 'goto':
                next();
                return parseGotoStatement();
        }
        if (Punctuator === token.type) {
            if (consume('::')) return parseLabelStatement();
        }
        // Assignments memorizes the location and pushes it manually for wrapper
        // nodes. Additionally empty `;` statements should not mark a location.
        if (trackLocations) locations.pop();
        // When a `;` is encounted, simply eat it without storing it.
        if (consume(';')) return;
        return parseAssignmentOrCallStatement();
    }
    // ## Statements
    //     label ::= '::' Name '::'
    function parseLabelStatement() {
        var name = token.value, label = parseIdentifier();
        if (options.scope) {
            scopeIdentifierName('::' + name + '::');
            attachScope(label, true);
        }
        expect('::');
        return finishNode(ast.labelStatement(label));
    }
    //     break ::= 'break'
    function parseBreakStatement() {
        return finishNode(ast.breakStatement());
    }
    //     goto ::= 'goto' Name
    function parseGotoStatement() {
        var name = token.value, label = parseIdentifier();
        return finishNode(ast.gotoStatement(label));
    }
    //     do ::= 'do' block 'end'
    function parseDoStatement() {
        if (options.scope) createScope();
        var body = parseBlock();
        if (options.scope) destroyScope();
        expect('end');
        return finishNode(ast.doStatement(body));
    }
    //     while ::= 'while' exp 'do' block 'end'
    function parseWhileStatement() {
        var condition = parseExpectedExpression();
        expect('do');
        if (options.scope) createScope();
        var body = parseBlock();
        if (options.scope) destroyScope();
        expect('end');
        return finishNode(ast.whileStatement(condition, body));
    }
    //     repeat ::= 'repeat' block 'until' exp
    function parseRepeatStatement() {
        if (options.scope) createScope();
        var body = parseBlock();
        expect('until');
        var condition = parseExpectedExpression();
        if (options.scope) destroyScope();
        return finishNode(ast.repeatStatement(condition, body));
    }
    //     retstat ::= 'return' [exp {',' exp}] [';']
    function parseReturnStatement() {
        var expressions = [];
        if ('end' !== token.value) {
            var expression = parseExpression();
            if (null != expression) expressions.push(expression);
            while(consume(',')){
                expression = parseExpectedExpression();
                expressions.push(expression);
            }
            consume(';'); // grammar tells us ; is optional here.
        }
        return finishNode(ast.returnStatement(expressions));
    }
    //     if ::= 'if' exp 'then' block {elif} ['else' block] 'end'
    //     elif ::= 'elseif' exp 'then' block
    function parseIfStatement() {
        var clauses = [], condition, body, marker;
        // IfClauses begin at the same location as the parent IfStatement.
        // It ends at the start of `end`, `else`, or `elseif`.
        if (trackLocations) {
            marker = locations[locations.length - 1];
            locations.push(marker);
        }
        condition = parseExpectedExpression();
        expect('then');
        if (options.scope) createScope();
        body = parseBlock();
        if (options.scope) destroyScope();
        clauses.push(finishNode(ast.ifClause(condition, body)));
        if (trackLocations) marker = createLocationMarker();
        while(consume('elseif')){
            pushLocation(marker);
            condition = parseExpectedExpression();
            expect('then');
            if (options.scope) createScope();
            body = parseBlock();
            if (options.scope) destroyScope();
            clauses.push(finishNode(ast.elseifClause(condition, body)));
            if (trackLocations) marker = createLocationMarker();
        }
        if (consume('else')) {
            // Include the `else` in the location of ElseClause.
            if (trackLocations) {
                marker = new Marker(previousToken);
                locations.push(marker);
            }
            if (options.scope) createScope();
            body = parseBlock();
            if (options.scope) destroyScope();
            clauses.push(finishNode(ast.elseClause(body)));
        }
        expect('end');
        return finishNode(ast.ifStatement(clauses));
    }
    // There are two types of for statements, generic and numeric.
    //
    //     for ::= Name '=' exp ',' exp [',' exp] 'do' block 'end'
    //     for ::= namelist 'in' explist 'do' block 'end'
    //     namelist ::= Name {',' Name}
    //     explist ::= exp {',' exp}
    function parseForStatement() {
        var variable = parseIdentifier(), body;
        // The start-identifier is local.
        if (options.scope) {
            createScope();
            scopeIdentifier(variable);
        }
        // If the first expression is followed by a `=` punctuator, this is a
        // Numeric For Statement.
        if (consume('=')) {
            // Start expression
            var start = parseExpectedExpression();
            expect(',');
            // End expression
            var end = parseExpectedExpression();
            // Optional step expression
            var step = consume(',') ? parseExpectedExpression() : null;
            expect('do');
            body = parseBlock();
            expect('end');
            if (options.scope) destroyScope();
            return finishNode(ast.forNumericStatement(variable, start, end, step, body));
        } else {
            // The namelist can contain one or more identifiers.
            var variables = [
                variable
            ];
            while(consume(',')){
                variable = parseIdentifier();
                // Each variable in the namelist is locally scoped.
                if (options.scope) scopeIdentifier(variable);
                variables.push(variable);
            }
            expect('in');
            var iterators = [];
            // One or more expressions in the explist.
            do {
                var expression = parseExpectedExpression();
                iterators.push(expression);
            }while (consume(','));
            expect('do');
            body = parseBlock();
            expect('end');
            if (options.scope) destroyScope();
            return finishNode(ast.forGenericStatement(variables, iterators, body));
        }
    }
    // Local statements can either be variable assignments or function
    // definitions. If a function definition is found, it will be delegated to
    // `parseFunctionDeclaration()` with the isLocal flag.
    //
    // This AST structure might change into a local assignment with a function
    // child.
    //
    //     local ::= 'local' 'function' Name funcdecl
    //        | 'local' Name {',' Name} ['=' exp {',' exp}]
    function parseLocalStatement() {
        var name;
        if (Identifier === token.type) {
            var variables = [], init = [];
            do {
                name = parseIdentifier();
                variables.push(name);
            }while (consume(','));
            if (consume('=')) do {
                var expression = parseExpectedExpression();
                init.push(expression);
            }while (consume(','));
            // Declarations doesn't exist before the statement has been evaluated.
            // Therefore assignments can't use their declarator. And the identifiers
            // shouldn't be added to the scope until the statement is complete.
            if (options.scope) for(var i = 0, l = variables.length; i < l; i++)scopeIdentifier(variables[i]);
            return finishNode(ast.localStatement(variables, init));
        }
        if (consume('function')) {
            name = parseIdentifier();
            if (options.scope) {
                scopeIdentifier(name);
                createScope();
            }
            // MemberExpressions are not allowed in local function statements.
            return parseFunctionDeclaration(name, true);
        } else raiseUnexpectedToken('<name>', token);
    }
    function validateVar(node) {
        // @TODO we need something not dependent on the exact AST used. see also isCallExpression()
        if (node.inParens || [
            'Identifier',
            'MemberExpression',
            'IndexExpression'
        ].indexOf(node.type) === -1) raise(token, errors.invalidVar, token.value);
    }
    //     assignment ::= varlist '=' explist
    //     var ::= Name | prefixexp '[' exp ']' | prefixexp '.' Name
    //     varlist ::= var {',' var}
    //     explist ::= exp {',' exp}
    //
    //     call ::= callexp
    //     callexp ::= prefixexp args | prefixexp ':' Name args
    function parseAssignmentOrCallStatement() {
        // Keep a reference to the previous token for better error messages in case
        // of invalid statement
        var previous = token, expression, marker;
        if (trackLocations) marker = createLocationMarker();
        expression = parsePrefixExpression();
        if (null == expression) return unexpected(token);
        if (',='.indexOf(token.value) >= 0) {
            var variables = [
                expression
            ], init = [], exp;
            validateVar(expression);
            while(consume(',')){
                exp = parsePrefixExpression();
                if (null == exp) raiseUnexpectedToken('<expression>', token);
                validateVar(exp);
                variables.push(exp);
            }
            expect('=');
            do {
                exp = parseExpectedExpression();
                init.push(exp);
            }while (consume(','));
            pushLocation(marker);
            return finishNode(ast.assignmentStatement(variables, init));
        }
        if (isCallExpression(expression)) {
            pushLocation(marker);
            return finishNode(ast.callStatement(expression));
        }
        // The prefix expression was neither part of an assignment or a
        // callstatement, however as it was valid it's been consumed, so raise
        // the exception on the previous token to provide a helpful message.
        return unexpected(previous);
    }
    // ### Non-statements
    //     Identifier ::= Name
    function parseIdentifier() {
        markLocation();
        var identifier = token.value;
        if (Identifier !== token.type) raiseUnexpectedToken('<name>', token);
        next();
        return finishNode(ast.identifier(identifier));
    }
    // Parse the functions parameters and body block. The name should already
    // have been parsed and passed to this declaration function. By separating
    // this we allow for anonymous functions in expressions.
    //
    // For local functions there's a boolean parameter which needs to be set
    // when parsing the declaration.
    //
    //     funcdecl ::= '(' [parlist] ')' block 'end'
    //     parlist ::= Name {',' Name} | [',' '...'] | '...'
    function parseFunctionDeclaration(name, isLocal) {
        var parameters = [];
        expect('(');
        // The declaration has arguments
        if (!consume(')')) // Arguments are a comma separated list of identifiers, optionally ending
        // with a vararg.
        while(true){
            if (Identifier === token.type) {
                var parameter = parseIdentifier();
                // Function parameters are local.
                if (options.scope) scopeIdentifier(parameter);
                parameters.push(parameter);
                if (consume(',')) continue;
                else if (consume(')')) break;
            } else if (VarargLiteral === token.type) {
                parameters.push(parsePrimaryExpression());
                expect(')');
                break;
            } else raiseUnexpectedToken('<name> or \'...\'', token);
        }
        var body = parseBlock();
        expect('end');
        if (options.scope) destroyScope();
        isLocal = isLocal || false;
        return finishNode(ast.functionStatement(name, parameters, isLocal, body));
    }
    // Parse the function name as identifiers and member expressions.
    //
    //     Name {'.' Name} [':' Name]
    function parseFunctionName() {
        var base, name, marker;
        if (trackLocations) marker = createLocationMarker();
        base = parseIdentifier();
        if (options.scope) {
            attachScope(base, scopeHasName(base.name));
            createScope();
        }
        while(consume('.')){
            pushLocation(marker);
            name = parseIdentifier();
            base = finishNode(ast.memberExpression(base, '.', name));
        }
        if (consume(':')) {
            pushLocation(marker);
            name = parseIdentifier();
            base = finishNode(ast.memberExpression(base, ':', name));
            if (options.scope) scopeIdentifierName('self');
        }
        return base;
    }
    //     tableconstructor ::= '{' [fieldlist] '}'
    //     fieldlist ::= field {fieldsep field} fieldsep
    //     field ::= '[' exp ']' '=' exp | Name = 'exp' | exp
    //
    //     fieldsep ::= ',' | ';'
    function parseTableConstructor() {
        var fields = [], key, value;
        while(true){
            markLocation();
            if (Punctuator === token.type && consume('[')) {
                key = parseExpectedExpression();
                expect(']');
                expect('=');
                value = parseExpectedExpression();
                fields.push(finishNode(ast.tableKey(key, value)));
            } else if (Identifier === token.type) {
                if ('=' === lookahead.value) {
                    key = parseIdentifier();
                    next();
                    value = parseExpectedExpression();
                    fields.push(finishNode(ast.tableKeyString(key, value)));
                } else {
                    value = parseExpectedExpression();
                    fields.push(finishNode(ast.tableValue(value)));
                }
            } else {
                if (null == (value = parseExpression())) {
                    locations.pop();
                    break;
                }
                fields.push(finishNode(ast.tableValue(value)));
            }
            if (',;'.indexOf(token.value) >= 0) {
                next();
                continue;
            }
            break;
        }
        expect('}');
        return finishNode(ast.tableConstructorExpression(fields));
    }
    // Expression parser
    // -----------------
    //
    // Expressions are evaluated and always return a value. If nothing is
    // matched null will be returned.
    //
    //     exp ::= (unop exp | primary | prefixexp ) { binop exp }
    //
    //     primary ::= nil | false | true | Number | String | '...'
    //          | functiondef | tableconstructor
    //
    //     prefixexp ::= (Name | '(' exp ')' ) { '[' exp ']'
    //          | '.' Name | ':' Name args | args }
    //
    function parseExpression() {
        var expression = parseSubExpression(0);
        return expression;
    }
    // Parse an expression expecting it to be valid.
    function parseExpectedExpression() {
        var expression = parseExpression();
        if (null == expression) raiseUnexpectedToken('<expression>', token);
        else return expression;
    }
    // Return the precedence priority of the operator.
    //
    // As unary `-` can't be distinguished from binary `-`, unary precedence
    // isn't described in this table but in `parseSubExpression()` itself.
    //
    // As this function gets hit on every expression it's been optimized due to
    // the expensive CompareICStub which took ~8% of the parse time.
    function binaryPrecedence(operator) {
        var charCode = operator.charCodeAt(0), length = operator.length;
        if (1 === length) switch(charCode){
            case 94:
                return 12; // ^
            case 42:
            case 47:
            case 37:
                return 10; // * / %
            case 43:
            case 45:
                return 9; // + -
            case 38:
                return 6; // &
            case 126:
                return 5; // ~
            case 124:
                return 4; // |
            case 60:
            case 62:
                return 3; // < >
        }
        else if (2 === length) switch(charCode){
            case 47:
                return 10; // //
            case 46:
                return 8; // ..
            case 60:
            case 62:
                if ('<<' === operator || '>>' === operator) return 7; // << >>
                return 3; // <= >=
            case 61:
            case 126:
                return 3; // == ~=
            case 111:
                return 1; // or
        }
        else if (97 === charCode && 'and' === operator) return 2;
        return 0;
    }
    // Implement an operator-precedence parser to handle binary operator
    // precedence.
    //
    // We use this algorithm because it's compact, it's fast and Lua core uses
    // the same so we can be sure our expressions are parsed in the same manner
    // without excessive amounts of tests.
    //
    //     exp ::= (unop exp | primary | prefixexp ) { binop exp }
    function parseSubExpression(minPrecedence) {
        var operator = token.value, expression, marker;
        if (trackLocations) marker = createLocationMarker();
        // UnaryExpression
        if (isUnary(token)) {
            markLocation();
            next();
            var argument = parseSubExpression(10);
            if (argument == null) raiseUnexpectedToken('<expression>', token);
            expression = finishNode(ast.unaryExpression(operator, argument));
        }
        if (null == expression) {
            // PrimaryExpression
            expression = parsePrimaryExpression();
            // PrefixExpression
            if (null == expression) expression = parsePrefixExpression();
        }
        // This is not a valid left hand expression.
        if (null == expression) return null;
        var precedence;
        while(true){
            operator = token.value;
            precedence = Punctuator === token.type || Keyword === token.type ? binaryPrecedence(operator) : 0;
            if (precedence === 0 || precedence <= minPrecedence) break;
            // Right-hand precedence operators
            if ('^' === operator || '..' === operator) precedence--;
            next();
            var right = parseSubExpression(precedence);
            if (null == right) raiseUnexpectedToken('<expression>', token);
            // Push in the marker created before the loop to wrap its entirety.
            if (trackLocations) locations.push(marker);
            expression = finishNode(ast.binaryExpression(operator, expression, right));
        }
        return expression;
    }
    //     prefixexp ::= prefix {suffix}
    //     prefix ::= Name | '(' exp ')'
    //     suffix ::= '[' exp ']' | '.' Name | ':' Name args | args
    //
    //     args ::= '(' [explist] ')' | tableconstructor | String
    function parsePrefixExpression() {
        var base, name, marker;
        if (trackLocations) marker = createLocationMarker();
        // The prefix
        if (Identifier === token.type) {
            name = token.value;
            base = parseIdentifier();
            // Set the parent scope.
            if (options.scope) attachScope(base, scopeHasName(name));
        } else if (consume('(')) {
            base = parseExpectedExpression();
            expect(')');
            base.inParens = true; // XXX: quick and dirty. needed for validateVar
        } else return null;
        // The suffix
        var expression, identifier;
        while(true){
            if (Punctuator === token.type) switch(token.value){
                case '[':
                    pushLocation(marker);
                    next();
                    expression = parseExpectedExpression();
                    base = finishNode(ast.indexExpression(base, expression));
                    expect(']');
                    break;
                case '.':
                    pushLocation(marker);
                    next();
                    identifier = parseIdentifier();
                    base = finishNode(ast.memberExpression(base, '.', identifier));
                    break;
                case ':':
                    pushLocation(marker);
                    next();
                    identifier = parseIdentifier();
                    base = finishNode(ast.memberExpression(base, ':', identifier));
                    // Once a : is found, this has to be a CallExpression, otherwise
                    // throw an error.
                    pushLocation(marker);
                    base = parseCallExpression(base);
                    break;
                case '(':
                case '{':
                    pushLocation(marker);
                    base = parseCallExpression(base);
                    break;
                default:
                    return base;
            }
            else if (StringLiteral === token.type) {
                pushLocation(marker);
                base = parseCallExpression(base);
            } else break;
        }
        return base;
    }
    //     args ::= '(' [explist] ')' | tableconstructor | String
    function parseCallExpression(base) {
        if (Punctuator === token.type) switch(token.value){
            case '(':
                next();
                // List of expressions
                var expressions = [];
                var expression = parseExpression();
                if (null != expression) expressions.push(expression);
                while(consume(',')){
                    expression = parseExpectedExpression();
                    expressions.push(expression);
                }
                expect(')');
                return finishNode(ast.callExpression(base, expressions));
            case '{':
                markLocation();
                next();
                var table = parseTableConstructor();
                return finishNode(ast.tableCallExpression(base, table));
        }
        else if (StringLiteral === token.type) return finishNode(ast.stringCallExpression(base, parsePrimaryExpression()));
        raiseUnexpectedToken('function arguments', token);
    }
    //     primary ::= String | Numeric | nil | true | false
    //          | functiondef | tableconstructor | '...'
    function parsePrimaryExpression() {
        var literals = StringLiteral | NumericLiteral | BooleanLiteral | NilLiteral | VarargLiteral, value = token.value, type = token.type, marker;
        if (trackLocations) marker = createLocationMarker();
        if (type & literals) {
            pushLocation(marker);
            var raw = input.slice(token.range[0], token.range[1]);
            next();
            return finishNode(ast.literal(type, value, raw));
        } else if (Keyword === type && 'function' === value) {
            pushLocation(marker);
            next();
            if (options.scope) createScope();
            return parseFunctionDeclaration(null);
        } else if (consume('{')) {
            pushLocation(marker);
            return parseTableConstructor();
        }
    }
    // Parser
    // ------
    // Export the main parser.
    //
    //   - `wait` Hold parsing until end() is called. Defaults to false
    //   - `comments` Store comments. Defaults to true.
    //   - `scope` Track identifier scope. Defaults to false.
    //   - `locations` Store location information. Defaults to false.
    //   - `ranges` Store the start and end character locations. Defaults to
    //     false.
    //   - `onCreateNode` Callback which will be invoked when a syntax node is
    //     created.
    //   - `onCreateScope` Callback which will be invoked when a new scope is
    //     created.
    //   - `onDestroyScope` Callback which will be invoked when the current scope
    //     is destroyed.
    //
    // Example:
    //
    //     var parser = require('luaparser');
    //     parser.parse('i = 0');
    exports1.parse = parse;
    function parse(_input, _options) {
        if ('undefined' === typeof _options && 'object' === typeof _input) {
            _options = _input;
            _input = undefined;
        }
        if (!_options) _options = {};
        input = _input || '';
        options = extend(defaultOptions, _options);
        // Rewind the lexer
        index = 0;
        line = 1;
        lineStart = 0;
        length = input.length;
        // When tracking identifier scope, initialize with an empty scope.
        scopes = [
            []
        ];
        scopeDepth = 0;
        globals = [];
        locations = [];
        if (options.comments) comments = [];
        if (!options.wait) return end();
        return exports1;
    }
    // Write to the source code buffer without beginning the parse.
    exports1.write = write;
    function write(_input) {
        input += String(_input);
        length = input.length;
        return exports1;
    }
    // Send an EOF and begin parsing.
    exports1.end = end;
    function end(_input) {
        if ('undefined' !== typeof _input) write(_input);
        // Ignore shebangs.
        if (input && input.substr(0, 2) === '#!') input = input.replace(/^.*/, function(line) {
            return line.replace(/./g, ' ');
        });
        length = input.length;
        trackLocations = options.locations || options.ranges;
        // Initialize with a lookahead token.
        lookahead = lex();
        var chunk = parseChunk();
        if (options.comments) chunk.comments = comments;
        if (options.scope) chunk.globals = globals;
        if (locations.length > 0) throw new Error('Location tracking failed. This is most likely a bug in luaparse');
        return chunk;
    }
}); /* vim: set sw=2 ts=2 et tw=79 : */ 

},{}],"gkKU3":[function(require,module,exports,__globalThis) {
exports.interopDefault = function(a) {
    return a && a.__esModule ? a : {
        default: a
    };
};
exports.defineInteropFlag = function(a) {
    Object.defineProperty(a, '__esModule', {
        value: true
    });
};
exports.exportAll = function(source, dest) {
    Object.keys(source).forEach(function(key) {
        if (key === 'default' || key === '__esModule' || Object.prototype.hasOwnProperty.call(dest, key)) return;
        Object.defineProperty(dest, key, {
            enumerable: true,
            get: function() {
                return source[key];
            }
        });
    });
    return dest;
};
exports.export = function(dest, destName, get) {
    Object.defineProperty(dest, destName, {
        enumerable: true,
        get: get
    });
};

},{}]},["32O6c","idZQs"], "idZQs", "parcelRequire94c2")

//# sourceMappingURL=index.f2c0ec77.js.map
