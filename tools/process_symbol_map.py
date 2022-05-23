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
parser.add_argument('-o', '--output', type=bool, help='output', default=False)
parser.add_argument('input', metavar='file', type=str, nargs='+', help='input json symbol maps')
args = parser.parse_args()

arch = args.arch
platform = args.platform
in_files = args.input
directory = str(args.directory)
debug_output = args.output

if not os.path.exists(directory):
    os.makedirs(directory)

file_header_name = "initcpp.h"

out_file_cpp = open(directory + "/initcpp.cpp", "w")
out_file_header = open(directory + "/" + file_header_name, "w")
out_file_asm = open(directory + "/initasm.asm", "w")

if(debug_output):
    print("CXX output: " + out_file_cpp.name)
    print("HXX output: " + out_file_header.name)
    print("ASM output: " + out_file_asm.name)

version_list = []
symbol_list = []
vtable_list = []
vtable_output = []
var_list = []
include_list = []
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

        sigtype = "SigscanCall" if(byteList[0] == "E8") else "Sigscan"
        
        return { "sig": sig, "mask": mask, "type": sigtype }

    return {}

def read_json(file):
    reader = json.load(file)
    for key in reader:
        if key == "version":
            for obj in reader[key]:
                version_list.append(obj)
        elif key == "vtable":
            for obj in reader[key]:
                vtable_list.append({
                    "name": obj["name"], 
                    "parent": obj.get("parent", ""), 
                    "address": obj.get("address", ""), 
                    "functions": obj.get("functions", []), 
                    "overload": obj.get("overload", "null")
                })
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
            for variable_keys in reader[key].keys():
                var_list.append({
                    "name": variable_keys, 
                    "address": reader[key][variable_keys]
                })
        elif key == "includes":
            for strs in reader[key]:
                include_list.append(strs)

def mangled_name_to_variable(str):
    str = str.replace("?", '_')
    str = str.replace("@", '_')
    return str

