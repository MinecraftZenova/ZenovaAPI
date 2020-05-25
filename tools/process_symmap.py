#!/usr/bin/python3
import datetime
import os
import argparse
import json
#from itanium_demangler import parse as demangle

parser = argparse.ArgumentParser(description='Processes headers')
parser.add_argument('-a', '--arch', type=str, help='arch', choices=['x86', 'x64'], required=True)
parser.add_argument('-p', '--platform', type=str, help='platform', choices=['windows'], required=True)
parser.add_argument('input', metavar='file', type=str, nargs='+', help='input json symbol maps')
args = parser.parse_args()
out_file_cpp = open("../src/initcpp.cpp", "w")
out_file_asm = open("../src/initasm.asm", "w")
arch = args.arch
platform = args.platform
in_files = args.input

symbol_list = []
vtable_list = []
var_list = []
include_list = []
cxx_output = ""
asm_output = ""

def output_cpp(text):
    text += "\n"
    global cxx_output
    cxx_output += "CXX: " + text
    if out_file_cpp is not None:
        out_file_cpp.write(text)

def output_asm(text):
    text += "\n"
    global asm_output
    asm_output += "ASM: " + text
    if out_file_asm is not None:
        out_file_asm.write(text)

def read_json(file):
    reader = json.load(file)
    for key in reader:
        if key == "vtable":
            for obj in reader[key]:
                vtable_list.append({"name": obj["name"], "address": obj.get("address", ""), "functions": obj.get("functions", []), "overload": obj.get("overload", "null")})
        if key == "functions":
            for obj in reader[key]:
                symbol_list.append({"mangled_name": obj["name"], "address": obj.get("address", ""), "signature": obj.get("signature", ""), "overload": obj.get("overload", "null")})
        if key == "variables":
            for variable_keys in reader[key].keys():
                var_list.append({"name": variable_keys, "address": reader[key][variable_keys]})
        if key == "includes":
            for strs in reader[key]:
                include_list.append(strs)

def mangled_name_to_variable(str):
    str = str.replace("?", '_')
    str = str.replace("@", '_')
    return str

def generate_init_cpp():
    output_cpp("")
    output_cpp("#include <Zenova/Hook.h>")
    for a in include_list:
        output_cpp(a)
    output_cpp("")
    for a in var_list:
        address = a["address"]
        if address != "":
            address = " = reinterpret_cast<" + a["name"][:-a["name"].rfind("*")] + ">(Zenova::Hook::SlideAddress(" + address + "))"
        output_cpp(a["name"] + address + ";")
    output_cpp("")
    output_cpp("extern \"C\" {")
    for a in symbol_list:
        output_cpp("\tvoid* " + mangled_name_to_variable(a["mangled_name"]) + "_ptr;")
    for a in vtable_list:
        output_cpp("\tvoid* " + a["name"] + "_vtable;")
    output_cpp("}")
    output_cpp("")
    output_cpp("void InitBedrockPointers() {")
    for a in var_list:
        if a["address"] == "":
            loc = a["name"].rfind("*") + 1
            while a["name"][loc] == " ":
                loc += 1
            output_cpp("\t" + a["name"][loc:] + " = reinterpret_cast<" + a["name"][:a["name"].rfind("*")] + ">(Zenova::Hook::FindVariable(\"" + a["name"] + "\"));")
    for a in symbol_list:
        name_legal = mangled_name_to_variable(a["mangled_name"])
        if a["address"] != "":
            output_cpp("\t" + name_legal + "_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(" + a["address"] + "));")
        if a["overload"] == "always" or (a["overload"] == "null" and a["address"] == ""):
            output_cpp("\t" + name_legal + "_ptr = reinterpret_cast<void*>(Zenova::Hook::FindSymbol(\"" + a["mangled_name"] + "\"));")
    for a in vtable_list:
        if a["address"] != "":
            output_cpp("\t" + a["name"] + "_vtable = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(" + a["address"] + "));")
        if a["overload"] == "always" or (a["overload"] == "null" and a["address"] == ""):
            output_cpp("\t" + name_legal + "_ptr = reinterpret_cast<void*>(Zenova::Hook::FindVtable(\"" + a["mangled_name"] + "\"));")
    output_cpp("}")

#NASM, MASM doesn't allow long identifiers
def generate_init_func_x86(size):
    if size == 64:
        word_name = "qword"
        reg = "rax"
        pointer_size = 8
        output_asm("bits 64")
    if size == 32:
        word_name = "dword"
        reg = "eax"
        pointer_size = 4

    output_asm("SECTION .data")
    for a in symbol_list:
        output_asm("extern " + mangled_name_to_variable(a["mangled_name"]) + "_ptr")
    for a in vtable_list:
        output_asm("extern " + a["name"] + "_vtable")
    output_asm("")
    output_asm("SECTION .text")
    for a in symbol_list:
        output_asm("global " + a["mangled_name"])
        output_asm(a["mangled_name"] + ":")
        output_asm("\tmov rax, [rel " + mangled_name_to_variable(a["mangled_name"]) + "_ptr" + "]")
        output_asm("\tjmp rax")
    i = 0
    for vtable in vtable_list:
        for a in vtable["functions"]:
            output_asm("global " + a)
            output_asm(a + ":")
            output_asm("\tmov " + reg + ", [rel " + vtable["name"] + "_vtable]")
            output_asm("\tjmp [" + reg + "+" + str(i * pointer_size) + "]")
            i += 1

def generate_init_func_arm():
    i = 0
    for a in symbol_list:
        output_asm(".globl  " + a["mangled_name"] + "")
        output_asm(".type   " + a["mangled_name"] + ", %function")
        output_asm(a["mangled_name"] + ":")
        output_asm(f"\tldr    r12, .L{i}$ptr")
        output_asm(f".L{i}$pldr:")
        output_asm(f"\tldr    r12, [pc, r12]")
        output_asm(f"\tldr    r12, [r12]")
        output_asm(f"\tbx	    r12")
        output_asm(f".L{i}$ptr:")
        output_asm(f"\t.long   {a['mangled_name']}_ptr(GOT_PREL)-((.L{i}$pldr+8)-.L{i}$ptr)")
        output_asm("\t.size   " + a["mangled_name"] + ", .-" + a["mangled_name"])
        i += 1

def generate_init_func():
    gen_time = datetime.datetime.utcnow().strftime("%a %b %d %Y %H:%M:%S UTC")
    output_cpp("// This file was automatically generated using tools/process_csv.py")
    output_cpp("// Generated on " + gen_time)
    generate_init_cpp()
    generate_windows_cpp()

    output_asm("; This file was automatically generated using tools/process_csv.py")
    output_asm("; Generated on " + gen_time)
    if arch == "x86":
        generate_init_func_x86(32)
    if arch == "x64":
        generate_init_func_x86(64)
    if arch == "armeabi-v7a":
        print(arch + " not supported")
        #generate_init_func_arm()

for file_path in in_files:
    print("Parsing Symbol Map:" + file_path)
    file_full_path = os.path.abspath(file_path)
    with open(file_path, "r") as f:
        read_json(f)
generate_init_func()
print(cxx_output)
print(asm_output)
out_file_cpp.close()
out_file_asm.close()