# name,  bitwidth, type, connection (wire|channel)
#
# name is any legal verilog and c identifier
# bitwidth is a number from 1 to 1024
# the signal may be signed or unsigned (this ends up in the header file)
# signal type can be "output" or "input" -- "io" is not supported
# signal connection can be "wire" or "channel" -- sync channels are not yet supported
# 
# comma delimited, basic edit checking is provided
#

# name,        bitwidth, signed,   type,  connection (wire|channel)
#
go,                   1, unsigned, input,  channel
done,                 1, unsigned, output, channel
relu,                 1, unsigned, input, wire
use_bias,             1, unsigned, input, wire
max_pool,             1, unsigned, input, wire
image_offset,        25, unsigned, input, wire
weight_offset,       25, unsigned, input, wire
bias_offset,         25, unsigned, input, wire
output_offset,       25, unsigned, input, wire
num_input_images,    25, unsigned, input, wire
num_output_images,   25, unsigned, input, wire
image_height,        25, unsigned, input, wire
image_width,         25, unsigned, input, wire
filter_height,       25, unsigned, output, wire
filter_width,        25, unsigned, output, wire
data_in,             16, unsigned, input, channel
data_out,             8, unsigned, output, channel

# if you have an axi master interface on the catapult component
# add the line below to the file.  The "bitwidth" is the number 
# of address bits for byte addresses on the memory interface 
# this is only supported for if_gen_axi

memory,              20, unsigned, input, master
