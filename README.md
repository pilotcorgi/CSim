# CSim

**CSim** is a MOS switch-level simulator. It simulates cells **without** circuit parameters, such as the sizing of transistors. To tackle the challenges caused by the lack of circuit parameters, a new transistor-level levelization algorithm is developed. SPICE simulator is used as reference simulator, CSim is accurate in a sense that the simulation results for a cell are consistent with SPICE simulation results for any transistor sizing configurations. The program was tested on all possible cells up to 4 transistors and was able to report accurate results for all levelizable cells.

## Installation
Note: CSim requires C++11 support.

```bash
$ make
```

## Usage

```bash
$ ./CSim <netlist_file>
```

## Quick Start
### Sample netlist file `inv.csim`
For detailed netlist syntax, please refer to [CSim Netlist Syntax](https://github.com/arkkevin/CSim/wiki/Netlist).

```spice
* This is an invertor

M0001 VDD IN001 OUT01 VDD PMOS		  ; This is the pull up network
M0002 OUT01 IN001 GND GND NMOS

* End of the file
```
### Run simulation
```
$ ./CSim inv.csim
```
### Sample output
```
--- netlist ---
M0001 VDD IN001 OUT01 VDD PMOS
M0002 OUT01 IN001 GND GND NMOS

--- simulation results ---
--- precision level 1 ---
|IN001||OUT01|
|  0  ||  1  |
|  1  ||  0  |

--- precision level 2 ---
|IN001||OUT01|
|  0  ||  1  |
|  1  ||  0  |
```
## Documentation
- [CSim Home](https://github.com/arkkevin/CSim/wiki)
- [CSim Netlist Syntax](https://github.com/arkkevin/CSim/wiki/Netlist)

# License
Licensed under the [Apache License 2.0](https://github.com/arkkevin/CSim/blob/master/LICENSE) License.