# toner-reset-esp32
esp32 program to reprogram the toner chip over i2c

### How this Works ?
The printer stores the page count inside the EEPROM chip of Toner/Cartridge.
Using this Program you can read the contents of the chip, copy it to your PC, edit it in notepad and then flash back to the chip, all via esp32.

### How to Use ?
1. Run: `git clone https://github.com/Tanishq-Banyal/printer-chip-reset-esp32.git`
2. Edit the `EEPROM_SIZE` variable in `main/main.cpp` according to your toners flash chip.
3. Open ESP-IDF shell, cd to the folder made by git, then run `idf.py build`
4. Connect the board to PC, then flash using `idf.py flash monitor --port <PortNameHere>`
5. You can now do everything using command-line interface.
