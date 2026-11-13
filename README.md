Files in Order
Phase 1:

config.h:  contains the hyperedge model that is used accross functions

debug.cpp/h: used for debugging purposes across files

Goals.txt: Written version of instructions from pdf assignement. 

io_writer.cpp/h: Writes to CSV file and write the wire length

main.cpp: run the phase 1 of the fastplace algorithm.

Makefile: required for running and helps build project: commands make, ./fastplace (circuit), make plot BASE=(circuit)

matrix_assembler.cpp/h:create a matrix assembler.

net_model.cpp/h: builds star and net models based i/o pins and netlist. 

placement.cpp/h: calculates total wire length of system and places ports intially. 

sparse_matrix.cpp/h: builds a sparse matrix.

system_builder.cpp/h: builds system of equations for placment problem. 

The rest are files that were used to test or provided. 