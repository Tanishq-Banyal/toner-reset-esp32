# printer-chip-reset-esp32
esp32 program to reprogram the toner chip over i2c

**Known Issue :** cmake is not properly configured for CXX build, so build will fail after this commit.

### How to Use ?
1. Run: `git clone https://github.com/Tanishq-Banyal/printer-chip-reset-esp32.git`
2. Open ESP-IDF shell, cd to the folder made by git, then run `idf.py build`
3. Connect the board to PC, then flash using `idf.py flash monitor --port <PortNameHere>`
4. Read from a new chip to a buffer, then write back to old chip using command-line interface.