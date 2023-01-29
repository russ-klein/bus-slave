
Usage: ./if_gen_axi: <instance_name> <signal specification file> <base_address> [<module_name>] 

<instance_name> will be used as the name of the catapult instance 
<signal specification file> is the name of the input file contaning the signal specifications for the interface (see example.spec)
<base address> will be used as the base address for accessing the interface from a processor, used in the header file
<module_name> is the module name of the catapult component being instantiated


outputs:
    <module_name>.v - verilog for register bank
    <signal_spec_filename>.sw.h - software header file for accessing register bank
