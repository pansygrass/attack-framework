# create and map work library

vlib work
vmap work work

# compile the design and testbench

vlog rtl/rijndael_sbox_masked.sv                \
     rtl/rijndael_simple_func_m.sv              \
     tb/test_simple_func_m.sv                   \
     $env(SYN_VDEF)

# invoke the simulator

vsim $env(SYN_PLUS) -voptargs="+acc" -t 1ns -lib work work.testbench

# add relevent waveforms to the wave window

add wave -noupdate -format Logic -label "clk" /testbench/dut/clk
add wave -noupdate -format Logic -label "rst" /testbench/dut/rst
add wave -noupdate -format Logic -label "valid" /testbench/dut/valid
add wave -noupdate -format Literal -radix Hexadecimal -label "din" /testbench/dut/din
add wave -noupdate -format Literal -radix Hexadecimal -label "key" /testbench/dut/key
add wave -noupdate -format Literal -radix Hexadecimal -label "imask" /testbench/dut/imask
add wave -noupdate -format Literal -radix Hexadecimal -label "omask" /testbench/dut/omask
add wave -noupdate -format Literal -radix Hexadecimal -label "dout" /testbench/dut/dout
add wave -noupdate -divider

add wave -noupdate -format Literal -label "cs" /testbench/dut/cs
add wave -noupdate -format Literal -label "ns" /testbench/dut/ns
add wave -noupdate -format Literal -radix Hexadecimal -label "data_reg" /testbench/dut/data_reg
add wave -noupdate -format Literal -radix Hexadecimal -label "data_reg" /testbench/dut/imask_reg
add wave -noupdate -format Literal -radix Hexadecimal -label "data_reg" /testbench/dut/omask_reg
add wave -noupdate -divider

view wave
view structure
view signals

run 30ms

