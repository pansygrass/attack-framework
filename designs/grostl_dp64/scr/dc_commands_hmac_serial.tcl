################################################################################
# Link design
################################################################################

set search_path {rtl tb lib .}
set target_library {core_typ.db}
set link_library {* core_typ.db}

################################################################################
# Compile HDL sources
################################################################################

set compile_clock_gating_through_hierarchy true

regsub -all {\+define\+} $env(SYN_VDEF) { } def
read_file -format sverilog -define [ list $def ] {                          \
          rtl/grostl_add_constant.sv                                        \
          rtl/grostl_compress_serial.sv                                     \
          rtl/grostl_mix_bytes.sv                                           \
          rtl/grostl_sbox_logic.sv                                          \
          rtl/grostl_sbox_lut.sv                                            \
          rtl/grostl_shift_bytes.sv                                         \
          rtl/grostl_sub_bytes.sv                                           \
      }

current_design grostl_compress_serial
link

list_designs
uniquify -force

#set_clock_gating_style -sequential_cell none -num_stages 3

check_design
compile -map_effort medium

report_cell
report_area -hierarchy

################################################################################
# Generate the gate-level netlist
################################################################################

write -format verilog -hierarchy -output grostl_hmac_serial.vg

exit
