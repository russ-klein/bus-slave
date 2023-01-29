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
        if (sp->is_master) count+=3;
        sp = sp->next;
    }

    return count;
}


static void print_defines(FILE *txt, signal_struct *signals)
{
    // todo, open a file for this
    //
    fprintf(txt, "`define bw_bits      3                                                 \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, "`define m_bits       4                                                 \n");
    fprintf(txt, "`define id_bits      4                                                 \n");
    fprintf(txt, "`define slave_id_bits 7                                                \n");
    fprintf(txt, "`define addr_bits   32                                                 \n");
    fprintf(txt, "`define len_bits     8                                                 \n");
    fprintf(txt, "`define data_bits    (8 * (1 << `bw_bits))                             \n");
    fprintf(txt, "`define strb_bits    (`data_bits/8)                                    \n");
    fprintf(txt, "`define size_bits    3                                                 \n");
    fprintf(txt, "`define burst_bits   2                                                 \n");
    fprintf(txt, "`define lock_bits    1                                                 \n");
    fprintf(txt, "`define cache_bits   4                                                 \n");
    fprintf(txt, "`define prot_bits    3                                                 \n");
    fprintf(txt, "`define resp_bits    2                                                 \n");
    fprintf(txt, "`define ruser_bits   7                                                 \n");
    fprintf(txt, "`define wuser_bits   9                                                 \n");
    fprintf(txt, "`define snoop_bits   3                                                 \n");
    fprintf(txt, "`define region_bits  4                                                 \n");
    fprintf(txt, "`define qos_bits     4                                                 \n");
    fprintf(txt, "                                                                       \n");

}


static void print_intro(FILE *txt, signal_struct *signals, char *instance_name)
{
    int master = has_master(signals);

    fprintf(txt, "module cat_accel_%s (                                                  \n", instance_name);
    fprintf(txt, "  input          clock,                                                \n");
    fprintf(txt, "  input          resetn,                                               \n");
    fprintf(txt, "  input  [15-`bw_bits:0]  read_addr,                                            \n");
    fprintf(txt, "  output [`data_bits-1:0]  read_data,                                            \n");
    fprintf(txt, "  input          oe,                                                   \n");
    fprintf(txt, "  input  [15-`bw_bits:0]  write_addr,                                           \n");
    fprintf(txt, "  input  [`data_bits-1:0]  write_data,                                           \n");
    fprintf(txt, "  input  [`strb_bits-1:0]   be,                                                   \n");
    fprintf(txt, "  input          we                                                    \n");
    if (master) {
        fprintf(txt, "  ,                                                                    \n");
        fprintf(txt, "  output [`id_bits-1:0]           AWID,                                \n");
        fprintf(txt, "  output [`addr_bits-1:0]         AWADDR,                              \n");
        fprintf(txt, "  output [`len_bits-1:0]          AWLEN,                               \n");
        fprintf(txt, "  output [`size_bits-1:0]         AWSIZE,                              \n");
        fprintf(txt, "  output [`burst_bits-1:0]        AWBURST,                             \n");
        fprintf(txt, "  output [`lock_bits-1:0]         AWLOCK,                              \n");
        fprintf(txt, "  output [`cache_bits-1:0]        AWCACHE,                             \n");
        fprintf(txt, "  output [`prot_bits-1:0]         AWPROT,                              \n");
        fprintf(txt, "  output [`region_bits-1:0]       AWREGION,                            \n");
        fprintf(txt, "  output [`qos_bits-1:0]          AWQOS,                               \n");
        fprintf(txt, "  output                          AWVALID,                             \n");
        fprintf(txt, "  input                           AWREADY,                             \n");
        fprintf(txt, "                                                                       \n");
        fprintf(txt, "  output [`id_bits-1:0]           WID,                                 \n");
        fprintf(txt, "  output [`data_bits-1:0]         WDATA,                               \n");
        fprintf(txt, "  output [`strb_bits-1:0]         WSTRB,                               \n");
        fprintf(txt, "  output                          WLAST,                               \n");
        fprintf(txt, "  output                          WVALID,                              \n");
        fprintf(txt, "  input                           WREADY,                              \n");
        fprintf(txt, "                                                                       \n");
        fprintf(txt, "  input  [`id_bits-1:0]           BID,                                 \n");
        fprintf(txt, "  input  [`resp_bits-1:0]         BRESP,                               \n");
        fprintf(txt, "  input                           BVALID,                              \n");
        fprintf(txt, "  output                          BREADY,                              \n");
        fprintf(txt, "                                                                       \n");
        fprintf(txt, "  output [`id_bits-1:0]           ARID,                                \n");
        fprintf(txt, "  output [`addr_bits-1:0]         ARADDR,                              \n");
        fprintf(txt, "  output [`len_bits-1:0]          ARLEN,                               \n");
        fprintf(txt, "  output [`size_bits-1:0]         ARSIZE,                              \n");
        fprintf(txt, "  output [`burst_bits-1:0]        ARBURST,                             \n");
        fprintf(txt, "  output [`lock_bits-1:0]         ARLOCK,                              \n");
        fprintf(txt, "  output [`cache_bits-1:0]        ARCACHE,                             \n");
        fprintf(txt, "  output [`prot_bits-1:0]         ARPROT,                              \n");
        fprintf(txt, "  output [`region_bits-1:0]       ARREGION,                            \n");
        fprintf(txt, "  output [`qos_bits-1:0]          ARQOS,                               \n");
        fprintf(txt, "  output                          ARVALID,                             \n");
        fprintf(txt, "  input                           ARREADY,                             \n");
        fprintf(txt, "                                                                       \n");
        fprintf(txt, "  input  [`id_bits-1:0]           RID,                                 \n");
        fprintf(txt, "  input  [`data_bits-1:0]         RDATA,                               \n");
        fprintf(txt, "  input  [`resp_bits-1:0]         RRESP,                               \n");
        fprintf(txt, "  input                           RLAST,                               \n");
        fprintf(txt, "  input                           RVALID,                              \n");
        fprintf(txt, "  output                          RREADY                               \n");
        fprintf(txt, "                                                                       \n");
    }
    fprintf(txt, ");                                                                     \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " reg     [63:0]  register_bank[%d:0];                                  \n", register_count(signals)-1);
    fprintf(txt, " reg     [63:0]  rd_reg;                                               \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " reg             ready_out = 1'b1;                                     \n");
    fprintf(txt, " reg             resp_out = 2'b00;                                     \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " wire    [15-`bw_bits:0]  read_address;                                \n");
    fprintf(txt, " wire    [15-`bw_bits:0]  write_address;                               \n");
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
        fprintf(txt, "                                     \n");
        fprintf(txt, " wire [ `addr_bits-1:0] addr_offset; \n");
        fprintf(txt, " wire [ 31:0]           burst_size;  \n");
        fprintf(txt, " wire [  2:0]           m_wstate;    \n");
        fprintf(txt, "                                     \n");
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
       fprintf(txt, " `define ADDR_OFFSET_LOW      %3d \n", offset++);
       fprintf(txt, " `define ADDR_OFFSET_HIGH     %3d \n", offset++);
       fprintf(txt, " `define BURST_SIZE           %3d \n", offset++);
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
        fprintf(txt, "                                                                                              \n");
        fprintf(txt, " assign addr_offset[31:0]               = register_bank[`ADDR_OFFSET_LOW];                    \n");
        //fprintf(txt, " assign addr_offset[`addr_bits-1:32]    = register_bank[`ADDR_OFFSET_HIGH][`addr_bits-33:0];  \n");
        fprintf(txt, " assign burst_size                      = register_bank[`BURST_SIZE];                         \n");
        fprintf(txt, "                                                                                              \n");
    }
    fprintf(txt, " \n");
}


static void print_register_accesses(FILE *txt, signal_struct *signals)
{
    signal_struct *sp;
    char buf[STRLEN];

    fprintf(txt, "                                                                       \n");
    fprintf(txt, " always @(posedge clock or negedge resetn) begin                       \n");
    fprintf(txt, "   if (resetn == 1'b0) begin                                           \n");
    fprintf(txt, "     rd_reg <= 32'h00000000;                                           \n");
    fprintf(txt, "   end else begin                                                      \n");
    fprintf(txt, "     if (read_enable) begin                                            \n");
    fprintf(txt, "       rd_reg <= register_bank[read_address];                          \n");
    fprintf(txt, "     end                                                               \n");
    fprintf(txt, "   end                                                                 \n");
    fprintf(txt, " end                                                                   \n");
    fprintf(txt, "                                                                       \n");
    fprintf(txt, " always @(posedge clock or negedge resetn) begin                       \n");
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
        fprintf(txt, "                                                                       \n");
        fprintf(txt, "       register_bank[`ADDR_OFFSET_LOW] <= 32'h61000000;                \n");
        fprintf(txt, "       register_bank[`ADDR_OFFSET_HIGH] <= 32'h00000000;               \n");
        fprintf(txt, "       register_bank[`BURST_SIZE] <= 32'h0000000F;                     \n");
        fprintf(txt, "                                                                       \n");
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
                fprintf(txt, " always @(posedge clock or negedge resetn) begin \n");
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
                fprintf(txt, " always @(posedge clock or negedge resetn) begin \n");
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
            fprintf(txt, "    .%s_rsc_m_wstate        (m_wstate),                  \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_m_wCaughtUp     (caught_up),                 \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_cfgTimeout      (0),                         \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_cfgrBurstSize   (burst_size),                \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_cfgwBurstSize   (burst_size),                \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_cfgrBaseAddress (addr_offset),               \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_cfgwBaseAddress (addr_offset),               \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_AWREADY  (AWREADY),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWVALID  (AWVALID),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWUSER   (AWUSER),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWREGION (AWREGION),                         \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWQOS    (AWQOS),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWPROT   (AWPROT),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWCACHE  (AWCACHE),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWLOCK   (AWLOCK),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWBURST  (AWBURST),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWSIZE   (AWSIZE),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWLEN    (AWLEN),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWADDR   (AWADDR),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_AWID     (AWID),                             \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_WREADY   (WREADY),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_WVALID   (WVALID),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_WUSER    (WUSER),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_WLAST    (WLAST),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_WSTRB    (WSTRB),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_WDATA    (WDATA),                            \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_BREADY   (BREADY),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_BVALID   (BVALID),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_BUSER    (BUSER),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_BRESP    (BRESP),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_BID      (BID),                              \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_ARREADY  (ARREADY),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARVALID  (ARVALID),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARUSER   (ARUSER),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARREGION (ARREGION),                         \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARQOS    (ARQOS),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARPROT   (ARPROT),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARCACHE  (ARCACHE),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARLOCK   (ARLOCK),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARBURST  (ARBURST),                          \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARSIZE   (ARSIZE),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARLEN    (ARLEN),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARADDR   (ARADDR),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_ARID     (ARID),                             \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_RREADY   (RREADY),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RVALID   (RVALID),                           \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RUSER    (RUSER),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RLAST    (RLAST),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RRESP    (RRESP),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RDATA    (RDATA),                            \n", sp->signal_name);
            fprintf(txt, "    .%s_rsc_RID      (RID),                              \n", sp->signal_name);
            fprintf(txt, "                                                         \n");
            fprintf(txt, "    .%s_rsc_triosy_lz (%s_triosy_lz)", sp->signal_name, sp->signal_name);
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


static void print_header_file(FILE *txt, signal_struct *signals, int accelerator_base_address)
{
    signal_struct *sp;
    int index = 0;
    char buf[STRLEN];
    int i;
    const int offset = 30;

    sp = signals;

    fprintf(txt, "/*************************************************************************** \n");
    fprintf(txt, " *  accelerator interface header                                             \n");
    fprintf(txt, " ***************************************************************************/\n");
    fprintf(txt, "   \n");
    fprintf(txt, "#define ACCEL_ADDR ((volatile unsigned int *) 0x%08x) \n", accelerator_base_address);
    fprintf(txt, "   \n");
    fprintf(txt, "   \n");
    fprintf(txt, "// register map \n");
    fprintf(txt, "   \n");

    while (sp) {
        if (!sp->is_master) {
            fprintf(txt, "#define %s_REG", uppercase(sp->signal_name, buf));
            for (i=strlen(sp->signal_name); i<offset; i++) fprintf(txt, " ");
            fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);
            if (sp->is_channel) {
                if (sp->is_input) {
                    fprintf(txt, "#define %s_READY_REG", uppercase(sp->signal_name, buf));
                    for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                    fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);
                } else {
                    fprintf(txt, "#define %s_VALID_REG", uppercase(sp->signal_name, buf));
                    for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                    fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);
                }
            }
        }
        sp = sp->next;
    }
    if (has_master(signals)) {
        fprintf(txt, "#define ADDR_OFFSET_LOW_REG");
        for (i=strlen("ADDR_OFFSET_LOW_REG"); i<offset+4; i++) fprintf(txt, " ");
        fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);

        fprintf(txt, "#define ADDR_OFFSET_HIGH_REG");
        for (i=strlen("ADDR_OFFSET_HIGH_REG"); i<offset+4; i++) fprintf(txt, " ");
        fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);

        fprintf(txt, "#define BURST_SIZE_REG");
        for (i=strlen("BURST_SIZE_REG"); i<offset+4; i++) fprintf(txt, " ");
        fprintf(txt, " (*(ACCEL_ADDR + %d)) \n", index++);
    }
    fprintf(txt, "\n\n\n");
    fprintf(txt, "// convenience macros \n\n");

    sp = signals;
    while (sp) {
        if (!sp->is_master) {
           if (sp->is_channel && (0 == strcmp(sp->signal_name, "go"))) {
               fprintf(txt, "#define GO                               { while (!GO_READY_REG); GO_REG = 1; } \n");
           }
           else if (sp->is_channel && (0 == strcmp(sp->signal_name, "done"))) {
               fprintf(txt, "#define WAIT_FOR_DONE                    { while (!DONE_VALID_REG); volatile unsigned int x = DONE_REG; } \n");
           }
           else {
                if (sp->is_input) {
                    if (sp->is_channel) {
                        fprintf(txt, "#define SET_%s(X) ", uppercase(sp->signal_name, buf));
                        for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                        fprintf(txt, " { while (!%s_READY_REG); %s_REG = X; } \n", uppercase(sp->signal_name, buf), uppercase(sp->signal_name, buf));
                    } else {
                        fprintf(txt, "#define SET_%s(X) ", uppercase(sp->signal_name, buf));
                        for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                        fprintf(txt, " { %s_REG = X; } \n", uppercase(sp->signal_name, buf));
                    }
                } else {
                    if (sp->is_channel) {
                        fprintf(txt, "#define GET_%s(X) ", uppercase(sp->signal_name, buf));
                        for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                        fprintf(txt, " { while (!%s_READY_REG); X = %s_REG; } \n", uppercase(sp->signal_name, buf), uppercase(sp->signal_name, buf));
                    } else {
                        fprintf(txt, "#define GET_%s(X) ", uppercase(sp->signal_name, buf));
                        for (i=strlen(sp->signal_name); i<offset-6; i++) fprintf(txt, " ");
                        fprintf(txt, " { X = %s_REG; } \n", uppercase(sp->signal_name, buf));
                    }
                }
           }
       }
       sp = sp->next;
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

        header_filename[count+0] = 's';
        header_filename[count+1] = 'w';
        header_filename[count+2] = '.';
        header_filename[count+3] = 'h';
        header_filename[count+4] = 0;
    } else {
        strcpy(header_filename, spec_filename);

        strcat(header_filename, ".sw.h");
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
    int accelerator_base_address;
  
    if ((argc != 4) && (argc != 5)) {
       fprintf(stderr, "Usage: %s: <instance_name> <signal specification file> <base_address> [<module_name>] \n", argv[0]);
       return;
    }

    if (strlen(argv[2]) > STRLEN - 3) {
       fprintf(stderr, "filename %s is too long \n", argv[2]);
       return;
    }

    accelerator_base_address = strtoul(argv[3], 0, 0);

    if (argc == 5) {
       strcpy(module_name, argv[4]);
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
       if (has_master(signals)) print_defines(verilog_file, signals);
       print_intro(verilog_file, signals, argv[1]);
       print_signals(verilog_file, signals);
       print_register_map(verilog_file, signals);
       print_assignments(verilog_file, signals);
       print_register_accesses(verilog_file, signals);
       print_ready_valids(verilog_file, signals);
       print_catapult_instantiation(verilog_file, signals, argv[1], module_name);
       print_epilog(verilog_file);
       print_header_file(header_file, signals, accelerator_base_address);
    }

    while (signals) {
       next_signal = signals->next;
       free(signals);
       signals = next_signal;
    }
}
