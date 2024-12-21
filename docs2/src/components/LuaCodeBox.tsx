import React, { useState, useEffect } from "react";
import CodeMirror from "@uiw/react-codemirror";
import { StreamLanguage } from "@codemirror/language";
import { lua } from "@codemirror/legacy-modes/mode/lua";

const LuaCodeBox = () => {
  const [code, setCode] = useState(`-- Example Lua code
local function greet(name)
  print("Hello, " .. name .. "!")
end

greet("World")`);
  const [output, setOutput] = useState(""); // State to hold the output

  useEffect(() => {
    // Check if the script is already added to avoid duplication
    if (!document.querySelector("script[data-persistent-luajs]")) {
      const script = document.createElement("script");
      script.type = "module";
      script.setAttribute("data-persistent-luajs", "true"); // Marker to identify the script
      script.textContent = `
        import emscriptenInit from '/luavm/luajs.mjs';
        const LuaJS = await emscriptenInit();
        window.LuaJS = LuaJS;
        window.luaState = LuaJS.newState();
      `;
      document.body.appendChild(script);
    }
  }, []); // Empty dependency array ensures this runs only once

  const handleRun = () => {
    console.log("Code content:", code);
    // Run the Lua code and handle the output
    window.luaState.then(async (L) => {
      let value;
      try {
        value = await L.run(code); // Execute Lua code
        setOutput(value || ""); // Update the output state
      } catch (error) {
        setOutput(`Error: ${error.message || error}`); // Handle errors
      }
    });
  };

  return (
    <div style={{ margin: "2rem 0" }}>
      <h3>Lua Code Example</h3>
      <CodeMirror
        value={code}
        height="200px"
        theme="light"
        extensions={[StreamLanguage.define(lua)]}
        onChange={(value) => {
          setCode(value);
        }}
      />
      <button
        onClick={handleRun}
        style={{ marginTop: "1rem", padding: "0.5rem 1rem" }}
      >
        Run
      </button>
      <div id="output" style={{ marginTop: "1rem" }}>
        <h4>Output:</h4>
        <pre
          style={{
            background: "#f4f4f4",
            padding: "1rem",
            borderRadius: "5px",
            border: "1px solid #ddd",
            overflowX: "auto",
          }}
        >
          <code>{output}</code>
        </pre>
      </div>
    </div>
  );
};

export default LuaCodeBox;
