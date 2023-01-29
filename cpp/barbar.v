`define bw_bits      3                                                 
                                                                       
`define m_bits       4                                                 
`define id_bits      4                                                 
`define slave_id_bits 7                                                
`define addr_bits   32                                                 
`define len_bits     8                                                 
`define data_bits    (8 * (1 << `bw_bits))                             
`define strb_bits    (`data_bits/8)                                    
`define size_bits    3                                                 
`define burst_bits   2                                                 
`define lock_bits    1                                                 
`define cache_bits   4                                                 
`define prot_bits    3                                                 
`define resp_bits    2                                                 
`define ruser_bits   7                                                 
`define wuser_bits   9                                                 
`define snoop_bits   3                                                 
`define region_bits  4                                                 
`define qos_bits     4                                                 
                                                                       
module cat_accel_foo (                                                  
  input          clock,                                                
  input          resetn,                                               
  input  [15-`bw_bits:0]  read_addr,                                            
  output [`data_bits-1:0]  read_data,                                            
  input          oe,                                                   
  input  [15-`bw_bits:0]  write_addr,                                           
  input  [`data_bits-1:0]  write_data,                                           
  input  [`strb_bits-1:0]   be,                                                   
  input          we                                                    
  ,                                                                    
  output [`id_bits-1:0]           AWID,                                
  output [`addr_bits-1:0]         AWADDR,                              
  output [`len_bits-1:0]          AWLEN,                               
  output [`size_bits-1:0]         AWSIZE,                              
  output [`burst_bits-1:0]        AWBURST,                             
  output [`lock_bits-1:0]         AWLOCK,                              
  output [`cache_bits-1:0]        AWCACHE,                             
  output [`prot_bits-1:0]         AWPROT,                              
  output [`region_bits-1:0]       AWREGION,                            
  output [`qos_bits-1:0]          AWQOS,                               
  output                          AWVALID,                             
  input                           AWREADY,                             
                                                                       
  output [`id_bits-1:0]           WID,                                 
  output [`data_bits-1:0]         WDATA,                               
  output [`strb_bits-1:0]         WSTRB,                               
  output                          WLAST,                               
  output                          WVALID,                              
  input                           WREADY,                              
                                                                       
  input  [`id_bits-1:0]           BID,                                 
  input  [`resp_bits-1:0]         BRESP,                               
  input                           BVALID,                              
  output                          BREADY,                              
                                                                       
  output [`id_bits-1:0]           ARID,                                
  output [`addr_bits-1:0]         ARADDR,                              
  output [`len_bits-1:0]          ARLEN,                               
  output [`size_bits-1:0]         ARSIZE,                              
  output [`burst_bits-1:0]        ARBURST,                             
  output [`lock_bits-1:0]         ARLOCK,                              
  output [`cache_bits-1:0]        ARCACHE,                             
  output [`prot_bits-1:0]         ARPROT,                              
  output [`region_bits-1:0]       ARREGION,                            
  output [`qos_bits-1:0]          ARQOS,                               
  output                          ARVALID,                             
  input                           ARREADY,                             
                                                                       
  input  [`id_bits-1:0]           RID,                                 
  input  [`data_bits-1:0]         RDATA,                               
  input  [`resp_bits-1:0]         RRESP,                               
  input                           RLAST,                               
  input                           RVALID,                              
  output                          RREADY                               
                                                                       
);                                                                     
                                                                       
                                                                       
 reg     [63:0]  register_bank[19:0];                                  
 reg     [63:0]  rd_reg;                                               
                                                                       
 reg             ready_out = 1'b1;                                     
 reg             resp_out = 2'b00;                                     
                                                                       
 wire    [15-`bw_bits:0]  read_address;                                
 wire    [15-`bw_bits:0]  write_address;                               
 wire            read_enable = oe;                                     
 wire            write_enable = we;                                    
                                                                       
 assign read_data = rd_reg;                                            
                                                                       
 assign read_address = read_addr[15:0];                                
 assign write_address = write_addr[15:0];                              
                                                                       
 
 // interface signals 
 
 wire         go; 
 wire         go_ready; 
 reg          go_valid; 
 wire         done; 
 reg          done_ready; 
 wire         done_valid; 
 wire         relu; 
 wire         relu_tz; 
 wire         use_bias; 
 wire         use_bias_tz; 
 wire         max_pool; 
 wire         max_pool_tz; 
 wire [ 24:0] image_offset; 
 wire         image_offset_tz; 
 wire [ 24:0] weight_offset; 
 wire         weight_offset_tz; 
 wire [ 24:0] bias_offset; 
 wire         bias_offset_tz; 
 wire [ 24:0] output_offset; 
 wire         output_offset_tz; 
 wire [ 24:0] num_input_images; 
 wire         num_input_images_tz; 
 wire [ 24:0] num_output_images; 
 wire         num_output_images_tz; 
 wire [ 24:0] image_height; 
 wire         image_height_tz; 
 wire [ 24:0] image_width; 
 wire         image_width_tz; 
 wire [ 24:0] filter_height; 
 wire         filter_height_tz; 
 wire [ 24:0] filter_width; 
 wire         filter_width_tz; 
                                     
 wire [ `addr_bits-1:0] addr_offset; 
 wire [ 31:0]           burst_size;  
 wire [  2:0]           m_wstate;    
                                     
 
 // register map 
 
 `define GO                     0 
 `define GO_READY               1 
 `define DONE                   2 
 `define DONE_VALID             3 
 `define RELU                   4 
 `define USE_BIAS               5 
 `define MAX_POOL               6 
 `define IMAGE_OFFSET           7 
 `define WEIGHT_OFFSET          8 
 `define BIAS_OFFSET            9 
 `define OUTPUT_OFFSET         10 
 `define NUM_INPUT_IMAGES      11 
 `define NUM_OUTPUT_IMAGES     12 
 `define IMAGE_HEIGHT          13 
 `define IMAGE_WIDTH           14 
 `define FILTER_HEIGHT         15 
 `define FILTER_WIDTH          16 
 `define ADDR_OFFSET_LOW       17 
 `define ADDR_OFFSET_HIGH      18 
 `define BURST_SIZE            19 
 
 
 // assignments for inputs 
 
 assign go                   = register_bank[`GO][0]; 
 assign relu                 = register_bank[`RELU][0]; 
 assign use_bias             = register_bank[`USE_BIAS][0]; 
 assign max_pool             = register_bank[`MAX_POOL][0]; 
 assign image_offset         = register_bank[`IMAGE_OFFSET][24:0]; 
 assign weight_offset        = register_bank[`WEIGHT_OFFSET][24:0]; 
 assign bias_offset          = register_bank[`BIAS_OFFSET][24:0]; 
 assign output_offset        = register_bank[`OUTPUT_OFFSET][24:0]; 
 assign num_input_images     = register_bank[`NUM_INPUT_IMAGES][24:0]; 
 assign num_output_images    = register_bank[`NUM_OUTPUT_IMAGES][24:0]; 
 assign image_height         = register_bank[`IMAGE_HEIGHT][24:0]; 
 assign image_width          = register_bank[`IMAGE_WIDTH][24:0]; 
 assign filter_height        = register_bank[`FILTER_HEIGHT][24:0]; 
 assign filter_width         = register_bank[`FILTER_WIDTH][24:0]; 
                                                                                              
 assign addr_offset[31:0]               = register_bank[`ADDR_OFFSET_LOW];                    
 assign burst_size                      = register_bank[`BURST_SIZE];                         
                                                                                              
 
                                                                       
 always @(posedge clock or negedge resetn) begin                       
   if (resetn == 1'b0) begin                                           
     rd_reg <= 32'h00000000;                                           
   end else begin                                                      
     if (read_enable) begin                                            
       rd_reg <= register_bank[read_address];                          
     end                                                               
   end                                                                 
 end                                                                   
                                                                       
 always @(posedge clock or negedge resetn) begin                       
   if (resetn == 1'b0) begin                                           
       register_bank[`GO] <= 32'h00000000; 
       register_bank[`GO_READY] <= 32'h00000000; 
       register_bank[`DONE] <= 32'h00000000; 
       register_bank[`DONE_VALID] <= 32'h00000000; 
       register_bank[`RELU] <= 32'h00000000; 
       register_bank[`USE_BIAS] <= 32'h00000000; 
       register_bank[`MAX_POOL] <= 32'h00000000; 
       register_bank[`IMAGE_OFFSET] <= 32'h00000000; 
       register_bank[`WEIGHT_OFFSET] <= 32'h00000000; 
       register_bank[`BIAS_OFFSET] <= 32'h00000000; 
       register_bank[`OUTPUT_OFFSET] <= 32'h00000000; 
       register_bank[`NUM_INPUT_IMAGES] <= 32'h00000000; 
       register_bank[`NUM_OUTPUT_IMAGES] <= 32'h00000000; 
       register_bank[`IMAGE_HEIGHT] <= 32'h00000000; 
       register_bank[`IMAGE_WIDTH] <= 32'h00000000; 
       register_bank[`FILTER_HEIGHT] <= 32'h00000000; 
       register_bank[`FILTER_WIDTH] <= 32'h00000000; 
                                                                       
       register_bank[`ADDR_OFFSET_LOW] <= 32'h61000000;                
       register_bank[`ADDR_OFFSET_HIGH] <= 32'h00000000;               
       register_bank[`BURST_SIZE] <= 32'h0000000F;                     
                                                                       
   end else begin                                                      
     if (write_enable) begin                                           
       if (write_address < 20) begin                                   
         register_bank[write_address] <= write_data;                   
       end                                                             
     end                                                               
       register_bank[`GO_READY] <= go_ready; 
       register_bank[`DONE] <= done; 
       register_bank[`DONE_VALID] <= done_valid; 
   end                                                                 
 end                                                                   
                                                                       
 
 always @(posedge clock or negedge resetn) begin 
    if (!resetn) begin 
       go_valid <= 1'b0; 
    end else begin 
       if (write_enable) begin 
          if (write_address == `GO) begin 
             go_valid <= 1'b1; 
          end 
       end 
       if (go_valid && go_ready) begin 
          go_valid <= 1'b0; 
       end 
    end 
 end 
 
 
 always @(posedge clock or negedge resetn) begin 
    if (!resetn) begin 
       done_ready <= 1'b0; 
    end else begin 
       if (read_enable) begin 
          if (read_address == `DONE) begin 
             done_ready <= 1'b1; 
          end 
       end 
       if (done_valid && done_ready) begin 
          done_ready <= 1'b0; 
       end 
    end 
 end 
 
 foo barbar ( 
    .clk (clock), 
    .arst_n (resetn), 
 
    .go_rsc_dat (go), 
    .go_rsc_vld (go_valid), 
    .go_rsc_rdy (go_ready), 
 
    .done_rsc_dat (done), 
    .done_rsc_vld (done_valid), 
    .done_rsc_rdy (done_ready), 
 
    .relu_rsc_dat (relu), 
    .relu_rsc_triosy_lz (relu_tz), 
 
    .use_bias_rsc_dat (use_bias), 
    .use_bias_rsc_triosy_lz (use_bias_tz), 
 
    .max_pool_rsc_dat (max_pool), 
    .max_pool_rsc_triosy_lz (max_pool_tz), 
 
    .image_offset_rsc_dat (image_offset), 
    .image_offset_rsc_triosy_lz (image_offset_tz), 
 
    .weight_offset_rsc_dat (weight_offset), 
    .weight_offset_rsc_triosy_lz (weight_offset_tz), 
 
    .bias_offset_rsc_dat (bias_offset), 
    .bias_offset_rsc_triosy_lz (bias_offset_tz), 
 
    .output_offset_rsc_dat (output_offset), 
    .output_offset_rsc_triosy_lz (output_offset_tz), 
 
    .num_input_images_rsc_dat (num_input_images), 
    .num_input_images_rsc_triosy_lz (num_input_images_tz), 
 
    .num_output_images_rsc_dat (num_output_images), 
    .num_output_images_rsc_triosy_lz (num_output_images_tz), 
 
    .image_height_rsc_dat (image_height), 
    .image_height_rsc_triosy_lz (image_height_tz), 
 
    .image_width_rsc_dat (image_width), 
    .image_width_rsc_triosy_lz (image_width_tz), 
 
    .filter_height_rsc_dat (filter_height), 
    .filter_height_rsc_triosy_lz (filter_height_tz), 
 
    .filter_width_rsc_dat (filter_width), 
    .filter_width_rsc_triosy_lz (filter_width_tz), 
 
    .memory_rsc_m_wstate        (m_wstate),                  
    .memory_rsc_m_wCaughtUp     (caught_up),                 
    .memory_rsc_cfgTimeout      (0),                         
    .memory_rsc_cfgrBurstSize   (burst_size),                
    .memory_rsc_cfgwBurstSize   (burst_size),                
                                                         
    .memory_rsc_cfgrBaseAddress (addr_offset),               
    .memory_rsc_cfgwBaseAddress (addr_offset),               
                                                         
    .memory_rsc_AWREADY  (AWREADY),                          
    .memory_rsc_AWVALID  (AWVALID),                          
    .memory_rsc_AWUSER   (AWUSER),                           
    .memory_rsc_AWREGION (AWREGION),                         
    .memory_rsc_AWQOS    (AWQOS),                            
    .memory_rsc_AWPROT   (AWPROT),                           
    .memory_rsc_AWCACHE  (AWCACHE),                          
    .memory_rsc_AWLOCK   (AWLOCK),                           
    .memory_rsc_AWBURST  (AWBURST),                          
    .memory_rsc_AWSIZE   (AWSIZE),                           
    .memory_rsc_AWLEN    (AWLEN),                            
    .memory_rsc_AWADDR   (AWADDR),                           
    .memory_rsc_AWID     (AWID),                             
                                                         
    .memory_rsc_WREADY   (WREADY),                           
    .memory_rsc_WVALID   (WVALID),                           
    .memory_rsc_WUSER    (WUSER),                            
    .memory_rsc_WLAST    (WLAST),                            
    .memory_rsc_WSTRB    (WSTRB),                            
    .memory_rsc_WDATA    (WDATA),                            
                                                         
    .memory_rsc_BREADY   (BREADY),                           
    .memory_rsc_BVALID   (BVALID),                           
    .memory_rsc_BUSER    (BUSER),                            
    .memory_rsc_BRESP    (BRESP),                            
    .memory_rsc_BID      (BID),                              
                                                         
    .memory_rsc_ARREADY  (ARREADY),                          
    .memory_rsc_ARVALID  (ARVALID),                          
    .memory_rsc_ARUSER   (ARUSER),                           
    .memory_rsc_ARREGION (ARREGION),                         
    .memory_rsc_ARQOS    (ARQOS),                            
    .memory_rsc_ARPROT   (ARPROT),                           
    .memory_rsc_ARCACHE  (ARCACHE),                          
    .memory_rsc_ARLOCK   (ARLOCK),                           
    .memory_rsc_ARBURST  (ARBURST),                          
    .memory_rsc_ARSIZE   (ARSIZE),                           
    .memory_rsc_ARLEN    (ARLEN),                            
    .memory_rsc_ARADDR   (ARADDR),                           
    .memory_rsc_ARID     (ARID),                             
                                                         
    .memory_rsc_RREADY   (RREADY),                           
    .memory_rsc_RVALID   (RVALID),                           
    .memory_rsc_RUSER    (RUSER),                            
    .memory_rsc_RLAST    (RLAST),                            
    .memory_rsc_RRESP    (RRESP),                            
    .memory_rsc_RDATA    (RDATA),                            
    .memory_rsc_RID      (RID),                              
                                                         
    .memory_rsc_triosy_lz (memory_triosy_lz) 
 ); 
 endmodule 
