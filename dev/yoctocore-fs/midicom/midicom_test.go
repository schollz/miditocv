package midicom

import (
	"fmt"
	"testing"

	"github.com/schollz/yoctocore/dev/yoctocore-fs/luaformatter"
)

func TestMIDI(t *testing.T) {
	go func() {
		contents, err := GetFile(0, 0)
		if err != nil {
			t.Error(err)
		}
		fmt.Printf("contents: \n'%s'\n", contents)
		beautified, err := luaformatter.Beautify(contents)
		if err != nil {
			t.Error(err)
		}
		fmt.Printf("beautified: \n%s\n", beautified)
	}()
	err := Listen("yoctocore", true)
	if err != nil {
		t.Error(err)
	}
}
