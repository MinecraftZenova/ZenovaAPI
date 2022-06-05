#!/usr/bin/python3
import datetime
import os
import argparse
import json
import re
from glob import glob 
from pathlib import Path
#from itanium_demangler import parse as demangle

parser = argparse.ArgumentParser(description='Processes symbol maps')
parser.add_argument('-a', '--arch', type=str, help='arch', choices=['x86', 'x64'], required=True)
parser.add_argument('-p', '--platform', type=str, help='platform', choices=['windows'], required=True)
parser.add_argument('-d', '--directory', type=Path, help='directory', default=".")
parser.add_argument('-o', '--output', help='log debug output', action='store_true')
parser.add_argument('-e', '--entry', help='create DllMain entry point', action='store_true')
parser.add_argument('input', metavar='file', type=str, nargs='+', help='input json symbol maps')
args = parser.parse_args()

arch = args.arch
platform = args.platform
in_files = args.input
directory = str(args.directory)
debug_output = args.output
entry = args.entry

if not os.path.exists(directory):
    os.makedirs(directory)

file_header_name = "initcpp.h"

out_file_cpp = None
out_file_header = None
out_file_asm = None

version_list = []
symbol_list = []
vtable_list = []
vdtor_list = []
var_list = []
var_dict = {"": []}
include_list = []
vtable_output = []
cxx_output = ""
hxx_output = ""
asm_output = ""

def output_cxx(text):
    text += "\n"
    global cxx_output
    cxx_output += "CXX: " + text
    if out_file_cpp is not None:
        out_file_cpp.write(text)

def output_header(text):
    text += "\n"
    global hxx_output
    hxx_output += "HXX: " + text
    if out_file_header is not None:
        out_file_header.write(text)

def output_asm(text):
    text += "\n"
    global asm_output
    asm_output += "ASM: " + text
    if out_file_asm is not None:
        out_file_asm.write(text)

def signature_helper(psig):
    if(psig != ""):
        sig = ""
        mask = ""

        byteList = psig.split(" ")

        for byte in byteList:
            if(byte == "?"):
                mask += "?"
                sig += "\\x00"
            else:
                mask += "x"
                sig += "\\x" + byte

        sigtype = "SigscanCall" if(byteList[0] == "E8" or byteList[0] == "E9") else "Sigscan"
        
        return { "sig": sig, "mask": mask, "type": sigtype }

    return {}

def read_json(file):
    reader = json.load(file)
    for key in reader:
        if key == "version":
            for ver in reader[key]:
                version_list.append(ver)
                var_dict[ver] = []

        elif key == "vtable":
            for obj in reader[key]:
                vtable_list.append({
                    "name": obj["name"], 
                    "parent": obj.get("parent", ""), 
                    "address": obj.get("address", ""), 
                    "functions": obj.get("functions", []), 
                    "overload": obj.get("overload", "null")
                })

                offset = 0
                for dtor in vtable_list[-1]["functions"]:
                    if dtor[:3] == "??1":
                        vdtor_list.append({
                            "vtable": vtable_list[-1]["name"],
                            "name": mangled_name_to_variable(dtor),
                            "mangled_name": dtor,
                            "address": '0x{:X}'.format(int(vtable_list[-1]["address"], 16) + (offset * 8))
                        })
                        vtable_list[-1]["functions"][offset] = ""
                    offset += 1

        elif key == "functions":
            for obj in reader[key]:
                name = obj["name"]

                symbol_list.append({
                    "name": mangled_name_to_variable(name),
                    "mangled_name": name,
                    "address": obj.get("address", ""),
                    "signature": signature_helper(obj.get("signature", "")),
                    "overload": obj.get("overload", "null")
                })
        elif key == "variables":
            var_count = 0
            for var_name in reader[key].keys():
                var_list.append(var_name)
                addr = reader[key][var_name]
                if type(addr) is str:
                    var_dict[""].append({
                        "offset": var_count,
                        "address": addr
                    })
                elif type(addr) is list:
                    i = 0
                    # check to see if ver == "" (needs to be disallowed)
                    for ver in version_list:
                        var_dict[ver].append({
                            "offset": var_count,
                            "address": addr[i]
                        })
                        i += 1
                elif type(addr) is dict:
                    for ver in addr:
                        var_dict[ver] = var_dict.get(key, []) + [{
                            "offset": var_count,
                            "address": addr[ver]
                        }]
                var_count += 1

        elif key == "includes":
            for strs in reader[key]:
                include_list.append(strs)

def mangled_name_to_variable(str):
    str = str.replace("?", '_')
    str = str.replace("@", '_')
    return str

