/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

import std.stdio;
import std.string;
import std.array;
import std.conv;
import std.file;
import std.path;
import std.algorithm : sort;

struct file_input_struct {
    bool ismacro;
    string name, fullpath;
    int linenum;
    File fd;
    string[] params;
}

struct input_line_struct {
    string orig, word, params, param1;
}

struct macro_struct {
    string name;
    string[] params;
    int[] index;
    string[] lines;
}

enum ParameterType { Integer, Pointer, Cpu }

struct func_parameter_struct {
    string name;
    ParameterType type;
}

struct func_struct {
    string name;
    bool has_return_value;
    func_parameter_struct[] params;
}

struct proc_struct {
    string name, alias_name;
    bool isexternal, ispublic, isglobal;
    string global_name;
    input_line_struct[] lines;
}

struct instruction_struct {
    string name;
    string[] params;
    bool[string][] params2;
    string variable_params;
    bool[string] variable_params2;
}

struct param_struct {
    string type, value;
    int ivalue;
}

struct proc_instr_struct {
    bool is_valid, is_inside_if, func_has_return_value, if_contains_call;
    string[2] write_reg;
    bool[2] last_reg_write;
    param_struct[] params;
}

struct dlabel_struct {
    string name, dataseg_name;
    int offset;
    bool isglobal;
}

struct ddata_struct {
    bool isaddr;
    ubyte[] data;
    int datalen;
    string addr;
}

struct dataseg_struct {
    string name;
    bool isconstant, isuninitialized;
    dlabel_struct[] labels;
    ddata_struct[] data;
}


enum RegisterState { Empty, Read, Write }

string input_filename, output_filename, input_directory, return_procedure, dispatcher_procedure;
string[] include_directories;
bool output_preprocessed_file, input_reading_proc, input_reading_dataseg, position_independent_code, old_bitcode, no_tail_calls;
uint global_optimization_level, procedure_optimization_level;

int file_input_level;
file_input_struct[] file_input_stack;
bool end_of_input;
input_line_struct current_line;

string current_proc, current_dataseg;
int current_proc_lines, current_dataseg_offset, current_dataseg_numlabels, current_dataseg_numdata;
bool current_dataseg_isuninitialized;
input_line_struct[] proc_instructions;
dlabel_struct[] dataseg_labels;
ddata_struct[] dataseg_data;
int temporary_register_index, label_index;

macro_struct[string] macro_list;
string[string] define_list;
bool[string] extern_list;
dataseg_struct[string] dataseg_list;
dlabel_struct[string] dlabel_list;
func_struct[string] func_list;
proc_struct[string] proc_list;
string[] local_proc_names;

bool used_ctlz_intrinsics, used_bswap_intrinsics;

int num_output_lines;
string[] output_lines;

immutable int num_regs = 9;
string[] registers_base_list = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "eflags", "tmpadr", "tmpcnd", "tmp0", "tmp1", "tmp2", "tmp3", "tmp4", "tmp5", "tmp6", "tmp7", "tmp8", "tmp9", "tmp10", "tmp11", "tmp12", "tmp13", "tmp14", "tmp15", "tmp16", "tmp17", "tmp18", "tmp19"];
string[] keywords_base_list = ["proc", "extern", "define", "macro", "func", "funcv", "include", "endp", "endm", "datasegment", "dlabel", "dalign", "db", "dinclude", "daddr", "dskip", "endd"];
string[] instructions_base_list = [
    "mov reg, reg/const/procaddr/externaddr",
    "add reg, reg, reg/const",
    "sub reg, reg/const, reg/const",
    "shl reg, reg, reg/const",
    "lshr reg, reg, reg/const",
    "ashr reg, reg, reg/const",
    "and reg, reg, reg/const",
    "or reg, reg, reg/const",
    "xor reg, reg, reg/const",
    "load reg, reg/externaddr, const",
    "load8s reg, reg/externaddr, const",
    "load8z reg, reg/externaddr, const",
    "load16s reg, reg/externaddr, const",
    "load16z reg, reg/externaddr, const",
    "store reg/const/procaddr/externaddr, reg/externaddr, const",
    "store8 reg/const, reg/externaddr, const",
    "store16 reg/const, reg/externaddr, const",
    "ext8s reg, reg",
    "ext16s reg, reg",
    "ins16 reg, reg, reg",
    "ins8ll reg, reg, reg",
    "ins8lh reg, reg, reg",
    "ins8hl reg, reg, reg",
    "ins8hh reg, reg, reg",
    "tcall reg/procaddr",
    "ctcallz reg, procaddr",
    "ctcallnz reg, procaddr",
    "cmovz reg, reg, reg/const, reg/const",
    "cmoveq reg, reg/const, reg, reg/const, reg/const",
    "cmovult reg, reg/const, reg, reg/const, reg/const",
    "cmovugt reg, reg/const, reg, reg/const, reg/const",
    "cmovslt reg, reg/const, reg, reg/const, reg/const",
    "cmovsgt reg, reg/const, reg, reg/const, reg/const",
    "call funcaddr [$/reg/const]",
    "mul reg, reg, reg/const",
    "imul reg, reg, reg, reg/const",
    "umul reg, reg, reg, reg/const",
    "ifz reg",
    "ifnz reg",
    "endif",
    "ctlz reg, reg",
    "bswap reg, reg"
];

bool[string] keywords_list, registers_list, temp_regs_list;
string[string] register_numbers_str;
instruction_struct[string] instructions_list;
RegisterState[num_regs] register_state;
string[string] current_temporary_register;

string[] str_split_strip(string str, dchar delim)
{
    string[] res;

    res.length = 0;

    while (str != "")
    {
        int nextpos = cast(int)str.indexOf(delim);

        int cur = cast(int)res.length;
        res.length++;

        if (nextpos == -1)
        {
            res[cur] = str.idup;
            str = "";
        }
        else
        {
            res[cur] = str[0..nextpos].stripRight().idup;
            str = str[nextpos + 1..$].stripLeft();
        }
    }

    return res;
}

void initialize()
{
    foreach(register; registers_base_list)
    {
        registers_list[register] = true;
    }
    registers_list = registers_list.rehash;

    for (int i = 0; i < num_regs; i++)
    {
        register_numbers_str[registers_base_list[i]] = to!string(i);
    }
    register_numbers_str = register_numbers_str.rehash;

    for (int i = num_regs; i < registers_base_list.length; i++)
    {
        temp_regs_list[registers_base_list[i]] = true;
    }
    temp_regs_list = temp_regs_list.rehash;


    foreach(keyword; keywords_base_list)
    {
        keywords_list[keyword] = true;
    }
    keywords_list = keywords_list.rehash;


    foreach(instruction; instructions_base_list)
    {
        long position = instruction.indexOf(' ');
        string instruction_name, variable_param;
        string[] params;

        if (position >= 0)
        {
            uint position2 = cast(uint)position;
            instruction_name = instruction[0..position2];
            instruction = instruction[position2+1..$].strip();

            position = instruction.indexOf('[');
            if (position >= 0)
            {
                position2 = cast(uint)position;
                variable_param = instruction[position2+1..$].strip();
                instruction = instruction[0..position2].strip();
            }
            else
            {
                variable_param = "";
            }

            params = str_split_strip(instruction, ',');

            if (variable_param != "")
            {
                variable_param = variable_param[0..variable_param.length-1].strip();
            }
        }
        else
        {
            instruction_name = instruction;
            params.length = 0;
            variable_param = "";
        }

        instruction_struct newinstruction;
        newinstruction.name = instruction_name.idup;
        newinstruction.params = params;
        newinstruction.variable_params = variable_param.idup;

        newinstruction.params2.length = newinstruction.params.length;
        for (int i = 0; i < newinstruction.params.length; i++)
        {
            auto params2 = str_split_strip(newinstruction.params[i], '/');
            foreach (param; params2)
            {
                newinstruction.params2[i][param] = true;
            }
            newinstruction.params2[i] = newinstruction.params2[i].rehash;
        }

        if (variable_param != "")
        {
            auto variable_params2 = str_split_strip(variable_param, '/');
            foreach (param; variable_params2)
            {
                newinstruction.variable_params2[param] = true;
            }
            newinstruction.variable_params2 = newinstruction.variable_params2.rehash;
        }

        instructions_list[instruction_name] = newinstruction;
    }
    instructions_list = instructions_list.rehash;

    used_ctlz_intrinsics = false;
    used_bswap_intrinsics = false;
}

void read_next_line()
{
    while (file_input_level >= 0)
    {
        char[] buf;

        if (file_input_stack[file_input_level].ismacro)
        {
            macro_struct curmacro = macro_list[file_input_stack[file_input_level].name];

            if (file_input_stack[file_input_level].linenum >= curmacro.lines.length)
            {
                file_input_stack[file_input_level].params.length = 0;
                file_input_level--;
                continue;
            }

            buf = curmacro.lines[file_input_stack[file_input_level].linenum].dup;

            for (int i = 0; i < curmacro.index.length; i++)
            {
                int index = curmacro.index[i];

                buf = buf.replace("\\" ~ curmacro.params[index], file_input_stack[file_input_level].params[index]);
            }
        }
        else
        {
            if (!file_input_stack[file_input_level].fd.readln(buf))
            {
                file_input_stack[file_input_level].fd.close();
                file_input_level--;
                continue;
            }

            // remove return from end of the line
            while (buf.length > 0 && (buf[buf.length - 1] == '\r' || buf[buf.length - 1] == '\n'))
            {
                buf.length--;
            }

        }

        file_input_stack[file_input_level].linenum++;
        current_line.orig = buf.idup;

        long position = buf.indexOf(';');
        if (position >= 0)
        {
            uint position2 = cast(uint)position;
            buf = buf[0..position2];
        }

        buf = buf.strip().detab(1);

        position = buf.indexOf(' ');
        if (position >= 0)
        {
            uint position2 = cast(uint)position;
            current_line.word = buf[0..position2].idup;
            buf = buf[position2+1..$].stripLeft();

            current_line.params = buf.idup;

            position = buf.indexOf(' ');
            if (position >= 0)
            {
                position2 = cast(uint)position;
                current_line.param1 = buf[0..position2].idup;
            }
            else
            {
                current_line.param1 = buf.idup;
            }
        }
        else
        {
            current_line.word = buf.idup;
            current_line.params = "".idup;
            current_line.param1 = "".idup;
        }

        return;
    }

    end_of_input = true;
}

void write_error(string error_string)
{
    stderr.writeln(error_string);
    for (int i = file_input_level; i >= 0; i--)
    {
        stderr.writeln((file_input_stack[i].ismacro ? "Macro" : "File") ~ ": " ~ file_input_stack[i].name ~ " - Line: " ~ to!string(file_input_stack[i].linenum));
    }
}

void write_error2(string error_string)
{
    stderr.writeln(error_string);
    stderr.writeln("Procedure: " ~ current_proc);
}

void add_output_line(string line)
{
    if (num_output_lines >= output_lines.length)
    {
        output_lines.length = 1 + 2 * output_lines.length;
    }

    output_lines[num_output_lines] = line.idup;
    num_output_lines++;
}

bool add_include_to_stack(string include_name)
{
    string fullpath = null;

    if (!isAbsolute(include_name))
    {
        string path = buildPath(input_directory, include_name);
        if (exists(path))
        {
            fullpath = absolutePath(path);
        }
        else
        {
            for (int i = 0; i < include_directories.length; i++)
            {
                if (isAbsolute(include_directories[i]))
                {
                    path = buildPath(include_directories[i], include_name);
                }
                else
                {
                    path = buildPath(input_directory, include_directories[i], include_name);
                }

                if (exists(path))
                {
                    fullpath = absolutePath(path);
                    break;
                }
            }
        }
    }

    if (fullpath == null)
    {
        if (exists(include_name))
        {
            fullpath = absolutePath(include_name);
        }
        else
        {
            write_error("Include not found: " ~ include_name);
            return false;
        }
    }

    for (int i = file_input_level; i >= 0; i--)
    {
        if (!file_input_stack[i].ismacro && file_input_stack[i].fullpath == fullpath)
        {
            write_error("Recursive include: " ~ include_name);
            return false;
        }
    }

    file_input_level++;
    if (file_input_level >= file_input_stack.length)
    {
        file_input_stack.length = file_input_level + 1;
    }
    file_input_stack[file_input_level].ismacro = false;
    file_input_stack[file_input_level].name = include_name.idup;
    file_input_stack[file_input_level].fullpath = fullpath.idup;
    file_input_stack[file_input_level].linenum = 0;
    file_input_stack[file_input_level].fd.open(fullpath);

    return true;
}

bool paramsComp(int[2] x, int[2] y) @safe pure nothrow
{
    if (x[1] > y[1]) return true;

    return false;
}

