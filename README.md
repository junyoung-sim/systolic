# Systolic Array Matrix Multiplier

**Spring 2025**

As an onboarding freshmen member of Digital Design at Cornell Custom Silicon Systems, I independently designed, implemented, verified, synthesized, and documented a systolic array for accelerating matrix multiplication. The project kickstarted the team’s development of a RISC-V machine learning processor architecture IP starting Fall 2025.

## Background

Matrix multiplication is ubiquitous in machine and deep learning tasks, and systolic arrays (used in Google’s TPUs) can accelerate this critical operation for two-dimensional input and weight matrices.

Systolic arrays typically consist of N x N processing elements (PE) where each PE not only performs the typical multiply-accumulate (MAC) operation but also propagates inputs, weights, and/or summations to other PEs such that every value in the matrices is constantly busy.

![fig1](https://github.com/junyoung-sim/systolic/blob/main/docs/fig1.png)

The two figures above show how data flows through a systolic array. Note that this shows an output-stationary data flow (used in our systolic array) where each PE has its own MAC output and propagates the inputs and weights to other PEs, and there are several other data flow types (e.g., input-stationary, weight-stationary, etc).

## RTL Design

### Top

![fig2](https://github.com/junyoung-sim/systolic/blob/main/docs/fig2.png)

Our systolic array uses the output-stationary data flow to multiply two 2D matrices with signed fixed point (FP) values. The following are top-level parameters and I/Os.

**Parameters**
* SIZE: number of PEs per row and per column (default: 4)
* NBITS: FP bitwidth of input and weight values (default: 16)
* DBITS: number of decimal bits of input and weight values (default: 8)

**Inputs**
* clk: clock signal
* rst: reset signal
* [NBITS-1:0] l_x_col_in [SIZE]: a column of the input matrix (zero-pad if necessary; deserializer outside of the systolic array may be helpful)
* x_recv_val: signal for indicating that an input column is being driven into the systolic array
* [NBITS-1:0] t_w_row_in [SIZE]: a row of the weight matrix (zero-pad if necessary; deserializer outside of the systolic array may be helpful)
* w_recv_val: signal for indicating that a weight row is being driven into the systolic array
* [$clog2(SIZE)-1:0] out_rsel: row index of output matrix to read
* [$clog2(SIZE)-1:0] out_csel: column index of output matrix to read

**Outputs**
* x_recv_rdy: signal indicating that an input matrix is fully loaded in the systolic array
* w_recv_rdy: signal indicating that a weight matrix is fully loaded in the systolic array
* mac_rdy: signal indicating that the systolic array is performing MAC operations
* out_rdy: signal indicating that all outputs in the systolic array are stable
* [NBITS-1:0] b_s_out: output matrix value at the specified index

### Data Path

![fig3](https://github.com/junyoung-sim/systolic/blob/main/docs/fig3.png)

For an N x N sized systolic array, there are N x N PEs that perform MAC operations and N synchronous FIFO buffers each for the input and weight matrices to store the matrices and flow the matrices into the array of PEs.

Each PE takes in one input and weight value. These values are driven into a combinational FP multiplier, and the product is accumulated to a sum register (i.e., the PE’s output). The input and weight entering each PE is also outputted through a register and delivered to adjacent PEs.

![fig4](https://github.com/junyoung-sim/systolic/blob/main/docs/fig4.png)

### Control Logic

![fig5](https://github.com/junyoung-sim/systolic/blob/main/docs/fig5.png)

**LOAD**
* Load one column of the input matrix per cycle by setting x_recv_val = 1. Input matrix can be loaded in descending or ascending order (must match weight matrix’s loading order) of columns. Zero-pad the input matrix if necessary. x_recv_rdy = 0 indicates the input matrix FIFOs are full (cannot load). x_recv_rdy = 1 indicates the input matrix FIFOs are not full (can load)
* Load one row of the weight matrix per cycle by setting w_recv_val = 1. Weight matrix can be loaded in descending or ascending order (must match input matrix’s loading order) of rows. Zero-pad the input matrix if necessary. w_recv_rdy = 0 indicates the weight matrix FIFOs are full (cannot load). w_recv_rdy = 1 indicates the weight matrix FIFOs are not full (can load)

**MAC**
* Once the input and weight FIFOs are full, the PEs are enabled to perform MAC operations (mac_rdy = 1, out_rdy = 0)
* During MAC, the input and weight FIFOs cannot be written

**OUT**
* The systolic array enters OUT state when the last PE finishes its computation and all outputs become stable (mac_rdy = 0, out_rdy = 1)
* Setting the output row (out_rsel) and column (out_csel) will combinationally dump the selected PE’s output (b_s_out). Attempting to read outputs during non-OUT states will return all zeros (out_rdy sets out_en of data path).
* During OUT, the input and weight FIFOs cannot be written
* Reset the systolic array to clear the PEs and start a new matrix multiplication (returns to LOAD)

The following waveform corresponds to an example execution of a 4x4 systolic array.

![fig6](https://github.com/junyoung-sim/systolic/blob/main/docs/fig6.png)

## Design Verification

Cocotb (Synopsys VCS) tests for the systolic array’s top-level consist of:

* Directed loading of an input matrix
* Directed loading of a weight matrix
* Directed synchronous loading of input and weight matrices
* Randomized asynchronous loading input and weight matrices
* Directed synchronous loading of input and weight matrices and full LOAD/MAC/OUT sequence
* Randomized asynchronous loading of input and weight matrices and full LOAD/MAC/OUT sequence

The following coverage report is from running all Cocotb tests on a 4x4 array (FP Q8.8).

### Top

![fig7](https://github.com/junyoung-sim/systolic/blob/main/docs/fig7.png)

### Data Path

![fig8](https://github.com/junyoung-sim/systolic/blob/main/docs/fig8.png)

### Control Logic

![fig9](https://github.com/junyoung-sim/systolic/blob/main/docs/fig9.png)

### Summary

![fig10](https://github.com/junyoung-sim/systolic/blob/main/docs/fig10.png)

Line coverage score for SystolicCtrl is due to untested default case statements within the output transition and latency counting logic. FP combinational multiplication on unsigned inputs were not tested.

## Physical Design

***Technical details beyond what is provided below cannot be shared under the non-disclosure agreement between Cornell Custom Silicon Systems, MPW (Multi-Project-Wafer) University Service (MUSE), and Taiwan Semiconductor Manufacturing Company (TSMC).***

Using our TSMC 180 nm process node, an 8x8 array with FP Q4.4 numbers might be the most reasonable and practical configuration of our systolic array.

### Synthesis

#### Area

![fig11](https://github.com/junyoung-sim/systolic/blob/main/docs/fig11.png)

#### Timing

![fig12](https://github.com/junyoung-sim/systolic/blob/main/docs/fig12.png)

### Place & Route

![fig13](https://github.com/junyoung-sim/systolic/blob/main/docs/fig13.png)
