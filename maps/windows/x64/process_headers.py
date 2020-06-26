#!/usr/bin/python3
import datetime
import os
import argparse
import json
import re
#from itanium_demangler import parse as demangle

parser = argparse.ArgumentParser(description='Processes headers')
parser.add_argument('-a', '--arch', type=str, help='arch', choices=['x86', 'x64'], required=True)
parser.add_argument('-p', '--platform', type=str, help='platform', choices=['windows'], required=True)
parser.add_argument('input', metavar='file', type=str, nargs='+', help='input json symbol maps')
args = parser.parse_args()
out_file_cpp = open("main.cpp", "w")
out_file_header = open("main.h", "w")
out_file_asm = open("init.asm", "w")
arch = args.arch
platform = args.platform
in_files = args.input

symbol_list = []
symbol_list_versioned = []
vtable_list = []
vtable_output = []
var_list = []
var_list_versioned = []
include_list = []
cxx_output = ""
asm_output = ""

def output_cpp(text):
    text += "\n"
    global cxx_output
    cxx_output += "CXX: " + text
    if out_file_cpp is not None:
        out_file_cpp.write(text)

def output_header_cpp(text):
    text += "\n"
    global cxx_output
    cxx_output += "CXX: " + text
    if out_file_header is not None:
        out_file_header.write(text)

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
                vtable_list.append({"name": obj["name"], "parent": obj.get("parent", ""), "address": obj.get("address", ""), "functions": obj.get("functions", []), "overload": obj.get("overload", "null")})
        if key == "functions":
            for obj in reader[key]:
                symbol_list.append({"mangled_name": obj["name"], "address": obj["address"], "signature": obj.get("signature", ""), "overload": obj.get("overload", "null")})
                if type(obj["address"]) == dict:
                    symbol_list_versioned.append({"mangled_name": obj["name"], "address": obj["address"], "signature": obj.get("signature", ""), "overload": obj.get("overload", "null")})
        if key == "variables":
            for variable_keys in reader[key].keys():
                var_list.append({"name": variable_keys, "address": reader[key][variable_keys]})
                if type(reader[key][variable_keys] == dict):
                    var_list_versioned.append({"name": variable_keys, "address": reader[key][variable_keys]}) 
        if key == "includes":
            for strs in reader[key]:
                include_list.append(strs)


def generate_windows_cpp():
    output_cpp("")
    output_cpp("#include <Windows.h>")
    output_cpp("")
    output_cpp("BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {")
    output_cpp("\tif(fdwReason == DLL_PROCESS_ATTACH) InitBedrockPointers();")
    output_cpp("\treturn TRUE;")
    output_cpp("}")

def mangled_name_to_variable(str):
    str = str.replace("?", '_')
    str = str.replace("@", '_')
    return str

