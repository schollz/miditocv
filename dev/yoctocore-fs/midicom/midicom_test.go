package midicom

import (
	"fmt"
	"testing"

	"github.com/schollz/miditocv/dev/miditocv-fs/luaformatter"
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
	err := Listen("miditocv", true)
	if err != nil {
		t.Error(err)
	}
}
