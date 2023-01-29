 
`timescale 1ns/1ns

module ready_gen 
    ( 
        CLK,
        RESETn,
        STROBE,
        READY
    );

    parameter waits = 5;

    input CLK;
    input RESETn;
    input STROBE;
    output READY;

    reg   [11:0]    counter;
    reg   [31:0]    zero = 32'h00000000;
    reg   [31:0]    one = 32'h00000001;
    reg             local_ready;

    assign READY = (waits == 0) ? STROBE: local_ready;

    always @(posedge CLK) begin
        if (RESETn == 0) begin
            counter <= zero;
            local_ready <= 0;
        end else begin
            if (!STROBE) begin
                counter <= zero;
                local_ready <= 0;
            end else if (counter > 0) begin
                counter <= counter - one;
                if ((counter - one) == 0) local_ready <= 1;
            end else begin 
                local_ready <= 0;
                if (STROBE) begin
                    counter <= waits;
                end
            end
        end
    end
endmodule
