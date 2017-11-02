# CSim
A MOS Switch-Level Simulator.
CSim simulates cells **without** circuit parameters, such as the sizing of transistors. To tackle the challenges caused by the lack of circuit parameters, a new transistor-level levelization algorithm is developed. SPICE simulator is used as reference simulator, CSim is accurate in a sense that the simulation results for a cell are consistent with SPICE simulation results for any transistor sizing configurations. The program was tested on all possible cells up to 4 transistors and was able to report accurate results for all levelizable cells.
