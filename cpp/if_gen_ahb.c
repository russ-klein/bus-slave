#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRLEN 1000
#define DEFAULT_MODULE_NAME "cat_accel"

typedef struct sig_str_type {
   char signal_name[STRLEN];
   int  width;
   int  is_signed;
   int  is_input; 
   int  is_wire;
   int  is_channel;
   int  is_master;
   struct sig_str_type *next;
} signal_struct;


static void clean_whitespace(char *s)
{
   // replaces all whitespace characters with a space
   // replaces all strings of whitespace with a single space
   // removes any leading and trailing spaces

   char *p, *start;

   p = start = s;

   // remove leading spaces
   while (isspace(*s)) s++;

   while (*s) {
      if (isspace(*s)) {
         *p++ = ' ';
         while (isspace(*s)) s++;
      } else {
         *p++ = *s++;
      }
   } 
  
   // remove trailing space
   if (p != start) if (*(p-1) == ' ') p--;

   // null terminate result
   *p = (char) 0;
} 
      

static char *lowercase(char *s)
{
   char *p;

   p = s;

   while (*p) {
     *p = tolower(*p);
     p++;
   }
   return s;
}


static char *uppercase(const char *s, char *s_out)
{
   int i = 0;

   while (*s) {
      s_out[i] = toupper(*s);
      s++;
      i++;
   }
   s_out[i] = 0;

   return s_out;
}


static int comment(char *s)
{
   if (s[0] == '#') return 1;
   if ((s[0] == '/') && (s[1] == '/')) return 1;
   return 0;
}


static int valid_signal_name(char *s)
{
   if (strlen(s) == 0) return 0;

   if (!((isalpha(*s) || (*s == '_')))) return 0;

   while (*s) {
      if ((!isalnum(*s)) && (*s != '_') && (*s != '$')) return 0;
      s++;
   }

   return 1;
}

static int valid_width_str(char *s)
{
   char *p;
   int num;

   if (strlen(s) == 0) return 0;

   p = s;
   while (*p) {
      if (!isdigit(*p)) return 0;
      p++;
   }

   num = atoi(s);

   if ((num < 1) || (num > 1024)) return 0;

   return 1;
}

static int valid_signed_str(char *s)
{
   if (0 == strcmp(lowercase(s), "signed")) return 1;
   if (0 == strcmp(lowercase(s), "unsigned")) return 1;
   return 0;
}

static int valid_input_str(char *s)
{
   if (0 == strcmp(lowercase(s), "input")) return 1;
   if (0 == strcmp(lowercase(s), "output")) return 1;

   return 0;
}

static int valid_wire_str(char *s)
{
   if (0 == strcmp(lowercase(s), "wire")) return 1;
   return 0;
}

static int valid_channel_str(char *s)
{
   if (0 == strcmp(lowercase(s), "channel")) return 1;
   return 0;
}

static int valid_master_str(char *s)
{
   if (0 == strcmp(lowercase(s), "master")) return 1;
   return 0;
}


