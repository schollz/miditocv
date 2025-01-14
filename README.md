
**uploading:**
```bash
cd build && make && sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program yoctocore.elf verify reset exit"
```

**setup server:**
```bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
```

**continue:**
```bash
gdb-multiarch yoctocore.elf  -ex "target remote localhost:3333" -ex "monitor reset init" -ex "continue"
```

**minicom**:
quit with `Ctrl-A` and then `Q`

```bash
minicom -b 115200 -o -D /dev/ttyACM1 
```