package luaformatter

import (
	"fmt"
	"strings"

	_ "embed"

	"github.com/dop251/goja"
)

//go:embed lua.js
var luaJs string

var vm *goja.Runtime

func init() {
	vm = goja.New()
	// Run the embedded JavaScript code
	_, err := vm.RunString(luaJs)
	if err != nil {
		panic(err)
	}
}

func Minify(code string) (minified string, err error) {
	// Retrieve the function from the VM
	fn := vm.Get("minify") // Replace with your actual function name
	if fn == nil {
		err = fmt.Errorf("Function not found")
		return
	}

	// Ensure it's a callable function
	callable, ok := goja.AssertFunction(fn)
	if !ok {
		err = fmt.Errorf("Value is not a function")
		return
	}

	// Call the function with a string input
	result, err := callable(goja.Undefined(), vm.ToValue(code))
	if err != nil {
		return
	}

	// Export and print the result
	minified = result.Export().(string)
	minified = strings.TrimSpace(minified)
	return
}

func Beautify(code string) (beautified string, err error) {
	// Retrieve the function from the VM
	fn := vm.Get("beautify") // Replace with your actual function name
	if fn == nil {
		err = fmt.Errorf("Function not found")
		return
	}

	// Ensure it's a callable function
	callable, ok := goja.AssertFunction(fn)
	if !ok {
		err = fmt.Errorf("Value is not a function")
		return
	}

	// Call the function with a string input
	result, err := callable(goja.Undefined(), vm.ToValue(code))
	if err != nil {
		return
	}

	// Export and print the result
	beautified = result.Export().(string)
	beautified = strings.TrimSpace(beautified)
	// remove all trailing `;`
	lines := strings.Split(beautified, "\n")
	for i, line := range lines {
		lines[i] = strings.TrimRight(line, ";")
	}
	beautified = strings.Join(lines, "\n")
	return
}
