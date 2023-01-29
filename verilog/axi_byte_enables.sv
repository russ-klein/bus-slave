
`timescale 1ns/1ns

module axi_byte_enables
    (
        ADDR,
        SIZE,
        BE
    );
`ifdef BITSIZE64

    input  [11:0]   ADDR;
    input  [2:0]    SIZE;
    output [7:0]    BE;
    reg    [7:0]    LOCAL_BE;

    assign BE = LOCAL_BE;

    always @(ADDR, SIZE) begin
    case (SIZE)
    3'b000   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00000001;
                 3'b001  : LOCAL_BE <= 8'b00000010;
                 3'b010  : LOCAL_BE <= 8'b00000100;
                 3'b011  : LOCAL_BE <= 8'b00001000;
                 3'b100  : LOCAL_BE <= 8'b00010000;
                 3'b101  : LOCAL_BE <= 8'b00100000;
                 3'b110  : LOCAL_BE <= 8'b01000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
   
    3'b001   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00000011;
                 3'b001  : LOCAL_BE <= 8'b00000110;
                 3'b010  : LOCAL_BE <= 8'b00001100;
                 3'b011  : LOCAL_BE <= 8'b00011000;
                 3'b100  : LOCAL_BE <= 8'b00110000;
                 3'b101  : LOCAL_BE <= 8'b01100000;
                 3'b110  : LOCAL_BE <= 8'b11000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
       
    3'b010   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00001111;
                 3'b001  : LOCAL_BE <= 8'b00011110;
                 3'b010  : LOCAL_BE <= 8'b00111100;
                 3'b011  : LOCAL_BE <= 8'b01111000;
                 3'b100  : LOCAL_BE <= 8'b11110000;
                 3'b101  : LOCAL_BE <= 8'b11100000;
                 3'b110  : LOCAL_BE <= 8'b11000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
      
    default : begin
                 LOCAL_BE <= 8'b11111111;
              end
       
    endcase
    end

