
`timescale 1ns/1ns

module axi_data_latch
    ( 
            CLK,
            RESETN,

            MASTER,
            ID,
            DATA,
            LAST,

            VALID,
            READY,

            O_MASTER,
            O_ID,
            O_DATA,
            O_RESP,
            O_LAST,

            O_VALID,
            O_READY
    );


    parameter masters = 4;
    parameter id_bits = 2;
    parameter data_width = 64;

`define FIFO_WIDTH (masters + id_bits + data_width + 1)

    input                   CLK;
    input                   RESETN;

    input  [masters-1:0]    MASTER;
    input  [id_bits-1:0]    ID;
    input  [data_width-1:0] DATA;
    input                   LAST;

    input                   VALID;
    output                  READY;

    output [masters-1:0]    O_MASTER;
    output [id_bits-1:0]    O_ID;
    output [data_width-1:0] O_DATA;
    output [1:0]            O_RESP;
    output                  O_LAST;

    output                  O_VALID;
    input                   O_READY;

    wire   [`FIFO_WIDTH-1:0] fifo_in;
    wire   [`FIFO_WIDTH-1:0] fifo_out;
    wire                    full;

    reg                     local_ready;
    reg                     send_req;

    reg    [31:0]           one  = 32'h00000001;
    reg    [31:0]           zero = 32'h00000000;

    reg                     fifo_ack;
    reg                     next;

    assign fifo_in = { MASTER, ID, DATA, LAST};
    assign READY = !full;
    assign { O_MASTER, O_ID, O_DATA, O_LAST } = fifo_out;

    bus_fifo #(`FIFO_WIDTH) fifo0 (
            .CLK            (CLK), 
            .RESET_N        (RESETN), 
            .DATA_STROBE    (VALID), 
            .DATA_IN        (fifo_in), 

            .DATA_READY     (O_VALID), 
            .DATA_OUT       (fifo_out), 
            .DATA_ACK       (O_READY), 
            .FULL           (full)
    );
 
endmodule
 