def generate_init_cpp():
    output_header_cpp("")
    output_header_cpp("#pragma once")
    output_header_cpp("")
    output_header_cpp("#include <Zenova/Hook.h>")
    if len(symbol_list) > 0 or len(vtable_list) > 0:
        output_header_cpp("")
        output_header_cpp("extern \"C\" {")
        for a in symbol_list:
            output_header_cpp("\textern void* " + mangled_name_to_variable(a["mangled_name"]) + "_ptr;")
        for a in vtable_list:
            output_header_cpp("\textern void* " + a["name"] + "_vtable;")
        output_header_cpp("}")
        output_header_cpp("")
        output_header_cpp("void InitBedrockPointers();")
        if len(symbol_list_versioned) > 0:
            output_header_cpp("void InitVersionPointers(std::string);")
    output_cpp("")
    output_cpp("#include \"main.h\"")
    for a in include_list:
        output_cpp(a)
    output_cpp("")
    for a in var_list:
        address = a["address"]
        if address and type(address) == str:
            address = " = reinterpret_cast<" + a["name"][:a["name"].rfind("*")+1] + ">(Zenova::Hook::SlideAddress(" + address + "))"
        else:
            address = ""
        output_cpp(a["name"] + address + ";")
    output_cpp("")
    output_cpp("extern \"C\" {")
    for a in symbol_list:
        output_cpp("\tvoid* " + mangled_name_to_variable(a["mangled_name"]) + "_ptr;")
    for a in vtable_list:
        output_cpp("\tvoid* " + a["name"] + "_vtable;")
    output_cpp("}")
    output_cpp("")
    if len(symbol_list_versioned) > 0:
        output_cpp("static std::unordered_map<std::string, size_t> pointerList[" + str(len(symbol_list_versioned) + len(var_list_versioned)) + "];")
        output_cpp("")
    if len(symbol_list) > 0 or len(vtable_list) > 0 or len(var_list) > 0:
        output_cpp("void InitBedrockPointers() {")
        for a in var_list:
            if a["address"] == "":
                loc = a["name"].rfind("*") + 1
                while a["name"][loc] == " ":
                    loc += 1
                output_cpp("\t" + a["name"][loc:] + " = reinterpret_cast<" + a["name"][:-a["name"].rfind("*")] + ">(Zenova::Hook::FindVariable(\"" + a["name"] + "\"));")
        pointer_index = 0
        for a in symbol_list:
            if type(a["address"]) == dict:
                for version, address in a["address"].items():
                    output_cpp("\tpointerList[" + str(pointer_index) + "][\"" + str(version) + "\"] = " + str(address) + ";")
                pointer_index += 1
            elif type(a["address"]) == str:
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
        for var in var_list_versioned:
            for version, address in var["address"].items():
                output_cpp("\tpointerList[" + str(pointer_index) + "][\"" + str(version) + "\"] = " + str(address) + ";")
            pointer_index += 1
        output_cpp("}")
    if len(symbol_list_versioned) > 0 or len(var_list_versioned) > 0:
        output_cpp("")
        output_cpp("void InitVersionPointers(std::string versionId) {")
        pointer_index = 0
        for a in symbol_list_versioned:
            name_legal = mangled_name_to_variable(a["mangled_name"])
            output_cpp("\t" + name_legal + "_ptr = reinterpret_cast<void*>(Zenova::Hook::SlideAddress(pointerList[" + str(pointer_index) + "][versionId]));")
            pointer_index += 1
        for var in var_list_versioned:
            address = " = reinterpret_cast<" + var["name"][:var["name"].rfind("*")+1] + ">(Zenova::Hook::SlideAddress(pointerList[" + str(pointer_index) + "][versionId]))"
            output_cpp("\t" + var["name"][var["name"].rfind("*")+1:].strip() + address + ";")
            pointer_index += 1
        output_cpp("}")



def process_vtable(vtable):
    vtable_out = next((x for x in vtable_output if vtable["name"] == x["name"]), {})
    if not vtable_out:
        vtable_out["name"] = vtable["name"]
        vtable_out["functions_out"] = []
        vtable_out["functions_in"] = []
        vtable_out["parents"] = []
        i = 0
        vtable_parent_str = vtable["parent"]
        vtable_parent_out = {}
        if vtable_parent_str:
            vtable_parent = next((x for x in vtable_list if x["name"] == vtable_parent_str), {})
            if vtable_parent:
                vtable_parent_out = process_vtable(vtable_parent)
                vtable_out["parents"].append(vtable_parent_out)
                vtable_out["parents"].extend(vtable_parent_out["parents"])
        for a in vtable["functions"]:
            func_name_base = a.replace(vtable["name"] + "@@", "@@", 1)
            if len(vtable_out["parents"]) > 0:
                for parent in vtable_out["parents"]:
                    func_name_base = re.sub('(@@@.*)' + parent["name"] + '@@', r'\g<1>1@', func_name_base)
                for b in vtable_parent_out["functions_in"][i:]:
                    if func_name_base == b[0]:
                        break
                    vtable_out["functions_in"].append([b[0], i])
                    i += 1
            vtable_out["functions_in"].append([func_name_base, i])
            if a:
                vtable_out["functions_out"].append([a, i])
            i += 1
    vtable_output.append(vtable_out)
    return vtable_out
            
        

#NASM, MASM doesn't allow long identifiers
def generate_init_func_x86(size):
    if size == 64:
        reg = "rax"
        pointer_size = 8
        output_asm("bits 64")
    if size == 32:
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
    for vtable in vtable_list:
        vtable_out = process_vtable(vtable)
        print(vtable_out["name"] + "\n")
        for a in vtable_out["functions_out"]:
            output_asm("global " + a[0])
            output_asm(a[0] + ":")
            output_asm("\tmov " + reg + ", [rel " + vtable_out["name"] + "_vtable]")
            output_asm("\tjmp [" + reg + "+" + str(a[1] * pointer_size) + "]")

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
    output_header_cpp("// This file was automatically generated using tools/process_csv.py")
    output_header_cpp("// Generated on " + gen_time)
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
out_file_cpp.close()
out_file_asm.close()