def generate_init_cpp():
    #*.hxx
    output_header("")
    output_header("#pragma once")
    output_header("")
    output_header("#include <Zenova/Hook.h>")
    output_header("#include <Zenova/Minecraft.h>")
    if len(symbol_list) > 0 or len(vtable_list) > 0:
        output_header("")
        output_header("extern \"C\" {")
        for a in symbol_list:
            output_header("\textern void* " + a["name"] + "_ptr;")
        for a in vtable_list:
            output_header("\textern void* " + a["name"] + "_vtable;")
        output_header("}")
    output_header("")
    output_header("void InitBedrockPointers();")
    output_header("void InitVersionPointers(const Zenova::Version&);")

    #*.cxx
    output_cxx("")
    output_cxx("#include \"" + file_header_name + "\"")
    for a in include_list:
        output_cxx(a)
    output_cxx("")

    output_cxx("using namespace Zenova::Hook;")
    output_cxx("")

    for a in var_list:
        address = a["address"]
        name = a["name"]
        if address and type(address) == str:
            output_cxx(name + " = reinterpret_cast<" + name[:name.rfind("*")+1] + ">(SlideAddress(" + address + "))" + ";")
        else:
            output_cxx(name + ";")
    output_cxx("")

    output_cxx("extern \"C\" {")
    for a in symbol_list:
        output_cxx("\tvoid* " + a["name"] + "_ptr;")
    for a in vtable_list:
        output_cxx("\tvoid* " + a["name"] + "_vtable;")
    output_cxx("}")
    output_cxx("")

    output_cxx("void InitBedrockPointers() {")
    if len(var_list) > 0:
        for a in var_list:
            if a["address"] == "":
                offset = a["name"].rfind("*") + 1
                output_cxx("\t" + a["name"][offset:].strip() + " = reinterpret_cast<" + a["name"][:offset] + ">(FindVariable(\"" + a["name"] + "\"));")

    if len(symbol_list) > 0:
        for a in symbol_list:
            address = a["address"]
            if(type(address) == str and address != ""):
                output_cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(SlideAddress(" + address + "));")
                
            signature = a["signature"]
            if(type(signature) == dict and len(signature) == 3):
                output_cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(" + signature["type"] + "(\"" + signature["sig"] + "\", \"" + signature["mask"] + "\"));")

    if len(vtable_list) > 0:
        for a in vtable_list:
            name = a["name"]

            if a["address"] != "":
                output_cxx("\t" + name + "_vtable = reinterpret_cast<void*>(SlideAddress(" + a["address"] + "));")
            if a["overload"] == "always" or (a["overload"] == "null" and a["address"] == ""):
                output_cxx("\t" + name + "_vtable = reinterpret_cast<void*>(FindVtable(\"" + name + "\"));")
    output_cxx("}")
    output_cxx("")

    output_cxx("void InitVersionPointers(const Zenova::Version& versionId) {")

    #I want to come back and add support for version based signatures

    cxx_dict_list = [{}, {}]
    for sym in symbol_list:
        address_list = sym["address"]
        if(type(address_list) == list):
            i = 0
            for address in address_list:
                if(address != ""):
                    cxx_str = "\t\t" + sym["name"] + "_ptr = reinterpret_cast<void*>(SlideAddress(" + address_list[i] + "));"
                    if(version_list[i] not in cxx_dict_list[0]):
                        cxx_dict_list[0][version_list[i]] = [ cxx_str ]
                    else:
                        cxx_dict_list[0][version_list[i]].append(cxx_str)
                i += 1

    for var in var_list:
        address_list = var["address"]
        if(type(address_list) == list):
            i = 0
            for address in address_list:
                if(address != ""):
                    offset = var["name"].rfind("*") + 1
                    cxx_str = "\t\t" + var["name"][offset:].strip() + " = reinterpret_cast<" + var["name"][:offset] + ">(SlideAddress(" + address_list[i] + "));"
                    if(version_list[i] not in cxx_dict_list[1]):
                        cxx_dict_list[1][version_list[i]] = [ cxx_str ]
                    else:
                        cxx_dict_list[1][version_list[i]].append(cxx_str)
                i += 1

    prefix = ""
    for version in version_list:
        output_cxx("\t" + prefix + "if(versionId == Zenova::Minecraft::v" + version.replace(".", "_") + ") {")
        for cxx_dict in cxx_dict_list:
            for cxx in cxx_dict.get(version, []):
                output_cxx(cxx)
        output_cxx("\t}")
        prefix = "else " if(prefix == "") else prefix

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
        output_asm("extern " + mangled_name_to_variable(a["mangled_name"]) + "_ptr")
    for a in vtable_list:
        output_asm("extern " + a["name"] + "_vtable")
    output_asm("")
    output_asm("SECTION .text")
    for a in symbol_list:
        output_asm("global " + a["mangled_name"])
        output_asm(a["mangled_name"] + ":")
        output_asm("\tjmp qword [rel " + mangled_name_to_variable(a["mangled_name"]) + "_ptr" + "]")
    for vtable in vtable_list:
        vtable_out = process_vtable(vtable)
        print(vtable_out["name"] + "\n")
        for a in vtable_out["functions_out"]:
            output_asm("global " + a[0])
            output_asm(a[0] + ":")
            output_asm("\tmov " + reg + ", [rel " + vtable_out["name"] + "_vtable]")
            output_asm("\tjmp [" + reg + "+" + str(a[1] * pointer_size) + "]")

def generate_init_func():
    gen_time = datetime.datetime.utcnow().strftime("%a %b %d %Y %H:%M:%S UTC")
    output_cxx("// This file was automatically generated using tools/" + os.path.basename(__file__))
    output_cxx("// Generated on " + gen_time)

    output_header("// This file was automatically generated using tools/" + os.path.basename(__file__))
    output_header("// Generated on " + gen_time)

    generate_init_cpp()

    output_asm("; This file was automatically generated using tools/" + os.path.basename(__file__))
    output_asm("; Generated on " + gen_time)

    if arch == "x86":
        generate_init_func_x86(32)
    if arch == "x64":
        generate_init_func_x86(64)
    if arch == "armeabi-v7a":
        print(arch + " not supported")
        #generate_init_func_arm()

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