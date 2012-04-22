# create and map work library

vlib work
vmap work work

# compile the design and testbench

vlog rtl/grostl_add_constant.sv
vlog rtl/grostl_add_constant_p.sv
vlog rtl/grostl_add_constant_q.sv
vlog rtl/grostl_compress_serial.sv
vlog rtl/grostl_mix_bytes.sv
vlog rtl/grostl_sbox_lut.sv
vlog rtl/grostl_shift_bytes.sv
vlog rtl/grostl_shift_bytes_p.sv
vlog rtl/grostl_shift_bytes_q.sv
vlog rtl/grostl_sub_bytes.sv
vlog tb/test_compress_serial.sv

# invoke the simulator

vsim -voptargs="+acc" -t 1ns -lib work work.testbench

# add relevent waveforms to the wave window

add wave -noupdate -format Logic -label "clk" /testbench/clk
add wave -noupdate -format Logic -label "wr_m" /testbench/wr_m
add wave -noupdate -format Logic -label "wr_h" /testbench/wr_h
add wave -noupdate -format Literal -radix Hexadecimal -label "sel_m" /testbench/sel_m
add wave -noupdate -format Logic -label "sel_h" /testbench/sel_h
add wave -noupdate -format Logic -label "sel_pq" /testbench/sel_pq
add wave -noupdate -divider
 
add wave -noupdate -format Literal -radix Hexadecimal -label "round" /testbench/round
add wave -noupdate -format Literal -radix Hexadecimal -label "m_in" /testbench/h_in
add wave -noupdate -format Literal -radix Hexadecimal -label "h_in" /testbench/m_in
add wave -noupdate -format Literal -radix Hexadecimal -label "state_out" /testbench/state_out
add wave -noupdate -divider

add wave -noupdate -format Literal -radix Hexadecimal -label "m_reg" /testbench/dut/m_reg
add wave -noupdate -format Literal -radix Hexadecimal -label "s_arc" /testbench/dut/s_arc
add wave -noupdate -format Literal -radix Hexadecimal -label "s_sub" /testbench/dut/s_sub
add wave -noupdate -format Literal -radix Hexadecimal -label "p_reg" /testbench/dut/p_reg
add wave -noupdate -format Literal -radix Hexadecimal -label "s_shf" /testbench/dut/s_shf
add wave -noupdate -format Literal -radix Hexadecimal -label "s_mix" /testbench/dut/s_mix
add wave -noupdate -format Literal -radix Hexadecimal -label "h_reg" /testbench/dut/h_reg
add wave -noupdate -divider

add wave -noupdate -format Literal -radix Hexadecimal -label "m_val" /testbench/dut/m_val
add wave -noupdate -format Literal -radix Hexadecimal -label "h_val" /testbench/dut/h_val
add wave -noupdate -divider

add wave -noupdate -format Literal -radix Hexadecimal -label "out_p" /testbench/dut/add_const/out_p
add wave -noupdate -format Literal -radix Hexadecimal -label "out_q" /testbench/dut/add_const/out_q
add wave -noupdate -format Literal -radix Hexadecimal -label "dout" /testbench/dut/add_const/dout
add wave -noupdate -format Logic -label "pq" /testbench/dut/add_const/pq
add wave -noupdate -divider

view wave
view structure
view signals

run 30ms

