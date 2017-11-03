# Solvers
- **Accurate Solver** is the implementation for [5-value system](#5_value)
- **Weak Precise Solver** is the implementation for [7-value system](#7_value)

## Different value systems

- [3-value system](#3_value)
- [5-value system](#5_value)
- [7-value system](#7_value)

<a name="3_value"></a>
### 3-value system `0, 1, and U`
For a conventional switch-level simulator, the value set would contain two values, namely logic 0 and logic 1. However, in our simulator, due to the lack of circuit parameters, CSim may not be able to compute the value of the output for some circuit configurations under certain input vectors. For example, the output value for a pseudo-nMOS invetor would be unknown if the input is logic 0. Hence, in CSim, we started with a 3-value system, i.e., `0` represent logic 0, `1` represent logic 1, and `U` represent unknown values.

The definition of each value is shown in the following table.

Symbolic value | Range of SPICE simulation result
--- | ---
0 | [0, V_tn]
U | (V_tn, VDD-V_tp)
1 | [VDD-V_tp, VDD]

The accuracy of CSim is defined by using SPICE simulator as a reference simulator. If CSim reports logic 0, it means that SPICE simulation would report logic 0 for any sizing combination, similarly, if CSim reports logic 1, then SPICE would report logic 1 for all possible sizing cases. If CSim reports U, then SPICE could report any value, in addition, different values may be reported for different sizing combinations or the same value could be reported for all sizing combinations.

With the 3-value system, CSim is able to report accurate simulation results. However, the simulation result would contain too many unknown value `U` due to 1) weak signal (e.g., passing logic 0 through PMOS); 2) conducting path to both logic 0 and logic 1 (e.g., pseudo-nMOS invetor); 3) no conducting path to any source (e.g., dynamic invertor).

<a name="5_value"></a>
### 5-value system `0, 1, R, Z, and U`

The `U` value in the 3-value system hides too much information and causes the simulation result to be imprecise. By using special composite values such as `R` to denote ratioed value and `Z` to denote high−impedance value, the precision of our simulator is improved while guaranteeing correctness.
If the value of a node can potentially be logic 0 for some cases and logic 1 for some other cases, then the value of that node is defined as `R`. If a node is not affected by any other nodes, then the value of that node would be defined as `Z`.

Symbolic value | Range of SPICE simulation result
--- | ---
0 | [0, V_tn]
U | (V_tn, VDD-V_tp)
1 | [VDD-V_tp, VDD]
R | [0, VDD]
Z | [0, VDD]

The following figure shows value at node Z given value of X and Y when both transistors are conducting.

<img src="http://128.125.225.145/value_circuit.png" alt="value system circuit" width="300"/>

<img src="http://128.125.225.145/value_5.png" alt="value system circuit" width="300"/>

`R` and `Z` do not affect the accuracy of the simulator, because when SPICE simulator generates either logic 0 or logic 1, it will still be consistent with `R` and `Z`.

`U` value caused by weak signals can be conquered by either introducing new values (i.e., [7-value system](#7_value)) or simply assuming that weak values can be restored in place by using additional transistors (e.g., inserting buffers). The latter approach gives designer an insight about potemtial logic that can be realized by a circuit, CSim 5-value system follows the latter approach.

<a name="7_value"></a>
### 7-value system `0, 1, o, i, R, Z, and U`

To improve the precision in case of weak values, two more symbols are added to the [5-value system](#5_value) (i.e., weak logic 0 `o` and weak logic 1 `i`).

Symbolic value | Range of SPICE simulation result
--- | ---
0 | [0, V_tn]
o | (V_tn, V_IL]
U | (V_IL, VDD-V_IH)
i | [VDD-V_IH, VDD-V_tp)
1 | [VDD-V_tp, VDD]
R | [0, VDD]
Z | [0, VDD]

The following figure shows value at node Z given value of X and Y when both transistors are conducting.

<img src="http://128.125.225.145/value_circuit.png" alt="value system circuit" width="300"/>

<img src="http://128.125.225.145/value_7.png" alt="7-value system" width="300"/>
