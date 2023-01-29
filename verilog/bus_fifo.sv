
`timescale 1ns/1ns

module bus_fifo
    (
        CLK,
        RESET_N,
        DATA_STROBE,
        DATA_IN,
        DATA_READY,
        DATA_OUT,
        DATA_ACK,
        FULL
    );

    parameter width = 8;
    parameter depth = 8;
    `define cwidth $clog2(depth)

    input                 CLK;
    input                 RESET_N;
    input                 DATA_STROBE;
    input  [width-1:0]    DATA_IN;
    output                DATA_READY;
    output [width-1:0]    DATA_OUT;
    input                 DATA_ACK;
    output                FULL;

    reg    [width-1:0]    fifo_mem[(1<<depth)-1:0];
    reg    [`cwidth-1:0]  input_pointer;
    reg    [`cwidth-1:0]  output_pointer;

    wire                  empty;
    wire   [`cwidth:0]    input_pointer_plus_one;

    assign empty = (input_pointer == output_pointer) ? 1'b1 : 1'b0;
    assign FULL  = (input_pointer_plus_one == output_pointer) ? 1'b1 : 1'b0;
    assign DATA_READY = !empty;

    assign input_pointer_plus_one = input_pointer + {{`cwidth-1 {1'b0}}, 1'b1};
    assign DATA_OUT = (empty) ? {width {1'b0}} : fifo_mem[output_pointer];

    always @(posedge CLK) begin
        
        if (!RESET_N) begin 
            input_pointer <= {`cwidth {1'b0}};
            output_pointer <= {`cwidth {1'b0}};
        end else begin
            if (DATA_STROBE & !FULL) begin
                fifo_mem[input_pointer] <= DATA_IN;
                input_pointer <= input_pointer + {{`cwidth-1 {1'b0}}, 1'b1};
            end
            if (DATA_ACK & !empty) begin
                output_pointer <= output_pointer + {{`cwidth-1 {1'b0}}, 1'b1};
            end
        end
    end

endmodule
