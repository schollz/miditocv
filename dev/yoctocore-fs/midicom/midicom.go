package midicom

import (
	"fmt"
	"os"
	"os/signal"
	"strings"
	"time"

	log "github.com/schollz/logger"
	"gitlab.com/gomidi/midi/v2"
	"gitlab.com/gomidi/midi/v2/drivers"
	_ "gitlab.com/gomidi/midi/v2/drivers/rtmididrv"
)

var isConnected = false
var filterMidiName string
var sysexOnly bool

func isAvailable(filterMidiName string) bool {
	return strings.Contains(strings.ToLower(midi.GetInPorts().String()), strings.ToLower(filterMidiName))
}

var fileContents string
var fileContentsAsk bool
var fileContentsReady bool

func doConnection(filterMidiName string) (stop func(), err error) {
	var midiInput drivers.In
	ins := midi.GetInPorts()
	if len(ins) == 0 {
		log.Error("no input devices")
		return
	}

	for _, in := range ins {
		log.Tracef("found input: '%s'", in.String())
		if strings.Contains(strings.ToLower(in.String()), strings.ToLower(filterMidiName)) {
			midiInput = in
			break
		}
	}
	if midiInput == nil {
		log.Error("no input devices")
		return
	}

	// listen to midi
	stop, err = midi.ListenTo(midiInput, func(msg midi.Message, timestamps int32) {
		var bt []byte
		var ch, key, vel uint8
		switch {
		case msg.GetSysEx(&bt):
			s := strings.TrimSpace(string(bt))
			fmt.Printf("%s\n", s)
			if fileContentsAsk && !fileContentsReady {
				if strings.HasPrefix(s, "LS") {
					fileContents = s[4:]
				} else if strings.HasPrefix(s, "LN") {
					fileContents += s[4:]
				} else if strings.HasPrefix(s, "LE") {
					fileContents += s[4:]
					fileContentsReady = true
				}
			}
		case msg.GetNoteStart(&ch, &key, &vel) && !sysexOnly:
			log.Infof("note_on=%s, ch=%v, vel=%v\n", midi.Note(key), ch, vel)
		case msg.GetNoteEnd(&ch, &key) && !sysexOnly:
			log.Infof("note_off=%s, ch=%v\n", midi.Note(key), ch)
		default:
			// ignore
		}
	}, midi.UseSysEx(), midi.SysExBufferSize(4096))

	if err != nil {
		log.Error(err)
		return
	}

	isConnected = true
	log.Infof("connected to\n\t'%s'", midiInput.String())
	return
}

func GetFile(scene int, output int) (contents string, err error) {
	fileContentsAsk = true
	err = SendText("yoctocore", fmt.Sprintf("%d_%d_24", scene, output))
	if err != nil {
		return
	}
	fileContentsAsk = true
	startTime := time.Now()
	for {
		if time.Since(startTime) > 5*time.Second {
			err = fmt.Errorf("timeout")
			break
		}
		if fileContentsReady {
			contents = fileContents
			break
		}
		time.Sleep(100 * time.Millisecond)
	}
	fileContentsAsk = false
	fileContentsReady = false
	return
}

func SendText(name string, text string) (err error) {
	var midiOutput drivers.Out
	outs := midi.GetOutPorts()
	if len(outs) == 0 {
		log.Error("no output devices")
		return
	}

	for _, out := range outs {
		log.Tracef("found output: '%s'", out.String())
		if strings.Contains(strings.ToLower(out.String()), strings.ToLower(name)) {
			midiOutput = out
			break
		}
	}
	if midiOutput == nil {
		log.Error("no output devices")
		return
	}

	midiOutput.Open()
	defer midiOutput.Close()
	sysexMessage := []byte{0xf0}
	sysexMessage = append(sysexMessage, []byte(text)...)
	sysexMessage = append(sysexMessage, 0xf7)
	midiOutput.Send(sysexMessage)
	log.Infof("sent text to\n\t'%s'", midiOutput.String())
	return
}

func Listen(name string, sysex bool) (err error) {
	filterMidiName = name
	sysexOnly = sysex

	done := make(chan struct{})
	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	go func() {
		for sig := range c {
			done <- struct{}{}
			time.Sleep(100 * time.Millisecond)
			log.Infof("captured %v, exiting..", sig)
		}
	}()

	var stopFunc func()
	go func() {
		// check if midi is connected
		for {
			if isConnected {
				if !isAvailable(filterMidiName) {
					isConnected = false
					midi.CloseDriver()
				}
			} else {
				if isAvailable(filterMidiName) {
					stopFunc, err = doConnection(filterMidiName)
					if err != nil {
						log.Error(err)
					}
				}
			}
			time.Sleep(100 * time.Millisecond)
		}
	}()

	for {
		select {
		case <-done:
			go stopFunc()
			time.Sleep(1 * time.Second)
			return
		}
	}
	return
}