bool add_macro_to_stack(string macro_name, string[] params)
{
    for (int i = file_input_level; i >= 0; i--)
    {
        if (file_input_stack[i].ismacro && file_input_stack[i].name == macro_name)
        {
            write_error("Recursive macro call: " ~ macro_name);
            return false;
        }
    }

    if (params.length != macro_list[macro_name].params.length)
    {
        write_error("Wrong number of macro parameters: " ~ macro_name);
        return false;
    }

    file_input_level++;
    if (file_input_level >= file_input_stack.length)
    {
        file_input_stack.length = file_input_level + 1;
    }
    file_input_stack[file_input_level].ismacro = true;
    file_input_stack[file_input_level].name = macro_name.idup;
    file_input_stack[file_input_level].linenum = 0;
    file_input_stack[file_input_level].params = params;

    return true;
}

string[] get_current_params()
{
    return str_split_strip(current_line.params, ',');
}

string[] get_current_params2()
{
    long position = current_line.params.indexOf(' ');

    if (position >= 0)
    {
        uint position2 = cast(uint)position;
        return str_split_strip(current_line.params[position2+1..$].stripLeft(), ',');
    }
    else
    {
        return str_split_strip("", ',');
    }
}

func_parameter_struct[] get_functions_params(string[] params)
{
    func_parameter_struct[] func_params;

    func_params.length = params.length;

    for (int i = 0; i < params.length; i++)
    {
        if (params[i] == "$")
        {
            func_params[i].type = ParameterType.Cpu;
            func_params[i].name = "cpu";
        }
        else if (params[i][0] == '*')
        {
            func_params[i].type = ParameterType.Pointer;
            func_params[i].name = params[i][1..$].stripLeft();
        }
        else
        {
            func_params[i].type = ParameterType.Integer;
            func_params[i].name = params[i];
        }
    }

    return func_params;
}

bool check_instruction_pass1(ref input_line_struct instr_line)
{
    auto instruction = instructions_list[instr_line.word];

    if (instruction.variable_params2.length == 0)
    {
        string[] params = str_split_strip(instr_line.params, ',');

        if (params.length != instruction.params.length)
        {
            write_error("Wrong number of instruction parameters: " ~ instr_line.word);
            return false;
        }

        for (int i = 0; i < params.length; i++)
        {
            if (instruction.params[i] == "reg")
            {
                if (params[i] !in registers_list)
                {
                    write_error("Wrong instruction parameter: " ~ instr_line.word ~ ": " ~ to!string(i));
                    return false;
                }
            }
        }

        if (instr_line.word == "ctlz")
        {
            used_ctlz_intrinsics = true;
        }
        if (instr_line.word == "bswap")
        {
            used_bswap_intrinsics = true;
        }
    }
    else
    {
        if (instr_line.word == "call" && instruction.params.length == 1)
        {
            if (instr_line.params == "")
            {
                write_error("Missing call parameters");
                return false;
            }
        }
        else
        {
            write_error("Instruction error");
            return false;
        }
    }

    return true;
}

bool is_number(string str_num)
{
    if (str_num.length > 2 && str_num[0] == '0' && str_num[1] == 'x')
    {
        foreach(c; str_num[2..$].toLower())
        {
            if ("0123456789abcdef".indexOf(c) == -1)
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        try
        {
            long num = to!long(str_num);
            return true;
        }
        catch (Exception e)
        {
            return false;
        }
    }
}

long get_number_value(string str_num)
{
    if (str_num.length > 2 && str_num[0] == '0' && str_num[1] == 'x')
    {
        long result = 0;

        foreach(c; str_num[2..$].toLower())
        {
            result = result * 16 + cast(long)("0123456789abcdef".indexOf(c));
        }

        return result;
    }
    else
    {
        return to!long(str_num);
    }
}

bool calculate_expr_value2(ref string expr)
{
    expr = expr.strip();

    if (expr == "") return false;

    string[2][] tokens;

    tokens.length = 0;

    string last_delim = "";

    while (expr != "")
    {
        int nextpos = -1;
        string next_delim = "";
        foreach (c; "+-*&|^~")
        {
            int pos = cast(int)expr.indexOf(c);
            if (pos != -1)
            {
                if (nextpos == -1 || nextpos > pos)
                {
                    nextpos = pos;

                    char[1] delim_char = [c];
                    next_delim = delim_char.idup;
                }
            }
        }

        int shlpos = cast(int)expr.indexOf("<<");
        int shrpos = cast(int)expr.indexOf(">>");
        int shiftpos;
        string shift_delim = "";
        if (shlpos != -1)
        {
            if (shrpos != -1)
            {
                if (shrpos < shlpos)
                {
                    shiftpos = shrpos;
                    shift_delim = ">>";
                }
                else
                {
                    shiftpos = shlpos;
                    shift_delim = "<<";
                }
            }
            else
            {
                shiftpos = shlpos;
                shift_delim = "<<";
            }
        }
        else
        {
            shiftpos = shrpos;
            if (shrpos != -1)
            {
                shift_delim = ">>";
            }
        }

        if (last_delim != "")
        {
            tokens.length++;
            tokens[tokens.length-1] = [last_delim, ""];
        }

        string expr_part;

        if (nextpos == -1 && shiftpos == -1)
        {
            expr_part = expr;
            expr = "";
            last_delim = "";
        }
        else if (shiftpos == -1 || (nextpos != -1 && nextpos < shiftpos))
        {
            expr_part = expr[0..nextpos].stripRight();
            expr = expr[nextpos + 1..$].stripLeft();
            last_delim = next_delim;
        }
        else
        {
            expr_part = expr[0..shiftpos].stripRight();
            expr = expr[shiftpos + 2..$].stripLeft();
            last_delim = shift_delim;
        }


        if (expr_part == "")
        {
            // ok
        }
        else if (is_number(expr_part))
        {
            // ok
            if (expr_part != "")
            {
                tokens.length++;
                tokens[tokens.length-1] = ["", expr_part];
            }
        }
        else
        {
            return false;
        }
    }

    if (last_delim != "")
    {
        tokens.length++;
        tokens[tokens.length-1] = [last_delim, ""];
    }

    // unary ops
    for (int i = cast(int)(tokens.length) - 1; i >= 0; i--)
    {
        string op = tokens[i][0];

        if (op == "") continue; // token is number
        if (op != "~" && op != "-") continue; // token is binary op

        if (i == tokens.length - 1) return false; // token is last token
        if (tokens[i + 1][0] != "") return false; // next token is not a number

        if (op == "-" && i > 0 && tokens[i + 1][0] == "") continue; // token is binary op

        long value = get_number_value(tokens[i + 1][1]);

        if (op == "-")
        {
            value = -value;
        }
        else
        {
            value = ~value;
        }

        uint value2 = cast(uint)value;
        string[2][] newtoken = [["", to!string(value2)]];

        tokens = tokens[0..i] ~ newtoken ~ tokens[i+2..$];
    }

    // binary ops
    static immutable string[][] precedence = [
        ["*"],
        ["&", "|", "^"],
        ["<<", ">>"],
        ["+", "-"],
    ];
    foreach (op_prec; precedence)
    {
        for (int i = cast(int)(tokens.length) - 1; i >= 0; i--)
        {
            string op = tokens[i][0];

            if (op == "") continue; // token is number

            bool op_found = false;
            foreach (prec; op_prec)
            {
                if (op == prec)
                {
                    op_found = true;
                    break;
                }
            }
            if (!op_found) continue; // wrong op precedence

            if (i == tokens.length - 1) return false; // token is last token
            if (tokens[i + 1][0] != "") return false; // next token is not a number

            if (i == 0) return false; // token is first token
            if (tokens[i - 1][0] != "") return false; // previous token is not a number

            long value0 = get_number_value(tokens[i - 1][1]);
            long value1 = get_number_value(tokens[i + 1][1]);

            switch (op)
            {
                case "+":
                    value0 += value1;
                    break;
                case "-":
                    value0 -= value1;
                    break;
                case "*":
                    value0 *= value1;
                    break;
                case "&":
                    value0 &= value1;
                    break;
                case "|":
                    value0 |= value1;
                    break;
                case "^":
                    value0 ^= value1;
                    break;
                case "<<":
                    value0 <<= value1;
                    break;
                case ">>":
                    value0 >>= value1;
                    break;
                default:
                    return false;
            }

            uint value2 = cast(uint)value0;
            string[2][] newtoken = [["", to!string(value2)]];

            tokens = tokens[0..i-1] ~ newtoken ~ tokens[i+2..$];
            i--;
        }
    }

    if (tokens.length == 1 && tokens[0][0] == "")
    {
        long value = get_number_value(tokens[0][1]);
        uint value2 = cast(uint)value;
        expr = to!string(value2);
        return true;
    }

    return false;
}

bool calculate_expr_value(ref string param)
{
    while (true)
    {
        long position = param.indexOf(')');
        if (position >= 0)
        {
            uint position2 = cast(uint)position;
            string part3 = param[position2+1..$];
            string part1 = param[0..position2];

            position = part1.lastIndexOf('(');
            if (position >= 0)
            {
                position2 = cast(uint)position;
                string part2 = part1[position2+1..$];
                part1 = part1[0..position2];

                if (!calculate_expr_value2(part2))
                {
                    return false;
                }

                param = part1 ~ " " ~ part2 ~ " " ~ part3;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (param.indexOf('(') >= 0)
            {
                return false;
            }

            break;
        }
    }

    return calculate_expr_value2(param);
}

bool replace_defines_in_expr2(string expr, out string res, string[] used_defines)
{
    string last_delim = "";
    res = "".idup;

    while (expr != "")
    {
        int nextpos = -1;
        string next_delim = "";
        foreach (c; "+-*&|^~()")
        {
            int pos = cast(int)expr.indexOf(c);
            if (pos != -1)
            {
                if (nextpos == -1 || nextpos > pos)
                {
                    nextpos = pos;

                    char[1] delim_char = [c];
                    next_delim = delim_char.idup;
                }
            }
        }

        int shlpos = cast(int)expr.indexOf("<<");
        int shrpos = cast(int)expr.indexOf(">>");
        int shiftpos;
        string shift_delim = "";
        if (shlpos != -1)
        {
            if (shrpos != -1)
            {
                if (shrpos < shlpos)
                {
                    shiftpos = shrpos;
                    shift_delim = ">>";
                }
                else
                {
                    shiftpos = shlpos;
                    shift_delim = "<<";
                }
            }
            else
            {
                shiftpos = shlpos;
                shift_delim = "<<";
            }
        }
        else
        {
            shiftpos = shrpos;
            if (shrpos != -1)
            {
                shift_delim = ">>";
            }
        }


        res = res ~ /*((last_delim == "(")?"":" ") ~*/ last_delim /*~ ((last_delim == ")")?"":" ")*/;

        string expr_part;

        if (nextpos == -1 && shiftpos == -1)
        {
            expr_part = expr;
            expr = "";
            last_delim = "";
        }
        else if (shiftpos == -1 || (nextpos != -1 && nextpos < shiftpos))
        {
            expr_part = expr[0..nextpos].stripRight();
            expr = expr[nextpos + 1..$].stripLeft();
            last_delim = next_delim;
        }
        else
        {
            expr_part = expr[0..shiftpos].stripRight();
            expr = expr[shiftpos + 2..$].stripLeft();
            last_delim = shift_delim;
        }

        if (expr_part == "")
        {
            // ok
        }
        else if (is_number(expr_part))
        {
            // ok
            res = res ~ expr_part;
        }
        else if (expr_part in define_list)
        {
            for (int i = 0; i < used_defines.length; i++)
            {
                if (expr_part == used_defines[i])
                {
                    stderr.writeln("Recursive define: " ~ expr_part);
                    return false;
                }
            }

            string[] used_defines2 = used_defines.dup;
            used_defines2.length++;
            used_defines2[used_defines2.length - 1] = expr_part.idup;

            string expr_part2;

            if (!replace_defines_in_expr2(define_list[expr_part], expr_part2, used_defines2))
            {
                return false;
            }

            // ok
            res = res ~ expr_part2;
        }
        else
        {
            return false;
        }
    }

    res = res ~ /*" " ~*/ last_delim;

    //res = res.strip();

    return true;
}

bool replace_defines_in_expr(ref string param)
{
    string[] used_defines;
    string expr2;
    bool retval = replace_defines_in_expr2(param, expr2, used_defines);

    if (retval)
    {
        param = expr2;
    }

    return retval;
}

bool is_temporary_register(string reg)
{
    if (reg in temp_regs_list)
    {
        return true;
    }
    else
    {
        return false;
    }
}

string get_new_temporary_register()
{
    temporary_register_index++;
    return "%" ~ to!string(temporary_register_index);
}

string get_new_label()
{
    label_index++;
    return "l_" ~ to!string(label_index);
}

string get_load_type(string type)
{
    if (old_bitcode)
    {
        return type ~ "*";
    }
    else
    {
        return type ~ ", " ~ type ~ "*";
    }
}

string get_parameter_read_value(param_struct param, bool as_pointer = false)
{
    switch (param.type)
    {
        case "reg":
            string reg;

            if (is_temporary_register(param.value))
            {
                if (procedure_optimization_level >= 2)
                {
                    if (param.value in current_temporary_register)
                    {
                        reg = current_temporary_register[param.value].idup;
                    }
                    else
                    {
                        write_error2("Using uninitialized register: " ~ param.value);
                        return "error";
                    }
                }
                else
                {
                    reg = get_new_temporary_register();
                    add_output_line(reg ~ " = load " ~ get_load_type("i32") ~ " %" ~ param.value ~ ", align 4");
                }
            }
            else
            {
                string regnumstr = register_numbers_str[param.value];

                if (procedure_optimization_level >= 1)
                {
                    int regnum = to!int(regnumstr);

                    if (register_state[regnum] == RegisterState.Empty)
                    {
                        register_state[regnum] = RegisterState.Read;

                        string addr = get_new_temporary_register();
                        reg = get_new_temporary_register();
                        add_output_line(addr ~ " = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ regnumstr);
                        add_output_line(reg ~ " = load " ~ get_load_type("i32") ~ " " ~ addr ~ ", align 4");

                        if (procedure_optimization_level >= 2)
                        {
                            current_temporary_register[param.value] = reg.idup;
                        }
                        else
                        {
                            add_output_line("store i32 " ~ reg ~ ", i32* %" ~ param.value ~ ", align 4");
                        }
                    }
                    else
                    {
                        if (procedure_optimization_level >= 2)
                        {
                            reg = current_temporary_register[param.value].idup;
                        }
                        else
                        {
                            reg = get_new_temporary_register();
                            add_output_line(reg ~ " = load " ~ get_load_type("i32") ~ " %" ~ param.value ~ ", align 4");
                        }
                    }
                }
                else
                {
                    string addr = get_new_temporary_register();
                    reg = get_new_temporary_register();
                    add_output_line(addr ~ " = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ regnumstr);
                    add_output_line(reg ~ " = load " ~ get_load_type("i32") ~ " " ~ addr ~ ", align 4");
                }
            }

            if (as_pointer)
            {
                string reg2 = get_new_temporary_register();
                add_output_line(reg2 ~ " = inttoptr i32 " ~ reg ~ " to i8*");
                return reg2;
            }
            else
            {
                return reg;
            }
        case "externaddr":
            if (!position_independent_code || param.value in extern_list || dlabel_list[param.value].isglobal)
            {
                if (param.ivalue == 0)
                {
                    if (as_pointer)
                    {
                        return "@" ~ param.value;
                    }
                    else
                    {
                        return "ptrtoint (i8* @" ~ param.value ~ " to i32)";
                    }
                }
                else
                {
                    if (as_pointer)
                    {
                        return "getelementptr (" ~ get_load_type("i8") ~ " @" ~ param.value ~ ", i32 " ~ to!string(param.ivalue) ~ ")";
                    }
                    else
                    {
                        return "ptrtoint (i8* getelementptr (" ~ get_load_type("i8") ~ " @" ~ param.value ~ ", i32 " ~ to!string(param.ivalue) ~ ") to i32)";
                    }
                }
            }
            else
            {
                auto addrlabel = dlabel_list[param.value];

                if (as_pointer)
                {
                    return "getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ addrlabel.dataseg_name ~ "* @" ~ addrlabel.dataseg_name ~ " to i8*), i32 " ~ to!string(addrlabel.offset + param.ivalue) ~ ")";
                }
                else
                {
                    return "ptrtoint (i8* getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ addrlabel.dataseg_name ~ "* @" ~ addrlabel.dataseg_name ~ " to i8*), i32 " ~ to!string(addrlabel.offset + param.ivalue) ~ ") to i32)";
                }
            }
        case "procaddr":
            if (as_pointer)
            {
                return "bitcast (" ~ ((no_tail_calls)?"i8*":"void") ~ "(%_cpu*)* @" ~ param.value ~ " to i8*)";
            }
            else
            {
                return "ptrtoint (" ~ ((no_tail_calls)?"i8*":"void") ~ "(%_cpu*)* @" ~ param.value ~ " to i32)";
            }
        default:
            if (as_pointer)
            {
                return "inttoptr (i32 (" ~ param.value ~ ") to i8*)";
            }
            else
            {
                return param.value;
            }
    }
}