`else

    input  [11:0]   ADDR;
    input  [2:0]    SIZE;
    output [15:0]   BE;
    reg    [15:0]   LOCAL_BE;

    assign BE = LOCAL_BE;

    always @(ADDR, SIZE) begin
    case (SIZE)
    3'b000   : begin
                 case (ADDR[3:0])
                 4'b0000  : LOCAL_BE <= 16'b0000000000000001;
                 4'b0001  : LOCAL_BE <= 16'b0000000000000010;
                 4'b0010  : LOCAL_BE <= 16'b0000000000000100;
                 4'b0011  : LOCAL_BE <= 16'b0000000000001000;
                 4'b0100  : LOCAL_BE <= 16'b0000000000010000;
                 4'b0101  : LOCAL_BE <= 16'b0000000000100000;
                 4'b0110  : LOCAL_BE <= 16'b0000000001000000;
                 4'b0111  : LOCAL_BE <= 16'b0000000010000000;
                 4'b1000  : LOCAL_BE <= 16'b0000000100000000;
                 4'b1001  : LOCAL_BE <= 16'b0000001000000000;
                 4'b1010  : LOCAL_BE <= 16'b0000010000000000;
                 4'b1011  : LOCAL_BE <= 16'b0000100000000000;
                 4'b1100  : LOCAL_BE <= 16'b0001000000000000;
                 4'b1101  : LOCAL_BE <= 16'b0010000000000000;
                 4'b1110  : LOCAL_BE <= 16'b0100000000000000;
                 4'b1111  : LOCAL_BE <= 16'b1000000000000000;
                 default  : LOCAL_BE <= 16'b1111111111111111;
                 endcase
              end
   
    3'b001   : begin
                 case (ADDR[3:0])
                 4'b0000  : LOCAL_BE <= 16'b0000000000000011;
                 4'b0001  : LOCAL_BE <= 16'b0000000000000110;
                 4'b0010  : LOCAL_BE <= 16'b0000000000001100;
                 4'b0011  : LOCAL_BE <= 16'b0000000000011000;
                 4'b0100  : LOCAL_BE <= 16'b0000000000110000;
                 4'b0101  : LOCAL_BE <= 16'b0000000001100000;
                 4'b0110  : LOCAL_BE <= 16'b0000000011000000;
                 4'b0111  : LOCAL_BE <= 16'b0000000110000000;
                 4'b1000  : LOCAL_BE <= 16'b0000001100000000;
                 4'b1001  : LOCAL_BE <= 16'b0000011000000000;
                 4'b1010  : LOCAL_BE <= 16'b0000110000000000;
                 4'b1011  : LOCAL_BE <= 16'b0001100000000000;
                 4'b1100  : LOCAL_BE <= 16'b0011000000000000;
                 4'b1101  : LOCAL_BE <= 16'b0110000000000000;
                 4'b1110  : LOCAL_BE <= 16'b1100000000000000;
                 4'b1111  : LOCAL_BE <= 16'b1000000000000000;
                 default  : LOCAL_BE <= 16'b1111111111111111;
                 endcase
              end
       
    3'b010   : begin
                 case (ADDR[3:0])
                 4'b0000  : LOCAL_BE <= 16'b0000000000001111;
                 4'b0001  : LOCAL_BE <= 16'b0000000000011110;
                 4'b0010  : LOCAL_BE <= 16'b0000000000111100;
                 4'b0011  : LOCAL_BE <= 16'b0000000001111000;
                 4'b0100  : LOCAL_BE <= 16'b0000000011110000;
                 4'b0101  : LOCAL_BE <= 16'b0000000111100000;
                 4'b0110  : LOCAL_BE <= 16'b0000001111000000;
                 4'b0111  : LOCAL_BE <= 16'b0000011110000000;
                 4'b1000  : LOCAL_BE <= 16'b0000111100000000;
                 4'b1001  : LOCAL_BE <= 16'b0001111000000000;
                 4'b1010  : LOCAL_BE <= 16'b0011110000000000;
                 4'b1011  : LOCAL_BE <= 16'b0111100000000000;
                 4'b1100  : LOCAL_BE <= 16'b1111000000000000;
                 4'b1101  : LOCAL_BE <= 16'b1110000000000000;
                 4'b1110  : LOCAL_BE <= 16'b1100000000000000;
                 4'b1111  : LOCAL_BE <= 16'b1000000000000000;
                 default  : LOCAL_BE <= 16'b1111111111111111;
                 endcase
              end
      
    3'b011   : begin
                 case (ADDR[3:0])
                 4'b0000  : LOCAL_BE <= 16'b0000000011111111;
                 4'b0001  : LOCAL_BE <= 16'b0000000111111110;
                 4'b0010  : LOCAL_BE <= 16'b0000001111111100;
                 4'b0011  : LOCAL_BE <= 16'b0000011111111000;
                 4'b0100  : LOCAL_BE <= 16'b0000111111110000;
                 4'b0101  : LOCAL_BE <= 16'b0001111111100000;
                 4'b0110  : LOCAL_BE <= 16'b0011111111000000;
                 4'b0111  : LOCAL_BE <= 16'b0111111110000000;
                 4'b1000  : LOCAL_BE <= 16'b1111111100000000;
                 4'b1001  : LOCAL_BE <= 16'b1111111000000000;
                 4'b1010  : LOCAL_BE <= 16'b1111110000000000;
                 4'b1011  : LOCAL_BE <= 16'b1111100000000000;
                 4'b1100  : LOCAL_BE <= 16'b1111000000000000;
                 4'b1101  : LOCAL_BE <= 16'b1110000000000000;
                 4'b1110  : LOCAL_BE <= 16'b1100000000000000;
                 4'b1111  : LOCAL_BE <= 16'b1000000000000000;
                 default  : LOCAL_BE <= 16'b1111111111111111;
                 endcase
              end
      
    default : begin
                 LOCAL_BE <= 16'b1111111111111111;
              end
       
    endcase
    end

`endif

endmodule