static signal_struct *parse_interface(char *filename)
{
   /* 
    * file should have the following format:
    *
    * <signal_name>, <width>, [signed | unsigned], [input | output], [wires | channel | master]
    *
    */

   FILE *if_spec;
   char *r;
   char line[STRLEN];
   int line_no = 0;
   char signal_name[STRLEN];
   char width_str[STRLEN];
   char signed_str[STRLEN];
   char input_str[STRLEN];
   char wire_str[STRLEN];
   signal_struct *ret_val = NULL;
   signal_struct *signals;
   signal_struct *parent;
   
   if_spec = fopen(filename, "r");
   if (!if_spec) {
      fprintf(stderr, "Unable to open file %s for reading \n", filename);
      perror("if_gen");
      return ret_val;
   }
 
   while (r = fgets(line, sizeof(line), if_spec)) {
      line_no++;

      clean_whitespace(line);
      if (strlen(line) == 0) continue;
      if (comment(line)) continue;

      signal_name[0] = 0;
      r = strtok(line, ",");
      if (r) strcpy(signal_name, r);
      clean_whitespace(signal_name);
 
      width_str[0] = 0;
      r = strtok(NULL, ",");
      if (r) strcpy(width_str, r);
      clean_whitespace(width_str);

      signed_str[0] = 0;
      r = strtok(NULL, ",");
      if (r) strcpy(signed_str, r);
      clean_whitespace(signed_str);

      input_str[0] = 0;
      r = strtok(NULL, ",");
      if (r) strcpy(input_str, r);
      clean_whitespace(input_str);

      wire_str[0] = 0;
      r = strtok(NULL, ",");
      if (r) strcpy(wire_str, r);
      clean_whitespace(wire_str);

      if (!valid_signal_name(signal_name)) {
         fprintf(stderr, "Invalid signal name at line %d: %s \n", line_no, signal_name);
         return ret_val;
      }

      if (!valid_width_str(width_str)) {
         fprintf(stderr, "Invalid signal width at line: %d: %s \n", line_no, width_str);
         return ret_val;
      }

      if (!valid_signed_str(signed_str)) {
         fprintf(stderr, "Invalid signed string at line %d: %s \n", line_no, signed_str);
         return ret_val;
      }

      if (!valid_input_str(input_str)) {
         fprintf(stderr, "Invalid input string at line %d: %s \n", line_no, input_str);
         return ret_val;
      }

      if (!valid_wire_str(wire_str) && !valid_channel_str(wire_str) && !valid_master_str(wire_str)) {
         fprintf(stderr, "Invalid wire string at line %d: %s \n", line_no, wire_str);
         return ret_val;
      }
    
      signals = (signal_struct *) malloc (sizeof(signal_struct));
      if (signals == NULL) {
         fprintf(stderr, "Unable to allocate memory for signal struct \n");
         perror("reg_if");
         return NULL;
      }

      strcpy(signals->signal_name, signal_name);
      signals->width  = atoi(width_str);
      signals->is_signed = (0 == strcmp(lowercase(signed_str), "signed" )) ? 1 : 0;
      signals->is_input  = (0 == strcmp(lowercase(input_str),  "input"  )) ? 1 : 0;
      signals->is_wire   = (0 == strcmp(lowercase(wire_str),   "wire"   )) ? 1 : 0;
      signals->is_channel= (0 == strcmp(lowercase(wire_str),   "channel")) ? 1 : 0;
      signals->is_master = (0 == strcmp(lowercase(wire_str),   "master" )) ? 1 : 0;
      signals->next   = NULL;

      if (ret_val == NULL) {
         ret_val = signals;
      } else {
         parent = ret_val;
         while (parent->next) parent = parent->next;
         parent->next = signals;
      }
   }

   if (!feof(if_spec)) {
      fprintf(stderr, "Error reading file %s \n", filename);
      perror("if_gen");
   }

   fclose(if_spec);

   return ret_val;
}


static int has_master(signal_struct *signals)
{
    signal_struct *sp;
    
    sp = signals;

    while (sp) {
        if (sp->is_master) return 1;
        sp = sp->next;
    }
    return 0;
}


static int master_width(signal_struct *signals)
{
    signal_struct *sp;
    
    sp = signals;

    while (sp) {
        if (sp->is_master) return sp->width;
        sp = sp->next;
    }
    return 0;
}


static int register_count(signal_struct *signals)
{
    signal_struct *sp;
    int count = 0;

    sp = signals;

    while (sp) {
        if (sp->is_wire) count++;
        if (sp->is_channel) count+=2;
        if (sp->is_master) count++;
        sp = sp->next;
    }

    return count;
}