def generate_init_cpp():
    #flags


    #*.hxx
    output_header("")
    output_header("#pragma once")
    if len(symbol_list) > 0 or len(vtable_list) > 0:
        output_header("")
        output_header("extern \"C\" {")
        for a in symbol_list:
            if a["name"]:
                output_header("\textern void* " + a["name"] + "_ptr;")
        for a in vtable_list:
            if a["name"]:
                output_header("\textern void* " + a["name"] + "_vtable;")
        for a in vdtor_list:
            if a["name"]:
                output_header("\textern void* " + a["name"] + "_ptr;")
        output_header("}")
    output_header("")
    output_header("void InitBedrockPointers();")
    output_header("void InitVersionPointers();")

    #*.cxx
    output_cxx("")
    output_cxx("#include <array>")
    output_cxx("#include <Zenova/Hook.h>")
    output_cxx("#include <Zenova/Minecraft.h>")
    output_cxx("")
    output_cxx("#include \"" + file_header_name + "\"")
    for include in include_list:
        output_cxx("#include \"" + include + "\"")
    output_cxx("")

    output_cxx("using namespace Zenova::Hook;")
    output_cxx("")

    # use lambdas to initialize the global variables (allows for "const" initialization)
    if var_list:
        var_arr_str = "std::array<uintptr_t, " + str(len(var_list)) + ">"
        output_cxx("namespace {")
        output_cxx("static " + var_arr_str + " var_addrs = []() -> " + var_arr_str + " {")
        output_cxx("\tconst Zenova::Version& versionId = Zenova::Minecraft::version();")
        output_cxx("\t" + var_arr_str + " vars{};") # should this be allocated on the heap?
        output_cxx("")

        # reimplement FindVariable when it's a more stable concept
        for var in var_dict[""]:
            offset, addr = var.values()
            output_cxx("\tvars[" + str(offset) + "] = SlideAddress(" + str(addr) + ");")

        output_cxx("")
        prefix = ""
        for ver in var_dict:
            if ver != "":
                output_cxx("\t" + prefix + "if (versionId == \"" + ver + "\") {")
                for var in var_dict[ver]:
                    offset, addr = var.values()
                    output_cxx("\t\tvars[" + str(offset) + "] = SlideAddress(" + str(addr) + ");")
                output_cxx("\t}")
                prefix = "else "

        if prefix == "else ":
            output_cxx("")

        output_cxx("\treturn vars;")
        output_cxx("}();")
        output_cxx("}")
        output_cxx("")

        i = 0
        for name in var_list:
            if name:
                output_cxx(name + " = *reinterpret_cast<" + name[:name.rfind("&")] + "*>(var_addrs[" + str(i) + "])" + ";")
            i += 1
        output_cxx("")

    for a in symbol_list:
        if a["name"]:
            output_cxx("void* " + a["name"] + "_ptr;")
    for a in vtable_list:
        if a["name"]:
            output_cxx("void* " + a["name"] + "_vtable;")
    for a in vdtor_list:
        if a["name"]:
            output_cxx("void* " + a["name"] + "_ptr;")
    output_cxx("")

    output_cxx("void InitBedrockPointers() {")
    for a in symbol_list:
        address = a["address"]
        if type(address) == str and address != "":
            output_cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(SlideAddress(" + address + "));")
            
        signature = a["signature"]
        if type(signature) == dict and len(signature) == 3: #signature_helper
            output_cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(" + signature["type"] + "(\"" + signature["sig"] + "\", \"" + signature["mask"] + "\"));")

    for a in vtable_list:
        name = a["name"]
        address = a["address"]

        if type(address) == str and address != "":
            output_cxx("\t" + name + "_vtable = reinterpret_cast<void*>(SlideAddress(" + address + "));")
        if a["overload"] == "always" or (a["overload"] == "null" and address == ""):
            output_cxx("\t" + name + "_vtable = reinterpret_cast<void*>(FindVtable(\"" + name + "\"));")

    for a in vdtor_list:
        output_cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(GetRealDtor(SlideAddress(" + a["address"] + ")));")
    output_cxx("}")
    output_cxx("")

    output_cxx("void InitVersionPointers() {")
    output_cxx("\tconst Zenova::Version& versionId = Zenova::Minecraft::version();")

    #I want to come back and add support for version based signatures

    cxx_dict_list = [{}, {}]
    for vtable in vtable_list:
        address_list = vtable["address"]
        if(type(address_list) == list):
            i = 0
            for address in address_list:
                if(address != ""):
                    cxx_str = "\t\t" + vtable["name"] + "_vtable = reinterpret_cast<void*>(SlideAddress(" + address_list[i] + "));"
                    if(version_list[i] not in cxx_dict_list[0]):
                        cxx_dict_list[0][version_list[i]] = [ cxx_str ]
                    else:
                        cxx_dict_list[0][version_list[i]].append(cxx_str)
                i += 1

    for sym in symbol_list:
        address_list = sym["address"]
        if(type(address_list) == list):
            i = 0
            for address in address_list:
                if(address != ""):
                    cxx_str = "\t\t" + sym["name"] + "_ptr = reinterpret_cast<void*>(SlideAddress(" + address_list[i] + "));"
                    if(version_list[i] not in cxx_dict_list[1]):
                        cxx_dict_list[1][version_list[i]] = [ cxx_str ]
                    else:
                        cxx_dict_list[1][version_list[i]].append(cxx_str)
                i += 1

    if cxx_dict_list[0] or cxx_dict_list[1]:
        prefix = ""
        for version in version_list:
            output_cxx("\t" + prefix + "if(versionId == \"" + version + "\") {")
            for cxx_dict in cxx_dict_list:
                for cxx in cxx_dict.get(version, []):
                    output_cxx(cxx)
            output_cxx("\t}")
            prefix = "else "

    output_cxx("}")

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
def generate_init_func_x86(bit):
    if bit == 64:
        reg = "rax"
        pointer_size = 8
        output_asm("bits 64")
    if bit == 32:
        reg = "eax"
        pointer_size = 4

    output_asm("SECTION .data")
    for a in symbol_list:
        output_asm("extern " + a["name"] + "_ptr")
    for a in vdtor_list:
        output_asm("extern " + a["name"] + "_ptr")
    for a in vtable_list:
        output_asm("extern " + a["name"] + "_vtable")
    output_asm("")
    output_asm("SECTION .text")
    for a in symbol_list:
        output_asm("global " + a["mangled_name"])
        output_asm(a["mangled_name"] + ":")
        output_asm("\tjmp qword [rel " + mangled_name_to_variable(a["mangled_name"]) + "_ptr" + "]")
    for a in vdtor_list:
        output_asm("global " + a["mangled_name"])
        output_asm(a["mangled_name"] + ":")
        output_asm("\tjmp qword [rel " + a["name"] + "_ptr]")
    for vtable in vtable_list:
        vtable_out = process_vtable(vtable)
        for a in vtable_out["functions_out"]:
            output_asm("global " + a[0])
            output_asm(a[0] + ":")
            output_asm("\tmov " + reg + ", [rel " + vtable_out["name"] + "_vtable]")
            output_asm("\tjmp [" + reg + "+" + str(a[1] * pointer_size) + "]")



def generate_init_windows():
    output_cxx("")
    output_cxx("BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {")
    output_cxx("\tif(fdwReason == DLL_PROCESS_ATTACH) {")
    output_cxx("\t\tInitBedrockPointers();")
    output_cxx("\t\tInitVersionPointers();")
    output_cxx("\t}")
    output_cxx("\treturn TRUE;")
    output_cxx("}")



def generate_init_func():
    gen_time = datetime.datetime.utcnow().strftime("%a %b %d %Y %H:%M:%S UTC")
    output_cxx("// This file was automatically generated using tools/" + os.path.basename(__file__))
    output_cxx("// " + gen_time)

    output_header("// This file was automatically generated using tools/" + os.path.basename(__file__))
    output_header("// " + gen_time)

    generate_init_cpp()

    if platform == "windows" and entry:
        generate_init_windows()

    output_asm("; This file was automatically generated using tools/" + os.path.basename(__file__))
    output_asm("; " + gen_time)

    if arch == "x86":
        generate_init_func_x86(32)
    if arch == "x64":
        generate_init_func_x86(64)
    if arch == "armeabi-v7a":
        print(arch + " not supported")
        #generate_init_func_arm()



def rebuild_symbol_gen():
    global out_file_cpp, out_file_header, out_file_asm
    out_file_cpp = open(directory + "/initcpp.cpp", "w")
    out_file_header = open(directory + "/" + file_header_name, "w")
    out_file_asm = open(directory + "/initasm.asm", "w")

    if(debug_output):
        print("cxx output: " + out_file_cpp.name)
        print("hxx output: " + out_file_header.name)
        print("asm output: " + out_file_asm.name)

    for file_path in in_files:
        for glob_file_path in glob(file_path):
            file_full_path = os.path.abspath(glob_file_path)
            print("Parsing Symbol Map: " + file_full_path)
            with open(file_full_path, "r") as f:
                read_json(f)

    generate_init_func()
    
    if(debug_output):
        print(cxx_output)
        print(hxx_output)
        print(asm_output)

    out_file_cpp.close()
    out_file_header.close()
    out_file_asm.close()



def should_rebuild_symbols():
    global directory, in_files
    
    last_asm_lines = []

    try:
        with open(directory + "/initasm.asm") as last_asm_file:
            last_asm_lines = last_asm_file.readlines()
    except FileNotFoundError: # if file doesn't exist
        return True
    
    if len(last_asm_lines) < 2: # if file doesn't have timestamp line
        return True

    try:
        last_gen_time = datetime.datetime.strptime(last_asm_lines[1][2:].strip(), "%a %b %d %Y %H:%M:%S UTC")
    except ValueError as e: # if time format is not current
        return True
    
    for file_path in in_files:
        for glob_file_path in glob(file_path):
            if datetime.datetime.utcfromtimestamp(os.path.getmtime(glob_file_path)) > last_gen_time: # if one of the .json symbol maps is newer than the last generated code
                return True
    
    if datetime.datetime.utcfromtimestamp(os.path.getmtime(os.path.abspath(__file__))) > last_gen_time: # if this script is newer than the last generated code
        return True
    
    return False



if should_rebuild_symbols():
    rebuild_symbol_gen()
else:
    print("Skipping symbol rebuild")