string get_parameter_read_addr(param_struct param, string ptrtype)
{
    if (param.type == "externaddr")
    {
        if (!position_independent_code || param.value in extern_list || dlabel_list[param.value].isglobal)
        {
            if (param.ivalue == 0)
            {
                if (ptrtype == "i8")
                {
                    return "@" ~ param.value;
                }
                else
                {
                    return "bitcast (i8* @" ~ param.value ~ " to " ~ ptrtype ~ "*)";
                }
            }
            else
            {
                if (ptrtype == "i8")
                {
                    return "getelementptr (" ~ get_load_type("i8") ~ " @" ~ param.value ~ ", i32 " ~ to!string(param.ivalue) ~ ")";
                }
                else
                {
                    return "bitcast (i8* getelementptr (" ~ get_load_type("i8") ~ " @" ~ param.value ~ ", i32 " ~ to!string(param.ivalue) ~ ") to " ~ ptrtype ~ "*)";
                }
            }
        }
        else
        {
            auto addrlabel = dlabel_list[param.value];

            if (ptrtype == "i8")
            {
                return "getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ addrlabel.dataseg_name ~ "* @" ~ addrlabel.dataseg_name ~ " to i8*), i32 " ~ to!string(addrlabel.offset + param.ivalue) ~ ")";
            }
            else
            {
                return "bitcast (i8* getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ addrlabel.dataseg_name ~ "* @" ~ addrlabel.dataseg_name ~ " to i8*), i32 " ~ to!string(addrlabel.offset + param.ivalue) ~ ") to " ~ ptrtype ~ "*)";
            }
        }
    }
    else
    {
        string reg = get_parameter_read_value(param);
        string addr = get_new_temporary_register();

        add_output_line(addr ~ " = inttoptr i32 " ~ reg ~ " to " ~ ptrtype ~ "*");

        return addr;
    }
}