static void print_arbiter(FILE *txt)
{
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " module bus_arbiter                                                              \n");
    fprintf(txt, " (                                                                               \n");
    fprintf(txt, "       clock,                                                                    \n");
    fprintf(txt, "       resetn,                                                                   \n");
    fprintf(txt, "       read_req,                                                                 \n");
    fprintf(txt, "       write_req,                                                                \n");
    fprintf(txt, "       read_done,                                                                \n");
    fprintf(txt, "       write_done,                                                               \n");
    fprintf(txt, "       read_grant,                                                               \n");
    fprintf(txt, "       write_grant                                                               \n");
    fprintf(txt, " );                                                                              \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " input  clock;                                                                   \n");
    fprintf(txt, " input  resetn;                                                                  \n");
    fprintf(txt, " input  read_req;                                                                \n");
    fprintf(txt, " input  write_req;                                                               \n");
    fprintf(txt, " input  read_done;                                                               \n");
    fprintf(txt, " input  write_done;                                                              \n");
    fprintf(txt, " output  read_grant;                                                             \n");
    fprintf(txt, " output  write_grant;                                                            \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " parameter IDLE         = 4'h0;                                                  \n");
    fprintf(txt, " parameter READ         = 4'h1;                                                  \n");
    fprintf(txt, " parameter READ_WAIT    = 4'h2;                                                  \n");
    fprintf(txt, " parameter WRITE        = 4'h3;                                                  \n");
    fprintf(txt, " parameter WRITE_WAIT   = 4'h4;                                                  \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " reg [3:0]  state;                                                               \n");
    fprintf(txt, " reg [3:0]  next_state;                                                          \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " reg read_grant_local;                                                           \n");
    fprintf(txt, " reg write_grant_local;                                                          \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " assign read_grant = read_grant_local;                                           \n");
    fprintf(txt, " assign write_grant = write_grant_local;                                         \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " always @(posedge clock or negedge resetn) begin                                 \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, "    if (resetn == 1'b0) begin                                                    \n");
    fprintf(txt, "      state <= IDLE;                                                             \n");
    fprintf(txt, "    end else begin                                                               \n");
    fprintf(txt, "      state <= next_state;                                                       \n");
    fprintf(txt, "    end                                                                          \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " end                                                                             \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " always @(*) begin                                                               \n");
    fprintf(txt, "   case (state)                                                                  \n");
    fprintf(txt, "      IDLE: begin                                                                \n");
    fprintf(txt, "              if (read_req) begin                                                \n");
    fprintf(txt, "                 next_state <= READ;                                             \n");
    fprintf(txt, "                 read_grant_local = 1'b1;                                        \n");
    fprintf(txt, "                 write_grant_local = 1'b0;                                       \n");
    fprintf(txt, "              end                                                                \n");
    fprintf(txt, "              else if (write_req) begin                                          \n");
    fprintf(txt, "                 next_state <= WRITE;                                            \n");
    fprintf(txt, "                 read_grant_local = 1'b0;                                        \n");
    fprintf(txt, "                 write_grant_local = 1'b1;                                       \n");
    fprintf(txt, "              end                                                                \n");
    fprintf(txt, "              else begin                                                         \n");
    fprintf(txt, "                 next_state <= IDLE;                                             \n");
    fprintf(txt, "                 read_grant_local = 1'b0;                                        \n");
    fprintf(txt, "                 write_grant_local = 1'b0;                                       \n");
    fprintf(txt, "              end                                                                \n");
    fprintf(txt, "            end                                                                  \n");
    fprintf(txt, "      READ: begin                                                                \n");
    fprintf(txt, "              if (read_done) begin                                               \n");
    fprintf(txt, "                 read_grant_local = 1'b0;                                        \n");
    fprintf(txt, "                 if (write_req) begin                                            \n");
    fprintf(txt, "                    next_state = WRITE;                                          \n");
    fprintf(txt, "                    write_grant_local = 1'b1;                                    \n");
    fprintf(txt, "                 end                                                             \n");
    fprintf(txt, "                 //else if (read_req) next_state = READ;                         \n");
    fprintf(txt, "                 else next_state = IDLE;                                         \n");
    fprintf(txt, "              end                                                                \n");
    fprintf(txt, "              else next_state = READ;                                            \n");
    fprintf(txt, "            end                                                                  \n");
    fprintf(txt, "      WRITE: begin                                                               \n");
    fprintf(txt, "              if (write_done) begin                                              \n");
    fprintf(txt, "                 write_grant_local = 1'b0;                                       \n");
    fprintf(txt, "                 if (read_req) begin                                             \n");
    fprintf(txt, "                    next_state = READ;                                           \n");
    fprintf(txt, "                    read_grant_local = 1'b1;                                     \n");
    fprintf(txt, "                 end                                                             \n");
    fprintf(txt, "                 //else if (write_req) next_state = WRITE;                       \n");
    fprintf(txt, "                 else next_state = IDLE;                                         \n");
    fprintf(txt, "              end                                                                \n");
    fprintf(txt, "              else next_state = WRITE;                                           \n");
    fprintf(txt, "            end                                                                  \n");
    fprintf(txt, "      default: next_state = IDLE;                                                \n");
    fprintf(txt, "   endcase                                                                       \n");
    fprintf(txt, " end                                                                             \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, " endmodule                                                                       \n");
    fprintf(txt, "                                                                                 \n");
    fprintf(txt, "                                                                                 \n");
}


static void instantiate_arbiter(FILE *txt)
{
    fprintf(txt, "                                        \n");
    fprintf(txt, " bus_arbiter u_bus_arbiter (            \n");
    fprintf(txt, "    .clock       (clock),               \n");
    fprintf(txt, "    .resetn      (resetn),              \n");
    fprintf(txt, "    .read_req    (input_master_re),     \n");
    fprintf(txt, "    .write_req   (output_master_we),    \n");
    fprintf(txt, "    .read_done   (read_done),           \n");
    fprintf(txt, "    .write_done  (write_done),          \n");
    fprintf(txt, "    .read_grant  (read_addr_active),    \n");
    fprintf(txt, "    .write_grant (write_addr_active)    \n");
    fprintf(txt, " );                                     \n");
    fprintf(txt, "                                        \n");
}

static void print_intro(FILE *txt, signal_struct *signals)
{
    int master = has_master(signals);

    fprintf(txt, "module cat_accel (                                                     \n");
    fprintf(txt, "  input          clock,                                                \n");
    fprintf(txt, "  input          resetn,                                               \n");
    fprintf(txt, "  input  [15:0]  read_addr,                                            \n");
    fprintf(txt, "  output [31:0]  read_data,                                            \n");
    fprintf(txt, "  input          oe,                                                   \n");
    fprintf(txt, "  input  [15:0]  write_addr,                                           \n");
    fprintf(txt, "  input  [31:0]  write_data,                                           \n");
    fprintf(txt, "  input  [3:0]   be,                                                   \n");
    if (master) {
        fprintf(txt, "  input          we,                                                   \n");
        fprintf(txt, "  output         ready,                                                \n");
//        fprintf(txt, "  output         hsel,                                                 \n");
//        fprintf(txt, "  output         hnonsec,                                              \n");
        fprintf(txt, "  output [31:0]  haddr,                                                \n");
        fprintf(txt, "  output [1:0]   htrans,                                               \n");
        fprintf(txt, "  output [2:0]   hsize,                                                \n");
        fprintf(txt, "  output         hwrite,                                               \n");
        fprintf(txt, "  output         hready,                                               \n");
        fprintf(txt, "  output [3:0]   hprot,                                                \n");
        fprintf(txt, "  output [2:0]   hburst,                                               \n");
//        fprintf(txt, "  output         hmastlock,                                            \n");
        fprintf(txt, "  output [31:0]  hwdata,                                               \n");
//        fprintf(txt, "  output         hexcl,                                                \n");
//        fprintf(txt, "  output [3:0]   hmaster,                                              \n");
        fprintf(txt, "  output         hlock,                                               \n");
        fprintf(txt, "  input  [1:0]   hresp,                                                \n");
        fprintf(txt, "  input  [31:0]  hrdata                                               \n");
//        fprintf(txt, "  input          hexokay,                                              \n");
//        fprintf(txt, "  output [1:0]   hauser,                                               \n");
//        fprintf(txt, "  output [1:0]   hwuser,                                               \n");
//        fprintf(txt, "  input  [1:0]   hruser                                                \n");
    } else {
         fprintf(txt, "  input          we                                                   \n");
    }
    fprintf(txt, ");                                                                     \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, "                                                                       \n");
    if (master) {
        //fprintf(txt, " reg             hsel_local      = 1'b1;         \n");
        //fprintf(txt, " reg             hnonsec_local   = 1'b0;         \n");
        fprintf(txt, " reg     [2:0]   hsize_local     = 3'b010; // 000 = char, 001 = short, 010 = long  \n");
        fprintf(txt, " reg     [6:0]   hprot_local     = 7'b0000000;   \n");
        fprintf(txt, " reg     [2:0]   hburst_local    = 3'b000;       \n");
        //fprintf(txt, " reg             hmastlock_local = 1'b0;         \n");
        //fprintf(txt, " reg             hexcl_local     = 1'b0;         \n");
        //fprintf(txt, " reg     [4:0]   hmaster_local   = 4'b0001;      \n");
        //fprintf(txt, " reg     [1:0]   hauser_local    = 2'b00;        \n");
        //fprintf(txt, " reg     [1:0]   hwuser_local    = 2'b00;        \n");
    }
    fprintf(txt, " reg     [31:0]  register_bank[%d:0];                                  \n", register_count(signals)-1);
    fprintf(txt, " reg     [31:0]  rd_reg;                                               \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " reg             ready_out = 1'b1;                                     \n");
    fprintf(txt, " reg             resp_out = 2'b00;                                     \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " wire    [15:0]  read_address;                                         \n");
    fprintf(txt, " wire    [15:0]  write_address;                                        \n");
    fprintf(txt, " wire            read_enable = oe;                                     \n");
    fprintf(txt, " wire            write_enable = we;                                    \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " assign read_data = rd_reg;                                            \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " assign read_address = read_addr[15:0];                                \n");
    fprintf(txt, " assign write_address = write_addr[15:0];                              \n");
    fprintf(txt, "                                                                       \n");
}

static void print_signals(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    int master;

    sp = signals;

    master = has_master(signals);    

    if (sp) {
        fprintf(txt, " \n");
        fprintf(txt, " // interface signals \n");
        fprintf(txt, " \n");
    }
    while (sp) {
        if (!sp->is_master) { 
           if (sp->width > 1) {
              fprintf(txt, " wire [%3d:0] %s; \n", sp->width-1, sp->signal_name);
           } else {
              fprintf(txt, " wire         %s; \n", sp->signal_name);
           }
           if (sp->is_wire) {
              fprintf(txt, " wire         %s_tz; \n", sp->signal_name);
           } 
           if (sp->is_channel) {
              fprintf(txt, " %4s         %s_ready; \n", sp->is_input?"wire":"reg ", sp->signal_name);
              fprintf(txt, " %4s         %s_valid; \n", sp->is_input?"reg ":"wire", sp->signal_name);
           }
        } 
        sp = sp->next;
    }
    if (master) {
        fprintf(txt, " wire [ 31:0] addr_offset; \n");
    }
    if (master) {
        fprintf(txt, "                                                              \n");
        fprintf(txt, " // bus master signals                                        \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " wire [31:0]  input_master_din;                               \n");
        fprintf(txt, " wire [29:0]  input_master_raddr;                             \n");
        fprintf(txt, " reg  [29:0]  input_master_raddr_delayed;                     \n");
        fprintf(txt, " wire         input_master_rrdy;                              \n");
        fprintf(txt, " reg          input_master_rrdy_delayed;                      \n");
        fprintf(txt, " wire         input_master_re;                                \n");
        fprintf(txt, " reg          input_master_re_delayed;                        \n");
        fprintf(txt, " wire         input_master_rstn;                              \n");
        fprintf(txt, " wire         input_master_clk;                               \n");
        fprintf(txt, " wire         input_master_triosy;                            \n");
        fprintf(txt, " wire [31:0]  output_master_dout;                             \n");
        fprintf(txt, " wire [29:0]  output_master_waddr;                            \n");
        fprintf(txt, " wire         output_master_wrdy;                             \n");
        fprintf(txt, " reg          output_master_wrdy_delayed;                     \n");
        fprintf(txt, " wire         output_master_we;                               \n");
        fprintf(txt, " wire         output_master_rstn;                             \n");
        fprintf(txt, " wire         output_master_clk;                              \n");
        fprintf(txt, " wire         output_master_triosy;                           \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " // AHB master signals                                        \n");
        fprintf(txt, "                                                              \n");
        //fprintf(txt, " assign hsel       = hsel_local;                              \n");
        //fprintf(txt, " assign hnonsec    = hnonsec_local;                           \n");
        fprintf(txt, " assign hsize      = hsize_local;                             \n");
        fprintf(txt, " assign hprot      = hprot_local;                             \n");
        fprintf(txt, " assign hburst     = hburst_local;                            \n");
        //fprintf(txt, " assign hmastlock  = hmastlock_local;                         \n");
        //fprintf(txt, " assign hexcl      = hexcl_local;                             \n");
        //fprintf(txt, " assign hmaster    = hmaster_local;                           \n");
        //fprintf(txt, " assign hauser     = hauser_local;                            \n");
        //fprintf(txt, " assign hwuser     = hwuser_local;                            \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " assign input_master_raddr[29:%d] = {%d {1'b0}};              \n", master_width(signals), 30 - master_width(signals));
        fprintf(txt, " assign output_master_waddr[29:%d] = {%d {1'b0}};             \n", master_width(signals), 30 - master_width(signals));
        fprintf(txt, "                                                              \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " wire [31:0]  hraddr = (input_master_raddr<<2) + addr_offset; \n");
        fprintf(txt, " wire [31:0]  hwaddr = (output_master_waddr<<2) + addr_offset;\n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " wire         read_addr_active;                               \n");
        fprintf(txt, " wire         write_addr_active;                              \n");
        fprintf(txt, " reg          read_data_active;                               \n");
        fprintf(txt, " reg          write_data_active;                              \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " wire         read_done = read_data_active & hready;          \n");
        fprintf(txt, " wire         write_done = write_data_active & hready;        \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " assign haddr = (read_addr_active) ? hraddr :                 \n");
        fprintf(txt, "                (write_addr_active) ? hwaddr :                \n");
        fprintf(txt, "                32'h00000000;                                 \n");
        fprintf(txt, " assign hwrite = write_addr_active;                           \n");
        fprintf(txt, " assign htrans = (write_addr_active | read_addr_active) ? 2'b10 : 2'b00;    \n");
        fprintf(txt, " assign hwdata = output_master_dout;                          \n");
        fprintf(txt, "                                                              \n");
        fprintf(txt, " assign input_master_din = hrdata;  // input_master_raddr_delayed[0] ? hrdata[31:16] : hrdata[15:0];   \n");
        fprintf(txt, " assign input_master_rrdy = read_data_active & hready;        \n");
        fprintf(txt, " assign output_master_wrdy = write_data_active & hready;      \n");
        fprintf(txt, " always @(posedge clock) begin                                 \n");
        fprintf(txt, "    if (hready) begin                                         \n");
        fprintf(txt, "      input_master_raddr_delayed <= input_master_raddr;       \n");
        fprintf(txt, "      input_master_rrdy_delayed <= input_master_rrdy;         \n");
        fprintf(txt, "      input_master_re_delayed <= input_master_re;             \n");
        fprintf(txt, "      output_master_wrdy_delayed <= output_master_wrdy;       \n");
        fprintf(txt, "      read_data_active <= read_addr_active;                   \n");
        fprintf(txt, "      write_data_active <= write_addr_active;                 \n");
        fprintf(txt, "    end                                                       \n");
        fprintf(txt, " end                                                          \n");
        fprintf(txt, "                                                              \n");
    }
}


static void print_register_map(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    char buf[STRLEN];
    int offset = 0;

    sp = signals;

    if (sp) {
        fprintf(txt, " \n");
        fprintf(txt, " // register map \n");
        fprintf(txt, " \n");
    }

    while (sp) {
        if (!sp->is_master) {
            fprintf(txt, " `define %-20s %3d \n", uppercase(sp->signal_name, buf), offset++);
            if (sp->is_channel) {
                if (sp->is_input) {
                   fprintf(txt, " `define %-20s %3d \n", strcat(uppercase(sp->signal_name, buf), "_READY"), offset++);
                } else {
                   fprintf(txt, " `define %-20s %3d \n", strcat(uppercase(sp->signal_name, buf), "_VALID"), offset++);
                }
            }
        }
        sp = sp->next;
    }
    if (has_master(signals)) {
       fprintf(txt, " `define ADDR_OFFSET          %3d \n", offset++);
    }
    fprintf(txt, " \n");

}


static void print_assignments(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    char buf[STRLEN];
    int offset = 0;

    sp = signals;

    if (sp) {
        fprintf(txt, " \n");
        fprintf(txt, " // assignments for inputs \n");
        fprintf(txt, " \n");
    }

    while (sp) {
        if (!sp->is_master) {
            if (sp->is_input) {
                if (sp->width == 1) {
                    fprintf(txt, " assign %-20s = register_bank[`%s][0]; \n", 
                                  sp->signal_name, uppercase(sp->signal_name, buf), sp->width-1);
                } 
                else if (sp->width < 32) {
                    fprintf(txt, " assign %-20s = register_bank[`%s][%d:0]; \n", 
                                  sp->signal_name, uppercase(sp->signal_name, buf), sp->width-1);
                }
                else if (sp->width == 32) {
                    fprintf(txt, " assign %-20s = register_bank[`%s]; \n", sp->signal_name, uppercase(sp->signal_name, buf));
                } 
                else { // sp->width > break intpu multiple registers
                    fprintf(stderr, "I'm not done yet, todo: implement wide register assignments \n");
                }
            }
        }
        sp = sp->next;
    }
    if (has_master(signals)) {
        fprintf(txt, " assign addr_offset          = register_bank[`ADDR_OFFSET]; \n");
    }
    fprintf(txt, " \n");
}


static void print_register_accesses(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    char buf[STRLEN];

    fprintf(txt, "                                                                       \n");
    fprintf(txt, " always @(posedge clock or resetn == 1'b0) begin                       \n");
    fprintf(txt, "   if (resetn == 1'b0) begin                                           \n");
    fprintf(txt, "     rd_reg <= 32'h00000000;                                           \n");
    fprintf(txt, "   end else begin                                                      \n");
    fprintf(txt, "     if (read_enable) begin                                            \n");
    fprintf(txt, "       rd_reg <= register_bank[read_address];                          \n");
    fprintf(txt, "     end                                                               \n");
    fprintf(txt, "   end                                                                 \n");
    fprintf(txt, " end                                                                   \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " always @(posedge clock or resetn == 1'b0) begin                       \n");
    fprintf(txt, "   if (resetn == 1'b0) begin                                           \n");

    sp = signals;
    while (sp) {
        if (!sp->is_master) {
            fprintf(txt, "       register_bank[`%s] <= 32'h00000000; \n", uppercase(sp->signal_name, buf));
            if (sp->is_channel) {
                if (sp->is_input) {
                    fprintf(txt, "       register_bank[`%s] <= 32'h00000000; \n", strcat(uppercase(sp->signal_name, buf), "_READY"));
                } else {
                    fprintf(txt, "       register_bank[`%s] <= 32'h00000000; \n", strcat(uppercase(sp->signal_name, buf), "_VALID"));
                }
            }
        }
        sp = sp->next;
    }
   
    if (has_master(signals)) {
        fprintf(txt, "       register_bank[`ADDR_OFFSET] <= 32'h40000000; \n");
    }
    fprintf(txt, "   end else begin                                                      \n");
    fprintf(txt, "     if (write_enable) begin                                           \n");
    fprintf(txt, "       if (write_address < %d) begin                                   \n", register_count(signals));
    fprintf(txt, "         register_bank[write_address] <= write_data;                   \n");
    fprintf(txt, "       end                                                             \n");
    fprintf(txt, "     end                                                               \n");

    sp = signals;
    while (sp) {
        if (!sp->is_master) {
            if (!sp->is_input) {
                fprintf(txt, "       register_bank[`%s] <= %s; \n", uppercase(sp->signal_name, buf), sp->signal_name);
            }
            if (sp->is_channel) {
                if (sp->is_input) {
                    fprintf(txt, "       register_bank[`%s] <= %s_ready; \n", strcat(uppercase(sp->signal_name, buf), "_READY"), sp->signal_name);
                } else {
                    fprintf(txt, "       register_bank[`%s] <= %s_valid; \n", strcat(uppercase(sp->signal_name, buf), "_VALID"), sp->signal_name);
                }
            }
        }
        sp = sp->next;
    }

    fprintf(txt, "   end                                                                 \n");
    fprintf(txt, " end                                                                   \n");
    fprintf(txt, "                                                                       \n");
}

static void print_ready_valids(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    char buf[STRLEN];

    sp = signals;

    while (sp) {
        if (sp->is_channel) {
            if (sp->is_input) {
                fprintf(txt, " \n");
                fprintf(txt, " always @(posedge clock) begin \n");
                fprintf(txt, "    if (!resetn) begin \n");
                fprintf(txt, "       %s_valid <= 1'b0; \n", sp->signal_name);
                fprintf(txt, "    end else begin \n");
                fprintf(txt, "       if (write_enable) begin \n");
                fprintf(txt, "          if (write_address == `%s) begin \n", uppercase(sp->signal_name, buf));
                fprintf(txt, "             %s_valid <= 1'b1; \n", sp->signal_name);
                fprintf(txt, "          end \n");
                fprintf(txt, "       end \n");
                fprintf(txt, "       if (%s_valid && %s_ready) begin \n", sp->signal_name, sp->signal_name);
                fprintf(txt, "          %s_valid <= 1'b0; \n", sp->signal_name);
                fprintf(txt, "       end \n");
                fprintf(txt, "    end \n");
                fprintf(txt, " end \n");
                fprintf(txt, " \n");
            } else {
                fprintf(txt, " \n");
                fprintf(txt, " always @(posedge clock) begin \n");
                fprintf(txt, "    if (!resetn) begin \n");
                fprintf(txt, "       %s_ready <= 1'b0; \n", sp->signal_name);
                fprintf(txt, "    end else begin \n");
                fprintf(txt, "       if (read_enable) begin \n");
                fprintf(txt, "          if (read_address == `%s) begin \n", uppercase(sp->signal_name, buf));
                fprintf(txt, "             %s_ready <= 1'b1; \n", sp->signal_name);
                fprintf(txt, "          end \n");
                fprintf(txt, "       end \n");
                fprintf(txt, "       if (%s_valid && %s_ready) begin \n", sp->signal_name, sp->signal_name);
                fprintf(txt, "          %s_ready <= 1'b0; \n", sp->signal_name);
                fprintf(txt, "       end \n");
                fprintf(txt, "    end \n");
                fprintf(txt, " end \n");
                fprintf(txt, " \n");
            }
        }
        sp = sp->next;
    }
}


static void print_catapult_instantiation(FILE *txt, signal_struct *signals, char *instance_name, char *module_name)
{
    signal_struct *sp;

    fprintf(txt, " %s %s ( \n", instance_name, module_name);
    fprintf(txt, "    .clk (clock), \n");
    fprintf(txt, "    .arst_n (resetn), \n");

    sp = signals;

    while (sp) {
        fprintf(txt, " \n");
        if (sp->is_wire) {
            fprintf(txt, "    .%s_rsc_dat (%s), \n", sp->signal_name, sp->signal_name);
            fprintf(txt, "    .%s_rsc_triosy_lz (%s_tz)", sp->signal_name, sp->signal_name);
        } 
        if (sp->is_channel) {
            fprintf(txt, "    .%s_rsc_dat (%s), \n", sp->signal_name, sp->signal_name);
            fprintf(txt, "    .%s_rsc_vld (%s_valid), \n", sp->signal_name, sp->signal_name);
            fprintf(txt, "    .%s_rsc_rdy (%s_ready)", sp->signal_name, sp->signal_name);
        }
        if (sp->is_master) {
            fprintf(txt, "    .%s_in_rsc_s_din (input_master_din),                 \n", sp->signal_name);
            fprintf(txt, "    .%s_in_rsc_s_raddr (input_master_raddr[%d:0]),       \n", sp->signal_name, sp->width-1);
            fprintf(txt, "    .%s_in_rsc_s_rrdy (input_master_rrdy),               \n", sp->signal_name);
            fprintf(txt, "    .%s_in_rsc_s_re (input_master_re),                   \n", sp->signal_name);
            fprintf(txt, "    .%s_in_rsc_rstn (input_master_rstn),                 \n", sp->signal_name);
            fprintf(txt, "    .%s_in_rsc_clk (input_master_clk),                   \n", sp->signal_name);
            fprintf(txt, "    .%s_in_rsc_triosy_lz (input_master_triosy),          \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_out_rsc_s_dout (output_master_dout),             \n", sp->signal_name);
            fprintf(txt, "    .%s_out_rsc_s_waddr (output_master_waddr[%d:0]),     \n", sp->signal_name, sp->width-1);
            fprintf(txt, "    .%s_out_rsc_s_wrdy (output_master_wrdy),             \n", sp->signal_name);
            fprintf(txt, "    .%s_out_rsc_s_we (output_master_we),                 \n", sp->signal_name);
            fprintf(txt, "    .%s_out_rsc_rstn (output_master_rstn),               \n", sp->signal_name);
            fprintf(txt, "    .%s_out_rsc_clk (output_master_clk),                 \n", sp->signal_name);
            fprintf(txt, "    .%s_out_rsc_triosy_lz (output_master_triosy)\n",          sp->signal_name);
        }
        if (sp->next) fprintf(txt, ", \n");
        else fprintf(txt, " \n");
        
        sp = sp->next;
    }
    fprintf(txt, " ); \n");
}


static void print_epilog(FILE *txt)
{
    fprintf(txt, " endmodule \n");
}


static void print_header_file(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    int index = 0;
    char buf[STRLEN];

    sp = signals;

    fprintf(txt, "/*************************************************************************** \n");
    fprintf(txt, " *  accelerator interface header                                             \n");
    fprintf(txt, " ***************************************************************************/\n");
    fprintf(txt, "   \n");
    fprintf(txt, "#define ACCEL_ADDR ((volatile unsigned int *) 0xA0000000) \n");
    fprintf(txt, "   \n");
    fprintf(txt, "   \n");
    fprintf(txt, "// register map \n");
    fprintf(txt, "   \n");

    while (sp) {
        if (!sp->is_master) {
            fprintf(txt, "#define %s (*(ACCEL_ADDR + %d)) \n", uppercase(sp->signal_name, buf), index++);
            if (sp->is_channel) {
                if (sp->is_input) {
                    fprintf(txt, "#define %s_READY (*(ACCEL_ADDR + %d)) \n", uppercase(sp->signal_name, buf), index++);
                } else {
                    fprintf(txt, "#define %s_VALID (*(ACCEL_ADDR + %d)) \n", uppercase(sp->signal_name, buf), index++);
                }
            }
        }
        sp = sp->next;
    }
    if (has_master(signals)) {
        fprintf(txt, "#define ADDR_OFFSET (*(ACCEL_ADDR + %d)) \n", index++);
    }
}
 
               
void make_filenames(char *spec_filename, char *header_filename)
{
    char *p;
    unsigned int count;
    unsigned int i;

    p = spec_filename + strlen(spec_filename);

    while ((*p != '.') && (*p != '/') && (p > spec_filename)) p--;

    if (*p == '.') {
        count = p - spec_filename + 1;
        for (i=0; i<count; i++) {
           header_filename[i] = spec_filename[i];
        }

        header_filename[count] = 'h';
        header_filename[count+1] = 0;
    } else {
        strcpy(header_filename, spec_filename);

        strcat(header_filename, ".h");
    }
}


main(int argc, char **argv)
{
    int i;
    signal_struct *signals;
    signal_struct *next_signal;
    FILE *verilog_file;
    FILE *header_file;
    char filename[STRLEN];
    char verilog_filename[STRLEN];
    char header_filename[STRLEN];
    char module_name[STRLEN] = DEFAULT_MODULE_NAME;
  
    if ((argc != 3) && (argc != 4)) {
       fprintf(stderr, "Usage: %s: <instance_name> <signal specification file> [<module_name>] \n", argv[0]);
       return;
    }

    if (strlen(argv[2]) > STRLEN - 3) {
       fprintf(stderr, "filename %s is too long \n", argv[2]);
       return;
    }

    if (argc == 4) {
       strcpy(module_name, argv[3]);
    }

    // modulename and filename must match for VCS

    strcpy(verilog_filename, module_name);
    strcat(verilog_filename, ".v");

    make_filenames(argv[2], header_filename);

    verilog_file = fopen(verilog_filename, "w");
    if (!verilog_filename) {
        fprintf(stderr, "Unable to open file %d for writing. \n", verilog_filename);
        perror("if_gen");
        return;
    }
       
    header_file = fopen(header_filename, "w");
    if (!header_file) {
        fprintf(stderr, "Unable to open file %d for writing. \n", header_filename);
        perror("if_gen");
        return;
    }
       

    signals = parse_interface(argv[2]);

    if (signals) {
       if (has_master(signals)) print_arbiter(verilog_file);
       print_intro(verilog_file, signals);
       print_signals(verilog_file, signals);
       print_register_map(verilog_file, signals);
       print_assignments(verilog_file, signals);
       print_register_accesses(verilog_file, signals);
       print_ready_valids(verilog_file, signals);
       print_catapult_instantiation(verilog_file, signals, argv[1], module_name);
       if (has_master(signals)) instantiate_arbiter(verilog_file);
       print_epilog(verilog_file);
       print_header_file(header_file, signals);
    }

    while (signals) {
       next_signal = signals->next;
       free(signals);
       signals = next_signal;
    }
}
