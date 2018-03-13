# CSim netlist syntax
CSim was developed to facilitate simulation tasks for an enumeration process. Since the netlists used by CSim are generated automatically, the syntax and format are very restricted.
**Netlist syntax is case sensitive.**

# Netlist components
## Circuit nodes
Node type | Node syntax | Example
--- | --- | ---
supply | VDD | VDD
supply | GND | GND
internal node | N\<4-digit node number> | N0001
primary input | IN\<3-digit input number> | IN001
primary output | OUT\<2-digit output number> | OUT01

## Transistors
```
M<4-digit transistor id> <diffusion node> <gate node> <diffusion node> <body node> <transistor type>
```
### Transistor types

- P-type transistor: `PMOS`
- N-type transistor: `NMOS`

### Example
```
M0001 VDD IN001 OUT01 VDD PMOS
```
## Comments
Comments are supported in the following two ways.

- line comment: Anyline starts with an `*` is ignored
- end of line comment: Any statement after the **first** `;` is ignored

## Example netlist

The following netlist describes a CMOS invertor

```spice
* This is an invertor

M0001 VDD IN001 OUT01 VDD PMOS		  ; This is the pull up network
M0002 OUT01 IN001 GND GND NMOS

* End of the file
```