void store_temporary_register_to_reg(string temp_reg, string reg, bool last_reg_write)
{
    if (is_temporary_register(reg))
    {
        if (procedure_optimization_level >= 2)
        {
            current_temporary_register[reg] = temp_reg.idup;
        }
        else
        {
            add_output_line("store i32 " ~ temp_reg ~ ", i32* %" ~ reg ~ ", align 4");
        }
    }
    else
    {
        string regnumstr = register_numbers_str[reg];

        if (procedure_optimization_level >= 1)
        {
            int regnum = to!int(regnumstr);

            register_state[regnum] = RegisterState.Write;
            if (procedure_optimization_level >= 2)
            {
                current_temporary_register[reg] = temp_reg.idup;
            }
            else
            {
                add_output_line("store i32 " ~ temp_reg ~ ", i32* %" ~ reg ~ ", align 4");
            }

            if (last_reg_write)
            {
                register_state[regnum] = RegisterState.Read;

                string addr = get_new_temporary_register();
                add_output_line(addr ~ " = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ regnumstr);
                add_output_line("store i32 " ~ temp_reg ~ ", i32* " ~ addr ~ ", align 4");
            }
        }
        else
        {
            string addr = get_new_temporary_register();
            add_output_line(addr ~ " = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ regnumstr);
            add_output_line("store i32 " ~ temp_reg ~ ", i32* " ~ addr ~ ", align 4");
        }
    }
}

void writeback_register(int regnum)
{
    string reg;

    if (procedure_optimization_level >= 2)
    {
        reg = current_temporary_register[registers_base_list[regnum]].idup;
    }
    else
    {
        reg = get_new_temporary_register();
        add_output_line(reg ~ " = load " ~ get_load_type("i32") ~ " %" ~ registers_base_list[regnum] ~ ", align 4");
    }

    string addr = get_new_temporary_register();
    add_output_line(addr ~ " = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ to!string(regnum));
    add_output_line("store i32 " ~ reg ~ ", i32* " ~ addr ~ ", align 4");

    register_state[regnum] = RegisterState.Read;
}

void writeback_registers()
{
    for (int i = 0; i < num_regs; i++)
    {
        if (register_state[i] == RegisterState.Write)
        {
            writeback_register(i);
        }
    }
}

bool process_proc_body(string proc_name)
{
// todo: optimize access to x86 regs
    current_proc = proc_name;
    auto curproc = proc_list[proc_name];
    bool is_inside_if = false;
    bool is_static_if, static_if_result;
    string endif_label;
    proc_instr_struct[] proc_instr_info;
    RegisterState[num_regs] if_reg_state;
    bool[string] used_reg_list;

    if (curproc.lines.length == 0)
    {
        write_error2("Empty procedure");
        return false;
    }

    proc_instr_info.length = curproc.lines.length;

    procedure_optimization_level = global_optimization_level;

/* ****************************************************************** */
/* analyze / check pass */

    for (int linenum = 0; linenum < curproc.lines.length; linenum++)
    {
        current_line = curproc.lines[linenum];
        auto instruction = instructions_list[current_line.word];

        proc_instr_info[linenum].is_valid = true;
        proc_instr_info[linenum].is_inside_if = false;
        proc_instr_info[linenum].func_has_return_value = false;
        proc_instr_info[linenum].if_contains_call = false;
        proc_instr_info[linenum].write_reg[0] = "";
        proc_instr_info[linenum].write_reg[1] = "";
        proc_instr_info[linenum].last_reg_write[0] = false;
        proc_instr_info[linenum].last_reg_write[1] = false;
        proc_instr_info[linenum].params.length = 0;

        string[] params;

        if (instruction.variable_params2.length != 0)
        {
            if (current_line.word == "call" && instruction.params.length == 1)
            {
                if (current_line.param1 !in func_list)
                {
                    write_error2("Function not found: " ~ current_line.param1);
                    return false;
                }

                auto func = func_list[current_line.param1];

                proc_instr_info[linenum].func_has_return_value = func.has_return_value;

                params = get_current_params2();

                if (params.length != func.params.length)
                {
                    write_error2("Wrong number of function parameters: " ~ current_line.orig);
                    return false;
                }

                for (int i = 0; i < params.length; i++)
                {
                    if (func.params[i].type == ParameterType.Cpu)
                    {
                        if (params[i] != "$")
                        {
                            write_error2("Expecting $ in function parameters: " ~ current_line.orig);
                            return false;
                        }
                    }
                    else
                    {
                        if (params[i] == "$")
                        {
                            write_error2("Unexpected $ in function parameters: " ~ current_line.orig);
                            return false;
                        }
                    }
                }
            }
            else
            {
                write_error2("Instruction error: " ~ current_line.orig);
                return false;
            }
        }
        else
        {
            params = get_current_params();
        }

        param_struct[] paramvals;

        paramvals.length = params.length;

        // get parameters type and value
        for (int i = 0; i < params.length; i++)
        {
            if (params[i] == "$")
            {
                paramvals[i] = param_struct("$".idup, "%cpu".idup);
                continue;
            }

            if (params[i] in registers_list)
            {
                paramvals[i] = param_struct("reg".idup, params[i].idup);

                // update list of used registers in procedure
                used_reg_list[params[i]] = true;

                continue;
            }

            if (params[i] in func_list)
            {
                paramvals[i] = param_struct("funcaddr".idup, params[i]);
                continue;
            }

            long position = params[i].indexOf('[');
            if (position >= 0)
            {
                uint position2 = cast(uint)position;
                string displacement = params[i][position2+1..$].strip();
                if (displacement.length > 0 && displacement[displacement.length - 1] == ']')
                {
                    displacement = displacement[0..$-1].strip();
                    string param_base = params[i][0..position2].strip();

                    if (param_base in define_list)
                    {
                        param_base = define_list[param_base];
                    }

                    if (param_base in extern_list || param_base in dlabel_list)
                    {
                        if (replace_defines_in_expr(displacement))
                        {
                            if (calculate_expr_value(displacement))
                            {
                                paramvals[i] = param_struct("externaddr".idup, param_base.idup, cast(int)(to!long(displacement)));
                                continue;
                            }
                        }
                    }
                }
            }

            string param;
            if (params[i] in define_list)
            {
                param = define_list[params[i]];
            }
            else
            {
                param = params[i];
            }

            if (param in extern_list || param in dlabel_list)
            {
                paramvals[i] = param_struct("externaddr".idup, param.idup, 0);
                continue;
            }

            if (param in proc_list)
            {
                paramvals[i] = param_struct("procaddr".idup, param.idup);
                continue;
            }

            if (replace_defines_in_expr(param))
            {
                if (calculate_expr_value(param))
                {
                    paramvals[i] = param_struct("const".idup, param.idup, cast(int)(to!long(param)));
                    continue;
                }
            }

            write_error2("Unknown instruction parameter: " ~ current_line.orig);
            return false;
        }

        // check parameters type
        if (instruction.variable_params2.length != 0)
        {
            for (int i = 0; i < paramvals.length; i++)
            {
                if (paramvals[i].type !in instruction.variable_params2)
                {
                    write_error2("Wrong instruction parameter type: " ~ current_line.orig);
                    return false;
                }
            }
        }
        else
        {
            for (int i = 0; i < paramvals.length; i++)
            {
                if (paramvals[i].type !in instruction.params2[i])
                {
                    write_error2("Wrong instruction parameter type: " ~ current_line.orig);
                    return false;
                }
            }
        }


        proc_instr_info[linenum].params = paramvals.dup;

        if (is_inside_if)
        {
            if (is_static_if)
            {
                if (!static_if_result)
                {
                    proc_instr_info[linenum].is_valid = false;
                }
            }
            else
            {
                proc_instr_info[linenum].is_inside_if = true;
            }
        }

        switch (current_line.word)
        {
            case "mov": // mov reg, reg/const/addr
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "add":  // add  reg, reg, reg/const
            case "sub":  // sub  reg, reg/const, reg/const
            case "shl":  // shl  reg, reg, reg/const
            case "lshr": // lshr reg, reg, reg/const
            case "ashr": // ashr reg, reg, reg/const
            case "and":  // and  reg, reg, reg/const
            case "or":   // or   reg, reg, reg/const
            case "xor":  // xor  reg, reg, reg/const
            case "mul":  // mul  reg, reg, reg/const
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "load": // load reg, reg/addr, const
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "load8s": // load8s reg, reg/addr, const
            case "load8z": // load8z reg, reg/addr, const
            case "load16s": // load16s reg, reg/addr, const
            case "load16z": // load16z reg, reg/addr, const
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "store": // store reg/const/addr, reg/addr, const
                break;
            case "store8":  // store8 reg/const, reg/addr, const
            case "store16": // store16 reg/const, reg/addr, const
                break;
            case "ext8s":  // ext8s reg, reg
            case "ext16s": // ext16s reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "ins16": // ins16 reg, reg, reg
            case "ins8ll": // ins8ll reg, reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "ins8lh": // ins8lh reg, reg, reg
            case "ins8hl": // ins8hl reg, reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "ins8hh": // ins8hh reg, reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "tcall": // tcall reg/addr
                if (linenum + 1 != curproc.lines.length)
                {
                    write_error2("Instructions after tcall: " ~ current_line.orig);
                    return false;
                }
                break;
            case "ctcallz": // ctcallz reg, addr
            case "ctcallnz": // ctcallnz reg, addr
                if (linenum + 2 != curproc.lines.length || curproc.lines[linenum+1].word != "tcall")
                {
                    write_error2("Instructions after ctcall: " ~ current_line.orig);
                    return false;
                }
                break;
            case "cmovz": // cmovz reg, reg, reg/const, reg/const
                proc_instr_info[linenum].write_reg[0] = paramvals[1].value;
                break;
            case "cmoveq":  // cmoveq reg, reg/const, reg, reg/const, reg/const
            case "cmovult": // cmovult reg, reg/const, reg, reg/const, reg/const
            case "cmovugt": // cmovugt reg, reg/const, reg, reg/const, reg/const
            case "cmovslt": // cmovslt reg, reg/const, reg, reg/const, reg/const
            case "cmovsgt": // cmovsgt reg, reg/const, reg, reg/const, reg/const
                proc_instr_info[linenum].write_reg[0] = paramvals[2].value;
                break;
            case "call": // call funcaddr [$/reg/const]
                if (proc_instr_info[linenum].func_has_return_value)
                {
                    proc_instr_info[linenum].write_reg[0] = "tmp0";

                    // update list of used registers in procedure
                    used_reg_list["tmp0"] = true;
                }
                break;
            case "imul": // imul reg, reg, reg, reg/const
            case "umul": // umul reg, reg, reg, reg/const
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                proc_instr_info[linenum].write_reg[1] = paramvals[1].value;
                break;
            case "ifz":  // ifz reg
            case "ifnz": // ifnz reg
                {
                    if (is_inside_if)
                    {
                        write_error2("If inside if: " ~ current_line.orig);
                        return false;
                    }

                    is_inside_if = true;
                    is_static_if = false;

                    if (procedure_optimization_level >= 1)
                    {
                        if (linenum > 0 && curproc.lines[linenum-1].word == "mov")
                        {
                            if (proc_instr_info[linenum-1].params[0].value == paramvals[0].value && proc_instr_info[linenum-1].params[1].type == "const")
                            {
                                is_static_if = true;
                                int value = proc_instr_info[linenum-1].params[1].ivalue;

                                if (current_line.word == "ifz")
                                {
                                    static_if_result = (value == 0);
                                }
                                else
                                {
                                    static_if_result = (value != 0);
                                }
                            }
                        }

                        if (linenum > 1 && curproc.lines[linenum-2].word == "mov" && curproc.lines[linenum-1].word == "cmovslt")
                        {
                            if (proc_instr_info[linenum-2].params[0].value == paramvals[0].value && proc_instr_info[linenum-2].params[1].type == "const")
                            {
                                if (proc_instr_info[linenum-1].params[0].value == paramvals[0].value &&
                                    proc_instr_info[linenum-1].params[1].type == "const" &&
                                    proc_instr_info[linenum-1].params[2].value == paramvals[0].value &&
                                    proc_instr_info[linenum-1].params[3].type == "const" &&
                                    proc_instr_info[linenum-1].params[4].type == "const"
                                   )
                                {
                                    is_static_if = true;
                                    int value2 = proc_instr_info[linenum-2].params[1].ivalue;
                                    int value1 = proc_instr_info[linenum-1].params[1].ivalue;
                                    int value0;

                                    if (value2 < value1)
                                    {
                                        value0 = proc_instr_info[linenum-1].params[3].ivalue;
                                    }
                                    else
                                    {
                                        value0 = proc_instr_info[linenum-1].params[4].ivalue;
                                    }

                                    if (current_line.word == "ifz")
                                    {
                                        static_if_result = (value0 == 0);
                                    }
                                    else
                                    {
                                        static_if_result = (value0 != 0);
                                    }
                                }
                            }
                        }
                    }

                    if (procedure_optimization_level > 1 && !is_static_if)
                    {
                        procedure_optimization_level = 1;
                    }

                    if (is_static_if)
                    {
                        proc_instr_info[linenum].is_valid = false;
                    }
                }
                break;
            case "endif":
                {
                    if (!is_inside_if)
                    {
                        write_error2("Endif without if: " ~ current_line.orig);
                        return false;
                    }

                    is_inside_if = false;
                    proc_instr_info[linenum].is_inside_if = false;

                    if (is_static_if)
                    {
                        proc_instr_info[linenum].is_valid = false;
                    }
                }
                break;
            case "ctlz": // ctlz reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            case "bswap": // bswap reg, reg
                proc_instr_info[linenum].write_reg[0] = paramvals[0].value;
                break;
            default:
                write_error2("Unhandled instruction: " ~ current_line.orig);
                return false;
        }
    }

    if (is_inside_if)
    {
        write_error2("Missing endif: " ~ current_line.orig);
        return false;
    }

/* ****************************************************************** */
/* optimization pass */

    if (procedure_optimization_level >= 1)
    {
        bool if_contains_call;
        bool[string] last_reg_write;

        foreach (reg; registers_base_list)
        {
            last_reg_write[reg] = true;
        }

        for (int linenum = cast(int)(curproc.lines.length) - 1; linenum >= 0; linenum--)
        {
            if (!proc_instr_info[linenum].is_valid) continue;

            string instr_word = curproc.lines[linenum].word;

            if (is_inside_if)
            {
                if (instr_word == "call")
                {
                    if_contains_call = true;
                }
                else if (instr_word == "ifz" || instr_word == "ifnz")
                {
                    is_inside_if = false;
                    proc_instr_info[linenum].if_contains_call = if_contains_call;
                }
            }
            else
            {
                if (instr_word == "endif")
                {
                    is_inside_if = true;
                    if_contains_call = false;
                }
            }

            for (int i = 0; i <= 1; i++)
            {
                string reg = proc_instr_info[linenum].write_reg[i];

                if (reg != "")
                {
                    if (last_reg_write[reg])
                    {
                        if (!proc_instr_info[linenum].is_inside_if)
                        {
                            proc_instr_info[linenum].last_reg_write[i] = true;
                        }
                    }

                    last_reg_write[reg] = false;
                }
            }

            if (instr_word == "call" || ((instr_word == "ifz" || instr_word == "ifnz") && proc_instr_info[linenum].if_contains_call))
            {
                foreach (reg; registers_base_list)
                {
                    last_reg_write[reg] = true;
                }
            }
        }
    }

/* ****************************************************************** */
/* output pass */

    temporary_register_index = 0;
    label_index = 0;
    current_temporary_register = (string[string]).init; // clear list
    if (current_temporary_register.length != 0)
    {
        write_error2("Error clearing current_temporary_register");
        return false;
    }

    if (procedure_optimization_level >= 1)
    {
        for (int i = 0; i < num_regs; i++)
        {
            register_state[i] = RegisterState.Empty;
            if (procedure_optimization_level == 1 && registers_base_list[i] in used_reg_list)
            {
                add_output_line("%" ~ registers_base_list[i] ~ " = alloca i32, align 4");
            }
        }
    }

    if (procedure_optimization_level <= 1)
    {
        for (int i = num_regs; i < registers_base_list.length; i++)
        {
            if (registers_base_list[i] in used_reg_list)
            {
                add_output_line("%" ~ registers_base_list[i] ~ " = alloca i32, align 4");
            }
        }
    }

    for (int linenum = 0; linenum < curproc.lines.length; linenum++)
    {
        current_line = curproc.lines[linenum];

        add_output_line(";" ~ current_line.orig);
        if (!proc_instr_info[linenum].is_valid) continue;

        param_struct[] paramvals = proc_instr_info[linenum].params;
        bool[2] last_reg_write = proc_instr_info[linenum].last_reg_write;


        switch (current_line.word)
        {
            case "mov": // mov reg, reg/const/addr
                {
                    string src = get_parameter_read_value(paramvals[1]);
                    store_temporary_register_to_reg(src, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "add":  // add  reg, reg, reg/const
            case "sub":  // sub  reg, reg/const, reg/const
            case "shl":  // shl  reg, reg, reg/const
            case "lshr": // lshr reg, reg, reg/const
            case "ashr": // ashr reg, reg, reg/const
            case "and":  // and  reg, reg, reg/const
            case "or":   // or   reg, reg, reg/const
            case "xor":  // xor  reg, reg, reg/const
            case "mul":  // mul  reg, reg, reg/const
                {
                    string src1 = get_parameter_read_value(paramvals[1]);
                    string src2 = get_parameter_read_value(paramvals[2]);
                    string dst = get_new_temporary_register();

                    add_output_line(dst ~ " = " ~ current_line.word ~ " i32 " ~ src1 ~ ", " ~ src2);

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "load": // load reg, reg/addr, const
                {
                    string src = get_parameter_read_addr(paramvals[1], "i32");
                    string dst = get_new_temporary_register();

                    add_output_line(dst ~ " = load " ~ get_load_type("i32") ~ " " ~ src ~ ", align " ~ paramvals[2].value);

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "load8s": // load8s reg, reg/addr, const
            case "load8z": // load8z reg, reg/addr, const
            case "load16s": // load16s reg, reg/addr, const
            case "load16z": // load16z reg, reg/addr, const
                {
                    string srctype = (current_line.word == "load8s" || current_line.word == "load8z")?"i8":"i16";
                    string extinstr = (current_line.word == "load8s" || current_line.word == "load16s")?"sext":"zext";

                    string src = get_parameter_read_addr(paramvals[1], srctype);
                    string tmpdst = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    add_output_line(tmpdst ~ " = load " ~ get_load_type(srctype) ~ " " ~ src ~ ", align " ~ paramvals[2].value);
                    add_output_line(dst ~ " = " ~ extinstr ~ " " ~ srctype ~ " " ~ tmpdst ~ " to i32");

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "store": // store reg/const/addr, reg/addr, const
                {
                    string src = get_parameter_read_value(paramvals[0]);
                    string dst = get_parameter_read_addr(paramvals[1], "i32");

                    add_output_line("store i32 " ~ src ~ ", i32* " ~ dst ~ ", align " ~ paramvals[2].value);
                }
                break;
            case "store8":  // store8 reg/const, reg/addr, const
            case "store16": // store16 reg/const, reg/addr, const
                {
                    string srctype = (current_line.word == "store8")?"i8":"i16";

                    string src = get_parameter_read_value(paramvals[0]);
                    string dst = get_parameter_read_addr(paramvals[1], srctype);
                    string src2 = get_new_temporary_register();

                    add_output_line(src2 ~ " = trunc i32 " ~ src ~ " to " ~ srctype);
                    add_output_line("store " ~ srctype ~ " " ~ src2 ~ ", " ~ srctype ~ "* " ~ dst ~ ", align " ~ paramvals[2].value);
                }
                break;
            case "ext8s":  // ext8s reg, reg
            case "ext16s": // ext16s reg, reg
                {
                    string src = get_parameter_read_value(paramvals[1]);
                    string src2 = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    string srctype = (current_line.word == "ext8s")?"i8":"i16";

                    add_output_line(src2 ~ " = trunc i32 " ~ src ~ " to " ~ srctype);
                    add_output_line(dst ~ " = sext " ~ srctype ~ " " ~ src2 ~ " to i32");

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "ins16": // ins16 reg, reg, reg
            case "ins8ll": // ins8ll reg, reg, reg
                {
                    string src1 = get_parameter_read_value(paramvals[1]);
                    string src2 = get_parameter_read_value(paramvals[2]);
                    string src21 = get_new_temporary_register();
                    string src22 = get_new_temporary_register();
                    string src11 = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    string srctype = (current_line.word == "ins16")?"i16":"i8";
                    uint andvalue = (current_line.word == "ins16")?0xffff0000:0xffffff00;

                    add_output_line(src21 ~ " = trunc i32 " ~ src2 ~ " to " ~ srctype);
                    add_output_line(src22 ~ " = zext " ~ srctype ~ " " ~ src21 ~ " to i32");
                    add_output_line(src11 ~ " = and i32 " ~ src1 ~ ", " ~ to!string(andvalue));
                    add_output_line(dst ~ " = or i32 " ~ src11 ~ ", " ~ src22);

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "ins8lh": // ins8lh reg, reg, reg
            case "ins8hl": // ins8hl reg, reg, reg
                {
                    string src1 = get_parameter_read_value(paramvals[1]);
                    string src2 = get_parameter_read_value(paramvals[2]);
                    string src21 = get_new_temporary_register();
                    string src22 = get_new_temporary_register();
                    string src23 = get_new_temporary_register();
                    string src11 = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    if (current_line.word == "ins8lh")
                    {
                        add_output_line(src21 ~ " = lshr i32 " ~ src2 ~ ", 8");
                        add_output_line(src22 ~ " = trunc i32 " ~ src21 ~ " to i8");
                        add_output_line(src23 ~ " = zext i8 " ~ src22 ~ " to i32");
                        add_output_line(src11 ~ " = and i32 " ~ src1 ~ ", " ~ to!string(0xffffff00));
                    }
                    else
                    {
                        add_output_line(src21 ~ " = trunc i32 " ~ src2 ~ " to i8");
                        add_output_line(src22 ~ " = zext i8 " ~ src21 ~ " to i32");
                        add_output_line(src23 ~ " = shl i32 " ~ src22 ~ ", 8");
                        add_output_line(src11 ~ " = and i32 " ~ src1 ~ ", " ~ to!string(0xffff00ff));
                    }

                    add_output_line(dst ~ " = or i32 " ~ src11 ~ ", " ~ src23);

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "ins8hh": // ins8hh reg, reg, reg
                {
                    string src1 = get_parameter_read_value(paramvals[1]);
                    string src2 = get_parameter_read_value(paramvals[2]);
                    string src21 = get_new_temporary_register();
                    string src11 = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    add_output_line(src21 ~ " = and i32 " ~ src2 ~ ", " ~ to!string(0xff00));
                    add_output_line(src11 ~ " = and i32 " ~ src1 ~ ", " ~ to!string(0xffff00ff));
                    add_output_line(dst ~ " = or i32 " ~ src11 ~ ", " ~ src21);

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "tcall": // tcall reg/addr
                {
                    if (procedure_optimization_level >= 1)
                    {
                        writeback_registers();
                    }

                    if (no_tail_calls)
                    {
                        string addr = get_parameter_read_value(paramvals[0], true);
                        add_output_line("ret i8* " ~ addr);
                    }
                    else
                    {
                        if (paramvals[0].type == "procaddr")
                        {
                            add_output_line("musttail call fastcc void @" ~ paramvals[0].value ~ "(%_cpu* %cpu) nounwind");
                        }
                        else
                        {
                            string dst = get_parameter_read_value(paramvals[0]);
                            string addr = get_new_temporary_register();

                            add_output_line(addr ~ " = inttoptr i32 " ~ dst ~ " to void(%_cpu*)*");
                            add_output_line("musttail call fastcc void " ~ addr ~ "(%_cpu* %cpu) nounwind");
                        }
                    }
                }
                break;
            case "ctcallz": // ctcallz reg, addr
            case "ctcallnz": // ctcallnz reg, addr
                {
                    if (procedure_optimization_level >= 1)
                    {
                        writeback_registers();
                    }

                    string condreg = get_parameter_read_value(paramvals[0]);
                    string cond = get_new_temporary_register();
                    string labelt = get_new_label();
                    string labelf = get_new_label();

                    string condtype = (current_line.word == "ctcallz")?"eq":"ne";

                    add_output_line(cond ~ " = icmp " ~ condtype ~ " i32 " ~ condreg ~ ", 0");
                    add_output_line("br i1 " ~ cond ~ ", label %" ~ labelt ~ ", label %" ~ labelf);
                    add_output_line(labelt ~ ":");

                    if (no_tail_calls)
                    {
                        string addr = get_parameter_read_value(paramvals[1], true);
                        add_output_line("ret i8* " ~ addr);
                    }
                    else
                    {
                        add_output_line("musttail call fastcc void @" ~ paramvals[1].value ~ "(%_cpu* %cpu) nounwind");

                        add_output_line("ret void");
                    }
                    add_output_line(labelf ~ ":");
                }
                break;
            case "cmovz": // cmovz reg, reg, reg/const, reg/const
                {
                    string condreg = get_parameter_read_value(paramvals[0]);
                    string src1 = get_parameter_read_value(paramvals[2]);
                    string src2 = get_parameter_read_value(paramvals[3]);
                    string cond = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    add_output_line(cond ~ " = icmp eq i32 " ~ condreg ~ ", 0");
                    add_output_line(dst ~ " = select i1 " ~ cond ~ ", i32 " ~ src1 ~ ", i32 " ~ src2);

                    store_temporary_register_to_reg(dst, paramvals[1].value, last_reg_write[0]);
                }
                break;
            case "cmoveq":  // cmoveq reg, reg/const, reg, reg/const, reg/const
            case "cmovult": // cmovult reg, reg/const, reg, reg/const, reg/const
            case "cmovugt": // cmovugt reg, reg/const, reg, reg/const, reg/const
            case "cmovslt": // cmovslt reg, reg/const, reg, reg/const, reg/const
            case "cmovsgt": // cmovsgt reg, reg/const, reg, reg/const, reg/const
                {
                    string condreg1 = get_parameter_read_value(paramvals[0]);
                    string condreg2 = get_parameter_read_value(paramvals[1]);
                    string src1 = get_parameter_read_value(paramvals[3]);
                    string src2 = get_parameter_read_value(paramvals[4]);
                    string cond = get_new_temporary_register();
                    string dst = get_new_temporary_register();

                    string condtype = current_line.word[4..$];

                    add_output_line(cond ~ " = icmp " ~ condtype ~ " i32 " ~ condreg1 ~ ", " ~ condreg2);
                    add_output_line(dst ~ " = select i1 " ~ cond ~ ", i32 " ~ src1 ~ ", i32 " ~ src2);

                    store_temporary_register_to_reg(dst, paramvals[2].value, last_reg_write[0]);
                }
                break;
            case "call": // call funcaddr [$/reg/const]
                {
                    string[] src;

                    if (procedure_optimization_level >= 1)
                    {
                        writeback_registers();
                    }

                    auto func = func_list[current_line.param1];

                    src.length = paramvals.length;
                    for (int i = 0; i < paramvals.length; i++)
                    {
                        src[i] = get_parameter_read_value(paramvals[i], (func.params[i].type == ParameterType.Pointer));
                    }

                    string srcstr = "";
                    for (int i = 0; i < src.length; i++)
                    {
                        string datatype;
                        switch (func.params[i].type)
                        {
                            case ParameterType.Cpu:
                                datatype = "%_cpu*";
                                break;
                            case ParameterType.Pointer:
                                datatype = "i8*";
                                break;
                            default:
                                datatype = "i32";
                                break;
                        }

                        if (i > 0)
                        {
                            srcstr ~= ", ";
                        }
                        srcstr = srcstr ~ datatype ~ " " ~ src[i];
                    }

                    if (procedure_optimization_level >= 1)
                    {
                        for (int i = 0; i < num_regs; i++)
                        {
                            register_state[i] = RegisterState.Empty;
                        }
                        // todo: vycistit vsetko, alebo iba x86 registre ?
                        current_temporary_register = (string[string]).init; // clear list
                    }

                    if (func.has_return_value)
                    {
                        string dst = get_new_temporary_register();

                        add_output_line(dst ~ " = call i32 @" ~ current_line.param1 ~ "(" ~ srcstr ~ ") nounwind");

                        store_temporary_register_to_reg(dst, "tmp0", last_reg_write[0]);
                    }
                    else
                    {
                        add_output_line("call void @" ~ current_line.param1 ~ "(" ~ srcstr ~ ") nounwind");
                    }
                }
                break;
            case "imul": // imul reg, reg, reg, reg/const
            case "umul": // umul reg, reg, reg, reg/const
                {
                    string src1 = get_parameter_read_value(paramvals[2]);
                    string src2 = get_parameter_read_value(paramvals[3]);
                    string src11 = get_new_temporary_register();
                    string src21 = get_new_temporary_register();
                    string dst = get_new_temporary_register();
                    string dstlo = get_new_temporary_register();
                    string tmpdst = get_new_temporary_register();
                    string dsthi = get_new_temporary_register();

                    string extinstr = (current_line.word == "imul")?"sext":"zext";

                    add_output_line(src11 ~ " = " ~ extinstr ~ " i32 " ~ src1 ~ " to i64");
                    add_output_line(src21 ~ " = " ~ extinstr ~ " i32 " ~ src2 ~ " to i64");
                    add_output_line(dst ~ " = mul i64 " ~ src11 ~ ", " ~ src21);
                    add_output_line(dstlo ~ " = trunc i64 " ~ dst ~ " to i32");
                    add_output_line(tmpdst ~ " = lshr i64 " ~ dst ~ ", 32");
                    add_output_line(dsthi ~ " = trunc i64 " ~ tmpdst ~ " to i32");

                    store_temporary_register_to_reg(dstlo, paramvals[0].value, last_reg_write[0]);
                    store_temporary_register_to_reg(dsthi, paramvals[1].value, last_reg_write[1]);
                }
                break;
            case "ifz":  // ifz reg
            case "ifnz": // ifnz reg
                {
                    if (procedure_optimization_level >= 1)
                    {
                        if (proc_instr_info[linenum].if_contains_call)
                        {
                            writeback_registers();
                        }
                    }

                    string condreg = get_parameter_read_value(paramvals[0]);
                    string cond = get_new_temporary_register();
                    string labelt = get_new_label();
                    string labelf = get_new_label();

                    string condtype = (current_line.word == "ifz")?"eq":"ne";

                    if (procedure_optimization_level >= 1)
                    {
                        if_reg_state = register_state.dup;
                    }

                    add_output_line(cond ~ " = icmp " ~ condtype ~ " i32 " ~ condreg ~ ", 0");
                    add_output_line("br i1 " ~ cond ~ ", label %" ~ labelt ~ ", label %" ~ labelf);
                    add_output_line(labelt ~ ":");

                    endif_label = labelf;
                }
                break;
            case "endif":
                {
                    if (procedure_optimization_level >= 1)
                    {
                        for (int i = 0; i < num_regs; i++)
                        {
                            if (register_state[i] == if_reg_state[i]) continue;

                            if (register_state[i] == RegisterState.Write)
                            {
                                writeback_register(i);
                            }

                            if (if_reg_state[i] == RegisterState.Write)
                            {
                                if (register_state[i] == RegisterState.Empty)
                                {
                                    get_parameter_read_value(param_struct("reg", registers_base_list[i]));
                                }
                                register_state[i] = RegisterState.Write;
                            }

                            if (register_state[i] != if_reg_state[i])
                            {
                                register_state[i] = RegisterState.Empty;
                            }
                        }
                    }

                    add_output_line("br label %" ~ endif_label);
                    add_output_line(endif_label ~ ":");
                }
                break;
            case "ctlz": // ctlz reg, reg
                {
                    string src = get_parameter_read_value(paramvals[0]);
                    string dst = get_new_temporary_register();

                    add_output_line(dst ~ " = call i32 @llvm.ctlz.i32(i32 " ~ src ~ ", i1 0)");

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            case "bswap": // bswap reg, reg
                {
                    string src = get_parameter_read_value(paramvals[0]);
                    string dst = get_new_temporary_register();

                    add_output_line(dst ~ " = call i32 @llvm.bswap.i32(i32 " ~ src ~ ")");

                    store_temporary_register_to_reg(dst, paramvals[0].value, last_reg_write[0]);
                }
                break;
            default:
                write_error2("Unhandled instruction: " ~ current_line.orig);
                return false;
        }
    }

    if (!no_tail_calls)
    {
        add_output_line("ret void");
    }
    return true;
}

void write_output()
{
    output_lines.length = num_output_lines;

    if (output_filename == "")
    {
        foreach (line; output_lines)
        {
            stdout.writeln(line);
        }
    }
    else
    {
        scope File fd;
        fd.open(output_filename, "wt");

        foreach (line; output_lines)
        {
            fd.writeln(line);
        }

        fd.close();
    }
}

public int main(string[] args)
{
    input_filename = "";
    output_filename = "";
    return_procedure = "";
    dispatcher_procedure = "";
    include_directories.length = 0;
    output_preprocessed_file = false;
    global_optimization_level = 0;
    position_independent_code = false;
    old_bitcode = false;
    no_tail_calls = false;
    for (int i = 1; i < args.length; i++)
    {
        switch(args[i])
        {
            case "-o":
                if (i + 1 == args.length)
                {
                    stderr.writeln("Missing argument: " ~ args[i]);
                    return 2;
                }

                output_filename = args[i + 1].idup;
                i++;
                break;
            case "-I":
                if (i + 1 == args.length)
                {
                    stderr.writeln("Missing argument: " ~ args[i]);
                    return 2;
                }

                include_directories.length++;
                include_directories[include_directories.length - 1] = args[i + 1].idup;
                i++;
                break;
            case "-O":
            case "-O2":
                global_optimization_level = 2;
                break;
            case "-O1":
                global_optimization_level = 1;
                break;
            case "-no-tail-calls":
                no_tail_calls = true;
                break;
            case "-p":
                output_preprocessed_file = true;
                break;
            case "-pic":
                position_independent_code = true;
                break;
            default:
                if (input_filename == "")
                {
                    input_filename = args[i].idup;
                }
                else
                {
                    stderr.writeln("Bad argument: " ~ args[i]);
                    return 1;
                }
        }
    }

    if (input_filename == "")
    {
        stderr.writeln("Missing input filename");
        return 3;
    }

    if (!exists(input_filename))
    {
        stderr.writeln("Input file not found: " ~ input_filename);
        return 4;
    }

    initialize();

    input_directory = dirName(absolutePath(input_filename)).idup;

    //add_source_file(baseName(input_filename));

    file_input_level = 0;
    file_input_stack.length = 1;
    file_input_stack[0].ismacro = false;
    file_input_stack[0].name = input_filename.idup;
    file_input_stack[0].fullpath = absolutePath(input_filename).idup;
    file_input_stack[0].linenum = 0;
    file_input_stack[0].fd.open(input_filename);

    end_of_input = false;
    input_reading_proc = false;
    input_reading_dataseg = false;
    local_proc_names.length = 0;

    num_output_lines = 0;

    while (true)
    {
        read_next_line();
        if (end_of_input) break;

        // preprocessing
        if (current_line.word == "include")
        {
            if (output_preprocessed_file)
            {
                add_output_line(";" ~ current_line.orig);
            }

            if (current_line.params == "")
            {
                write_error("Missing include filename");
                return 5;
            }

            if (!add_include_to_stack(current_line.params))
            {
                return 6;
            }

            continue;
        }

        if (current_line.word == "macro")
        {
            if (output_preprocessed_file)
            {
                add_output_line(";" ~ current_line.orig);
            }

            if (current_line.param1 == "")
            {
                write_error("Missing macro name");
                return 7;
            }

            if (current_line.param1 in macro_list)
            {
                write_error("Macro already defined: " ~ current_line.param1);
                return 8;
            }

            if (current_line.param1 in keywords_list || current_line.param1 in instructions_list)
            {
                write_error("Macro name equals keyword/instruction: " ~ current_line.param1);
                return 9;
            }

            macro_struct newmacro;

            newmacro.name = current_line.param1.idup;
            newmacro.params = get_current_params2();
            newmacro.lines.length = 0;

            int[2][] index_sort;
            index_sort.length = newmacro.params.length;
            for (int i = 0; i < index_sort.length; i++)
            {
                int[2] entry;
                entry[0] = i;
                entry[1] = cast(int)(newmacro.params[i].length);
                index_sort[i] = entry;
            }

            auto index_sort2 = sort!(paramsComp)(index_sort);

            newmacro.index.length = newmacro.params.length;
            for (int i = 0; i < newmacro.index.length; i++)
            {
                newmacro.index[i] = index_sort2[i][0];
            }

            while (true)
            {
                read_next_line();
                if (end_of_input)
                {
                    stderr.writeln("Missing end of macro: " ~ newmacro.name);
                    return 10;
                }

                if (current_line.word == "endm")
                {
                    if (current_line.params != "")
                    {
                        write_error("Additional data after macro end: " ~ newmacro.name);
                        return 11;
                    }

                    break;
                }

                newmacro.lines.length++;

                newmacro.lines[newmacro.lines.length - 1] = current_line.orig.idup;
            }

            macro_list[newmacro.name] = newmacro;

            continue;
        }


        if (current_line.word in macro_list)
        {
            if (output_preprocessed_file)
            {
                add_output_line(";" ~ current_line.orig);
            }

            if (!add_macro_to_stack(current_line.word, get_current_params()))
            {
                return 12;
            }

            continue;
        }

        if (output_preprocessed_file)
        {
            add_output_line(current_line.orig);
            continue;
        }

        if (current_line.word == "")
        {
            continue;
        }

        // normal processing
        if (current_line.word == "define")
        {
            if (input_reading_proc || input_reading_dataseg)
            {
                write_error("Define inside proc/dataseg");
                return 13;
            }

            if (current_line.param1 == "")
            {
                write_error("Define name missing");
                return 14;
            }

            if (current_line.param1 in define_list)
            {
                write_error("Define already defined: " ~ current_line.param1);
                return 15;
            }

            if (current_line.param1 in func_list || current_line.param1 in registers_list || current_line.param1 in dlabel_list || current_line.param1 in dataseg_list)
            {
                write_error("Define equals func/register/dlabel/dataseg: " ~ current_line.param1);
                return 16;
            }

            if (current_line.param1 in proc_list || current_line.param1 in extern_list)
            {
                write_error("Define defined after proc/extern: " ~ current_line.param1);
                return 17;
            }

            if (current_line.param1 == current_line.params)
            {
                write_error("Define value missing: " ~ current_line.param1);
                return 18;
            }

            define_list[current_line.param1] = current_line.params[current_line.param1.length+1..$].strip().idup;

            continue;
        }

        if (current_line.word == "extern")
        {
            if (input_reading_proc || input_reading_dataseg)
            {
                write_error("Extern inside proc/dataseg");
                return 19;
            }

            if (current_line.param1 == "")
            {
                write_error("Extern name missing");
                return 20;
            }

            string extern_name;

            if (current_line.param1 in define_list)
            {
                extern_name = define_list[current_line.param1];

                if (extern_name.indexOf(' ') != -1)
                {
                    write_error("Extern name contains spaces: " ~ extern_name ~ " (" ~ current_line.param1 ~ ")");
                    return 21;
                }
            }
            else
            {
                extern_name = current_line.param1;
            }

            if (extern_name in extern_list)
            {
                write_error("Extern already defined: " ~ extern_name ~ " (" ~ current_line.param1 ~ ")");
                return 22;
            }

            if (extern_name in func_list || extern_name in proc_list || extern_name in dlabel_list || extern_name in dataseg_list)
            {
                write_error("Extern equals func/proc/dlabel/dataseg: " ~ extern_name ~ " (" ~ current_line.param1 ~ ")");
                return 23;
            }

            if (current_line.param1 != current_line.params)
            {
                write_error("Additional data after extern: " ~ current_line.params);
                return 24;
            }

            extern_list[extern_name] = true;

            continue;
        }

        if (current_line.word == "func" || current_line.word == "funcv")
        {
            if (input_reading_proc || input_reading_dataseg)
            {
                write_error("Func inside proc/dataseg");
                return 25;
            }

            if (current_line.param1 == "")
            {
                write_error("Func name missing");
                return 26;
            }

            if (current_line.param1 in func_list)
            {
                write_error("Func already defined: " ~ current_line.param1);
                return 27;
            }

            if (current_line.param1 in define_list || current_line.param1 in extern_list || current_line.param1 in proc_list || current_line.param1 in dlabel_list || current_line.param1 in dataseg_list)
            {
                write_error("Func equals define/extern/proc/dlabel/dataseg: " ~ current_line.param1);
                return 28;
            }

            func_struct newfunc;

            newfunc.name = current_line.param1.idup;
            newfunc.has_return_value = (current_line.word == "func");
            newfunc.params = get_functions_params(get_current_params2());

            func_list[current_line.param1] = newfunc;

            continue;
        }

        if (current_line.word == "proc")
        {
            if (input_reading_proc || input_reading_dataseg)
            {
                write_error("Proc inside proc/dataseg");
                return 29;
            }

            if (current_line.param1 == "")
            {
                write_error("Proc name missing");
                return 30;
            }

            string proc_name, alias_name;

            if (current_line.param1 in define_list)
            {
                alias_name = current_line.param1;
                proc_name = define_list[current_line.param1];

                if (proc_name.indexOf(' ') != -1)
                {
                    write_error("Proc name contains spaces: " ~ proc_name ~ " (" ~ alias_name ~ ")");
                    return 31;
                }
            }
            else
            {
                proc_name = current_line.param1;
                alias_name = "";
            }

            if (proc_name in proc_list)
            {
                write_error("Proc already defined: " ~ proc_name ~ " (" ~ alias_name ~ ")");
                return 32;
            }

            if (proc_name in extern_list || proc_name in func_list || proc_name in dlabel_list || proc_name in dataseg_list)
            {
                write_error("Proc equals extern/func/dlabel/dataseg: " ~ proc_name ~ " (" ~ alias_name ~ ")");
                return 33;
            }

            proc_struct newproc;

            newproc.name = proc_name.idup;
            newproc.alias_name = alias_name.idup;

            newproc.isexternal = false;
            newproc.ispublic = false;
            newproc.isglobal = false;
            newproc.global_name = "".idup;
            newproc.lines.length = 0;

            if (current_line.param1 == current_line.params)
            {
                input_reading_proc = true;
            }
            else
            {
                string str = current_line.params[current_line.param1.length+1..$].strip();

                if (str == "external")
                {
                    newproc.isexternal = true;
                }
                else if (str == "public")
                {
                    newproc.ispublic = true;
                    input_reading_proc = true;
                }
                else
                {
                    long position = str.indexOf(' ');

                    if (position >= 0 && str[0..cast(uint)position] == "global")
                    {
                        uint position2 = cast(uint)position;

                        newproc.isglobal = true;
                        input_reading_proc = true;

                        str = str[position2+1..$].strip();

                        position = str.indexOf(' ');
                        if (position >= 0)
                        {
                            write_error("Global proc name contains spaces: " ~ str);
                            return 34;
                        }
                        else
                        {
                            newproc.global_name = str.idup;
                        }
                    }
                    else
                    {
                        write_error("Unknown aditional data after proc: " ~ str);
                        return 35;
                    }
                }
            }

            proc_list[proc_name] = newproc;

            current_proc = newproc.name;
            current_proc_lines = 0;

            if (!newproc.isexternal)
            {
                int index = cast(int)(local_proc_names.length);
                local_proc_names.length = index + 1;
                local_proc_names[index] = newproc.name;
            }

            continue;
        }

        if (current_line.word == "endp")
        {
            if (!input_reading_proc)
            {
                write_error("Endp outside proc");
                return 36;
            }

            if (current_line.params != "")
            {
                write_error("Additional data after endp: " ~ current_line.params);
                return 37;
            }

            input_reading_proc = false;

            auto curproc = proc_list[current_proc];
            curproc.lines = proc_instructions[0..current_proc_lines].dup;
            proc_list[current_proc] = curproc;

            continue;
        }

        if (current_line.word in instructions_list)
        {
            if (!input_reading_proc)
            {
                write_error("Instruction outside proc");
                return 38;
            }

            if (!check_instruction_pass1(current_line))
            {
                return 39;
            }


            if (current_proc_lines >= proc_instructions.length)
            {
                proc_instructions.length = 1 + 2 * proc_instructions.length;
            }

            proc_instructions[current_proc_lines] = current_line;
            current_proc_lines++;

            continue;
        }


        if (current_line.word == "datasegment")
        {
            if (input_reading_proc || input_reading_dataseg)
            {
                write_error("Datasegment inside proc/dataseg");
                return 40;
            }

            if (current_line.param1 == "")
            {
                write_error("Datasegment name missing");
                return 41;
            }

            string dataseg_name = current_line.param1.replace(".", "_");

            if (dataseg_name != current_line.param1)
            {
                stderr.writeln("Replacing datasegment name " ~ current_line.param1 ~ " with " ~ dataseg_name);
            }

            if (dataseg_name in dataseg_list)
            {
                write_error("Datasegment already defined: " ~ dataseg_name);
                return 42;
            }

            if (dataseg_name in proc_list || dataseg_name in extern_list || dataseg_name in func_list || dataseg_name in dlabel_list)
            {
                write_error("Datasegment equals proc/extern/func/dlabel: " ~ dataseg_name);
                return 43;
            }


            dataseg_struct newdataseg;

            newdataseg.name = dataseg_name.idup;
            newdataseg.isconstant = false;
            newdataseg.isuninitialized = false;

            current_dataseg = newdataseg.name;
            current_dataseg_offset = 0;
            current_dataseg_numlabels = 0;
            dataseg_labels.length = 0;
            current_dataseg_numdata = 0;
            dataseg_data.length = 0;
            current_dataseg_isuninitialized = false;

            input_reading_dataseg = true;

            if (current_line.param1 != current_line.params)
            {
                string str = current_line.params[current_line.param1.length+1..$].strip();

                if (str == "constant")
                {
                    newdataseg.isconstant = true;
                }
                else if (str == "uninitialized")
                {
                    newdataseg.isuninitialized = true;
                    current_dataseg_isuninitialized = true;
                }
                else
                {
                    write_error("Unknown aditional data after dataseg: " ~ str);
                    return 44;
                }
            }

            dataseg_list[dataseg_name] = newdataseg;

            continue;
        }

        if (current_line.word == "dlabel")
        {
            if (!input_reading_dataseg)
            {
                write_error("Dlabel outside dataseg");
                return 45;
            }

            if (current_line.param1 == "")
            {
                write_error("Dlabel name missing");
                return 46;
            }

            if (current_line.param1 in dlabel_list)
            {
                write_error("Dlabel already defined: " ~ current_line.param1);
                return 47;
            }

            if (current_line.param1 in proc_list || current_line.param1 in extern_list || current_line.param1 in func_list || current_line.param1 in dataseg_list)
            {
                write_error("Dlabel equals proc/extern/func/dataseg: " ~ current_line.param1);
                return 48;
            }


            dlabel_struct newlabel;

            newlabel.name = current_line.param1.idup;
            newlabel.dataseg_name = current_dataseg;
            newlabel.offset = current_dataseg_offset;
            newlabel.isglobal = false;

            if (current_line.param1 != current_line.params)
            {
                string str = current_line.params[current_line.param1.length+1..$].strip();

                if (str == "global")
                {
                    newlabel.isglobal = true;
                }
                else
                {
                    write_error("Unknown aditional data after dlabel: " ~ str);
                    return 49;
                }
            }

            if (current_dataseg_numlabels >= dataseg_labels.length)
            {
                dataseg_labels.length = 1 + 2 * dataseg_labels.length;
            }

            dataseg_labels[current_dataseg_numlabels] = newlabel;
            current_dataseg_numlabels++;

            dlabel_list[current_line.param1] = newlabel;

            continue;
        }

        if (current_line.word == "dalign")
        {
            if (!input_reading_dataseg)
            {
                write_error("Dalign outside dataseg");
                return 50;
            }

            if (current_line.param1 == "")
            {
                write_error("Dalign parameter missing");
                return 51;
            }

            if (!is_number(current_line.param1))
            {
                write_error("Dalign parameter is not a number: " ~ current_line.param1);
                return 52;
            }

            if (current_line.param1 != current_line.params)
            {
                write_error("Unknown aditional data after dalign: " ~ current_line.params);
                return 53;
            }

            int alignnum = cast(int)(get_number_value(current_line.param1));

            if (alignnum != 1 && alignnum != 2 && alignnum != 4)
            {
                write_error("Dalign has wrong alignment: " ~ current_line.param1);
                return 54;
            }

            if (alignnum > 1)
            {
                int alignment = current_dataseg_offset % alignnum;
                if (alignment > 0)
                {
                    int numbytes = alignnum - alignment;

                    if (current_dataseg_numdata > 0 && !dataseg_data[current_dataseg_numdata - 1].isaddr)
                    {
                        // extend existing data
                        int datalen = dataseg_data[current_dataseg_numdata - 1].datalen;

                        if (datalen + numbytes - 1 >= dataseg_data[current_dataseg_numdata - 1].data.length)
                        {
                            dataseg_data[current_dataseg_numdata - 1].data.length = numbytes + 1 + 2 * dataseg_data[current_dataseg_numdata - 1].data.length;
                        }


                        dataseg_data[current_dataseg_numdata - 1].data[datalen..datalen + numbytes] = 0;

                        dataseg_data[current_dataseg_numdata - 1].datalen += numbytes;
                    }
                    else
                    {
                        // add data
                        if (current_dataseg_numdata >= dataseg_data.length)
                        {
                            dataseg_data.length = 4 + 2 * dataseg_data.length;
                        }

                        dataseg_data[current_dataseg_numdata].isaddr = false;
                        dataseg_data[current_dataseg_numdata].data.length = numbytes;
                        dataseg_data[current_dataseg_numdata].datalen = numbytes;
                        dataseg_data[current_dataseg_numdata].addr = "";

                        dataseg_data[current_dataseg_numdata].data[0..$] = 0;

                        current_dataseg_numdata++;
                    }

                    current_dataseg_offset += numbytes;
                }
            }

            continue;
        }

        if (current_line.word == "db")
        {
            if (!input_reading_dataseg)
            {
                write_error("Db outside dataseg");
                return 55;
            }

            if (current_dataseg_isuninitialized)
            {
                write_error("Db inside uninitialized dataseg");
                return 56;
            }


            if (current_line.param1 == "")
            {
                write_error("Db parameter missing");
                return 57;
            }

            if (!is_number(current_line.param1))
            {
                write_error("Db parameter is not a number: " ~ current_line.param1);
                return 58;
            }

            int dbdup = 1;

            if (current_line.param1 != current_line.params)
            {
                string str = current_line.params[current_line.param1.length+1..$].strip();

                long position = str.indexOf(' ');

                if (position >= 0 && str[0..cast(uint)position] == "dup")
                {
                    uint position2 = cast(uint)position;
                    str = str[position2+1..$].strip();

                    if (!is_number(str))
                    {
                        write_error("Db dup parameter is not a number: " ~ str);
                        return 59;
                    }

                    dbdup = to!int(get_number_value(str));

                    if (dbdup < 1)
                    {
                        write_error("Db dup parameter is less than 1: " ~ str);
                        return 60;
                    }
                }
                else
                {
                    write_error("Unknown aditional data after db: " ~ str);
                    return 61;
                }
            }

            ubyte value = to!ubyte(get_number_value(current_line.param1));

            if (current_dataseg_numdata > 0 && !dataseg_data[current_dataseg_numdata - 1].isaddr)
            {
                // extend existing data
                int datalen = dataseg_data[current_dataseg_numdata - 1].datalen;

                if (datalen + dbdup - 1 >= dataseg_data[current_dataseg_numdata - 1].data.length)
                {
                    dataseg_data[current_dataseg_numdata - 1].data.length = dbdup + 1 + 2 * dataseg_data[current_dataseg_numdata - 1].data.length;
                }

                dataseg_data[current_dataseg_numdata - 1].data[datalen..datalen + dbdup] = value;

                dataseg_data[current_dataseg_numdata - 1].datalen += dbdup;
            }
            else
            {
                // add data
                if (current_dataseg_numdata >= dataseg_data.length)
                {
                    dataseg_data.length = 4 + 2 * dataseg_data.length;
                }

                dataseg_data[current_dataseg_numdata].isaddr = false;
                dataseg_data[current_dataseg_numdata].data.length = dbdup;
                dataseg_data[current_dataseg_numdata].datalen = dbdup;
                dataseg_data[current_dataseg_numdata].addr = "";

                dataseg_data[current_dataseg_numdata].data[0..$] = value;

                current_dataseg_numdata++;
            }

            current_dataseg_offset += dbdup;

            continue;
        }

        if (current_line.word == "dinclude")
        {
            if (!input_reading_dataseg)
            {
                write_error("Dinclude outside dataseg");
                return 62;
            }

            if (current_dataseg_isuninitialized)
            {
                write_error("Dinclude inside uninitialized dataseg");
                return 63;
            }


            if (current_line.params == "")
            {
                write_error("Dinclude filename missing");
                return 64;
            }

            string fullpath = null;

            if (!isAbsolute(current_line.params))
            {
                string path = buildPath(input_directory, current_line.params);
                if (exists(path))
                {
                    fullpath = absolutePath(path);
                }
                else
                {
                    for (int i = 0; i < include_directories.length; i++)
                    {
                        if (isAbsolute(include_directories[i]))
                        {
                            path = buildPath(include_directories[i], current_line.params);
                        }
                        else
                        {
                            path = buildPath(input_directory, include_directories[i], current_line.params);
                        }

                        if (exists(path))
                        {
                            fullpath = absolutePath(path);
                            break;
                        }
                    }
                }
            }

            if (fullpath == null)
            {
                if (exists(current_line.params))
                {
                    fullpath = absolutePath(current_line.params);
                }
                else
                {
                    write_error("Dinclude not found: " ~ current_line.params);
                    return 65;
                }
            }

            ubyte[] values;

            File fd;
            fd.open(fullpath);
            values.length = fd.size;
            if (values.length > 0)
            {
                fd.rawRead(values);
            }

            fd.close();

            if (values.length > 0)
            {
                if (current_dataseg_numdata > 0 && !dataseg_data[current_dataseg_numdata - 1].isaddr)
                {
                    // extend existing data
                    int datalen = dataseg_data[current_dataseg_numdata - 1].datalen;

                    if (datalen + values.length - 1 >= dataseg_data[current_dataseg_numdata - 1].data.length)
                    {
                        dataseg_data[current_dataseg_numdata - 1].data.length = values.length + 1 + 2 * dataseg_data[current_dataseg_numdata - 1].data.length;
                    }

                    dataseg_data[current_dataseg_numdata - 1].data[datalen..datalen + values.length] = values;

                    dataseg_data[current_dataseg_numdata - 1].datalen += values.length;
                }
                else
                {
                    // add data
                    if (current_dataseg_numdata >= dataseg_data.length)
                    {
                        dataseg_data.length = 4 + 2 * dataseg_data.length;
                    }

                    dataseg_data[current_dataseg_numdata].isaddr = false;
                    dataseg_data[current_dataseg_numdata].data = values;
                    dataseg_data[current_dataseg_numdata].datalen = to!int(values.length);
                    dataseg_data[current_dataseg_numdata].addr = "";

                    current_dataseg_numdata++;
                }

                current_dataseg_offset += values.length;
            }

            continue;
        }

        if (current_line.word == "daddr")
        {
            if (!input_reading_dataseg)
            {
                write_error("Daddr outside dataseg");
                return 66;
            }

            if (current_dataseg_isuninitialized)
            {
                write_error("Daddr inside uninitialized dataseg");
                return 67;
            }

            if (current_line.param1 == "")
            {
                write_error("Daddr parameter missing");
                return 68;
            }

            if (current_line.param1 != current_line.params)
            {
                write_error("Unknown aditional data after daddr: " ~ current_line.params);
                return 69;
            }

            string param;
            if (current_line.param1 in define_list)
            {
                param = define_list[current_line.param1];
            }
            else
            {
                param = current_line.param1;
            }

            if (current_dataseg_numdata > 0 && !dataseg_data[current_dataseg_numdata - 1].isaddr)
            {
                dataseg_data[current_dataseg_numdata - 1].data.length = dataseg_data[current_dataseg_numdata - 1].datalen;
            }

            if (current_dataseg_numdata >= dataseg_data.length)
            {
                dataseg_data.length = 4 + 2 * dataseg_data.length;
            }

            dataseg_data[current_dataseg_numdata].isaddr = true;
            dataseg_data[current_dataseg_numdata].data.length = 0;
            dataseg_data[current_dataseg_numdata].datalen = 0;
            dataseg_data[current_dataseg_numdata].addr = param.idup;

            current_dataseg_numdata++;

            current_dataseg_offset += 4;

            continue;
        }

        if (current_line.word == "dskip")
        {
            if (!input_reading_dataseg)
            {
                write_error("Dskip outside dataseg");
                return 70;
            }

            if (!current_dataseg_isuninitialized)
            {
                write_error("Dskip inside initialized dataseg");
                return 71;
            }

            if (current_line.param1 == "")
            {
                write_error("Dskip parameter missing");
                return 72;
            }

            if (!is_number(current_line.param1))
            {
                write_error("Dskip parameter is not a number: " ~ current_line.param1);
                return 73;
            }

            if (current_line.param1 != current_line.params)
            {
                write_error("Unknown aditional data after dskip: " ~ current_line.params);
                return 74;
            }

            int value = to!int(get_number_value(current_line.param1));

            if (value < 1)
            {
                write_error("Dskip parameter is less than 1: " ~ current_line.param1);
                return 75;
            }

            if (current_dataseg_numdata > 0)
            {
                // extend existing data
                int datalen = dataseg_data[current_dataseg_numdata - 1].datalen;

                if (datalen + value - 1 >= dataseg_data[current_dataseg_numdata - 1].data.length)
                {
                    dataseg_data[current_dataseg_numdata - 1].data.length = value + 1 + 2 * dataseg_data[current_dataseg_numdata - 1].data.length;
                }

                dataseg_data[current_dataseg_numdata - 1].data[datalen..datalen + value] = 0;

                dataseg_data[current_dataseg_numdata - 1].datalen += value;
            }
            else
            {
                // add data
                if (current_dataseg_numdata >= dataseg_data.length)
                {
                    dataseg_data.length = 4 + 2 * dataseg_data.length;
                }

                dataseg_data[current_dataseg_numdata].isaddr = false;
                dataseg_data[current_dataseg_numdata].data.length = value;
                dataseg_data[current_dataseg_numdata].datalen = value;
                dataseg_data[current_dataseg_numdata].addr = "";

                dataseg_data[current_dataseg_numdata].data[0..$] = 0;

                current_dataseg_numdata++;
            }

            current_dataseg_offset += value;

            continue;
        }

        if (current_line.word == "endd")
        {
            if (!input_reading_dataseg)
            {
                write_error("Endd outside dataseg");
                return 76;
            }

            if (current_line.param1 != current_line.params)
            {
                write_error("Unknown aditional data after endd: " ~ current_line.params);
                return 77;
            }

            if (current_dataseg_offset == 0)
            {
                write_error("Empty dataseg: " ~ current_dataseg);
                return 78;
            }

            if (current_dataseg_numdata > 0 && !dataseg_data[current_dataseg_numdata - 1].isaddr)
            {
                dataseg_data[current_dataseg_numdata - 1].data.length = dataseg_data[current_dataseg_numdata - 1].datalen;
            }

            auto newdataseg = dataseg_list[current_dataseg];

            newdataseg.labels = dataseg_labels[0..current_dataseg_numlabels].dup;
            newdataseg.data = dataseg_data[0..current_dataseg_numdata].dup;

            dataseg_list[current_dataseg] = newdataseg;

            input_reading_dataseg = false;

            continue;
        }


        write_error("Unknown word: " ~ current_line.word);
        return 79;
    }

    if (input_reading_proc)
    {
        stderr.writeln("Missing end of proc");
        return 80;
    }

    if (input_reading_dataseg)
    {
        stderr.writeln("Missing end of datasegment");
        return 81;
    }

    foreach (ref dataseg; dataseg_list)
    {
        foreach (ref data; dataseg.data)
        {
            if (data.isaddr)
            {
                if (data.addr !in proc_list && data.addr !in dlabel_list)
                {
                    stderr.writeln("Daddr not a proc nor a dlabel: " ~ data.addr);
                    return 82;
                }
            }
        }
    }

    if (output_preprocessed_file)
    {
        write_output();
        return 0;
    }

    // second pass

    define_list = define_list.rehash;
    extern_list = extern_list.rehash;
    dlabel_list = dlabel_list.rehash;
    func_list = func_list.rehash;
    proc_list = proc_list.rehash;


    add_output_line("target datalayout = \"e-p:32:32-f64:32:64-n32\"");
    add_output_line("");
    add_output_line("%_cpu = type { i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 }");

    add_output_line("");
    add_output_line("; data segments");
    foreach (dataseg_name; dataseg_list.keys.sort())
    {
        auto dataseg = dataseg_list[dataseg_name];

        string dataseg_type = "";
        string dataseg_values = "";
        for (int i = 0; i < dataseg.data.length; i++)
        {
            if (i > 0)
            {
                dataseg_type ~= ", ";
                dataseg_values ~= ", ";
            }
            if (dataseg.data[i].isaddr)
            {
                dataseg_type ~= "i32";
                dataseg_values ~= "i32 ";
                if (dataseg.data[i].addr in proc_list)
                {
                    dataseg_values ~= "ptrtoint (" ~ ((no_tail_calls)?"i8*":"void") ~ "(%_cpu*)* @" ~ dataseg.data[i].addr ~ " to i32)";
                }
                else
                {
                    if (position_independent_code)
                    {
                        auto addrlabel = dlabel_list[dataseg.data[i].addr];

                        dataseg_values ~= "ptrtoint (i8* getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ addrlabel.dataseg_name ~ "* @" ~ addrlabel.dataseg_name ~ " to i8*), i32 " ~ to!string(addrlabel.offset) ~ ") to i32)";
                    }
                    else
                    {
                        dataseg_values ~= "ptrtoint (i8* @" ~ dataseg.data[i].addr ~ " to i32)";
                    }
                }
            }
            else
            {
                dataseg_type ~= "[" ~ to!string(dataseg.data[i].datalen) ~ " x i8]";
                dataseg_values ~= "[" ~ to!string(dataseg.data[i].datalen) ~ " x i8]";
                if (dataseg.isuninitialized)
                {
                    dataseg_values ~= " undef";
                }
                else
                {
                    dataseg_values ~= " c\"";
                    foreach(value; dataseg.data[i].data)
                    {
                        int value_hi = (value >> 4) & 0x0f;
                        int value_lo = value & 0x0f;

                        dataseg_values ~= "\\" ~ "0123456789ABCDEF"[value_hi..value_hi+1] ~ "0123456789ABCDEF"[value_lo..value_lo+1];
                    }
                    dataseg_values ~= "\"";
                }
            }
        }

        add_output_line("%_" ~ dataseg_name ~ " = type <{ " ~ dataseg_type ~ " }>");
        add_output_line("@" ~ dataseg_name ~ " = private " ~ (dataseg.isconstant?"constant":"global") ~ " %_" ~ dataseg_name ~ " <{ " ~ dataseg_values ~ " }>, align 4");

        foreach (label; dataseg.labels)
        {
            if (!position_independent_code || label.isglobal)
            {
                add_output_line("@" ~ label.name ~ " = " ~ (label.isglobal?"":"private ") ~ "alias " ~ get_load_type("i8") ~ " getelementptr (" ~ get_load_type("i8") ~ " bitcast (%_" ~ dataseg_name ~ "* @" ~ dataseg_name ~ " to i8*), i32 " ~ to!string(label.offset) ~ ")");
            }
        }
    }


    add_output_line("");
    add_output_line("; external variables");
    foreach (extern_name; extern_list.keys.sort())
    {
        add_output_line("@" ~ extern_name ~ " = external global i8");
    }

    add_output_line("");
    add_output_line("; external functions");
    foreach (func_name; func_list.keys.sort())
    {
        auto func = func_list[func_name];

        string func_args = "";
        for (int i = 0; i < func.params.length; i++)
        {
            string datatype;
            switch (func.params[i].type)
            {
                case ParameterType.Cpu:
                    datatype = "%_cpu*";
                    break;
                case ParameterType.Pointer:
                    datatype = "i8*";
                    break;
                default:
                    datatype = "i32";
                    break;
            }

            if (i > 0)
            {
                func_args ~= ",";
            }
            func_args ~= datatype;
        }

        add_output_line("declare external ccc " ~ (func.has_return_value?"i32":"void") ~ " @" ~ func_name ~ "(" ~ func_args ~ ") nounwind");
    }


    add_output_line("");
    add_output_line("; external procedures");
    foreach (proc_name; proc_list.keys.sort())
    {
        auto proc = proc_list[proc_name];

        if (!proc.isexternal) continue;

        add_output_line("declare external fastcc " ~ ((no_tail_calls)?"i8*":"void") ~ " @" ~ proc_name ~ "(%_cpu*) nounwind");
        if (proc.alias_name != "")
        {
            //add_output_line("@" ~ proc.alias_name ~ " = external alias void(%_cpu*)* @" ~ proc_name);
        }
    }

    add_output_line("");
    add_output_line("; intrinsics");
    if (used_ctlz_intrinsics)
    {
        add_output_line("declare i32 @llvm.ctlz.i32(i32, i1)");
    }
    if (used_bswap_intrinsics)
    {
        add_output_line("declare i32 @llvm.bswap.i32(i32)");
    }

    add_output_line("");
    add_output_line("; procedures");
    foreach (proc_name; local_proc_names)
    {
        auto proc = proc_list[proc_name];

        add_output_line("define " ~ ((proc.ispublic || proc.isglobal)?"":"private ") ~ "fastcc " ~ ((no_tail_calls)?"i8*":"void") ~ " @" ~ proc_name ~ "(%_cpu* %cpu) nounwind {");

        if (!process_proc_body(proc_name))
        {
            return 79;
        }

        add_output_line("}");

        if (proc.alias_name != "")
        {
            //add_output_line("@" ~ proc.alias_name ~ " = alias void(%_cpu* %cpu)* @" ~ proc_name);
        }

        if (proc.isglobal)
        {
            if (return_procedure == "")
            {
                return_procedure = "__return_procedure";
                if (no_tail_calls)
                {
                    add_output_line("define private fastcc i8* @" ~ return_procedure ~ "(%_cpu* %cpu) nounwind {");
                    add_output_line("ret i8* null");
                }
                else
                {
                    add_output_line("define private fastcc void @" ~ return_procedure ~ "(%_cpu* %cpu) nounwind {");
                    add_output_line("ret void");
                }
                add_output_line("}");
            }

            if (no_tail_calls && dispatcher_procedure == "")
            {
                dispatcher_procedure = "__dispatcher_procedure";
                add_output_line("define private fastcc void @" ~ dispatcher_procedure ~ "(%_cpu* %cpu, i8*(%_cpu*)* %proc) nounwind {");

                add_output_line("%tproc = alloca i8*(%_cpu*)*, align 4");

                add_output_line("%1 = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ register_numbers_str["esp"]);
                add_output_line("%2 = load " ~ get_load_type("i32") ~ " %1, align 4");
                add_output_line("%3 = sub i32 %2, 4");
                add_output_line("store i32 %3, i32* %1, align 4");

                add_output_line("%4 = inttoptr i32 %3 to i32*");
                add_output_line("%5 = ptrtoint " ~ ((no_tail_calls)?"i8*":"void") ~ "(%_cpu*)* @" ~ return_procedure ~ " to i32");
                add_output_line("store i32 %5, i32* %4, align 4");

                add_output_line("store i8*(%_cpu*)* %proc, i8*(%_cpu*)** %tproc, align 4");
                add_output_line("br label %l_1");

                add_output_line("l_1:");
                add_output_line("%6 = load " ~ get_load_type("i8*(%_cpu*)*") ~ " %tproc, align 4");
                add_output_line("%7 = call fastcc i8* %6(%_cpu* %cpu) nounwind");
                add_output_line("%8 = bitcast i8* %7 to i8*(%_cpu*)*");
                add_output_line("store i8*(%_cpu*)* %8, i8*(%_cpu*)** %tproc, align 4");
                add_output_line("%9 = icmp eq i8* %7, null");
                add_output_line("br i1 %9, label %l_2, label %l_1");
                add_output_line("l_2:");

                add_output_line("ret void");
                add_output_line("}");
            }

            add_output_line("define ccc void @" ~ proc.global_name ~ "(%_cpu* %cpu) nounwind {");

            if (no_tail_calls)
            {
                add_output_line("tail call fastcc void @" ~ dispatcher_procedure ~ "(%_cpu* %cpu, i8*(%_cpu*)* @" ~ proc_name ~ ") nounwind");
            }
            else
            {
                add_output_line("%1 = getelementptr " ~ get_load_type("%_cpu") ~ " %cpu, i32 0, i32 " ~ register_numbers_str["esp"]);
                add_output_line("%2 = load " ~ get_load_type("i32") ~ " %1, align 4");
                add_output_line("%3 = sub i32 %2, 4");
                add_output_line("store i32 %3, i32* %1, align 4");

                add_output_line("%4 = inttoptr i32 %3 to i32*");
                add_output_line("%5 = ptrtoint " ~ ((no_tail_calls)?"i8*":"void") ~ "(%_cpu*)* @" ~ return_procedure ~ " to i32");
                add_output_line("store i32 %5, i32* %4, align 4");

                add_output_line("call fastcc void @" ~ proc_name ~ "(%_cpu* %cpu) nounwind");
            }

            add_output_line("ret void");
            add_output_line("}");
        }
    }

    write_output();
    return 0;
}

