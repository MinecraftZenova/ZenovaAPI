#!/usr/bin/python3
import datetime
import os
import argparse
import json
import re
import copy
from glob import glob 
from pathlib import Path
#from itanium_demangler import parse as demangle

file_header_name = "initcpp.h"

class Args:
    def __init__(self):
        parser = argparse.ArgumentParser(description='Processes symbol maps')
        parser.add_argument('-a', '--arch', type=str, help='arch', choices=['x86', 'x64'], required=True)
        parser.add_argument('-p', '--platform', type=str, help='platform', choices=['windows'], required=True)
        parser.add_argument('-r', '--rebuild', help='Force regenerating of files', action='store_true')
        parser.add_argument('-d', '--directory', type=Path, help='directory', default=".")
        parser.add_argument('-o', '--output', help='log debug output', action='store_true')
        parser.add_argument('-e', '--entry', help='create DllMain entry point', action='store_true')
        parser.add_argument('input', metavar='file', type=str, nargs='+', help='input json symbol maps')
        args = parser.parse_args()

        self.arch = args.arch
        self.platform = args.platform
        self.in_files = args.input
        self.rebuild = args.rebuild
        self.directory = str(args.directory)
        self.debug_output = args.output
        self.entry = args.entry

        if not os.path.exists(self.directory):
            os.makedirs(self.directory)

class Output:
    out_file_cpp = None
    out_file_header = None
    out_file_asm = None

    cxx_output = ""
    hxx_output = ""
    asm_output = ""

    def __init__(self, args: Args):
        self.out_file_cpp = open(args.directory + "/initcpp.cpp", "w")
        self.out_file_header = open(args.directory + "/" + file_header_name, "w")
        self.out_file_asm = open(args.directory + "/initasm.asm", "w")

        self.debug = args.debug_output
        if(self.debug):
            print("cxx output: " + self.out_file_cpp.name)
            print("hxx output: " + self.out_file_header.name)
            print("asm output: " + self.out_file_asm.name)

    def cleanup(self):
        self.out_file_cpp.close()
        self.out_file_header.close()
        self.out_file_asm.close()

    def cxx(self, text):
        text += "\n"
        self.cxx_output += "CXX: " + text
        if self.out_file_cpp is not None:
            self.out_file_cpp.write(text)

    def header(self, text):
        text += "\n"
        self.hxx_output += "HXX: " + text
        if self.out_file_header is not None:
            self.out_file_header.write(text)

    def asm(self, text):
        text += "\n"
        self.asm_output += "ASM: " + text
        if self.out_file_asm is not None:
            self.out_file_asm.write(text)

    def dump(self):
        if(self.debug):
            print(self.cxx_output)
            print(self.hxx_output)
            print(self.asm_output)

class Map:
    # 0 = var_name, 1 = mangled_name (sorry)
    symbol_list = []
    symbol_dict = {}
    # cached isn't the best name, holds onto vtables that can't be parsed yet
    cached_vtables = {}
    direct_vtables = {}
    parsed_vtables = {}
    vdtor_list = []
    var_list = []
    var_dict = {"": []}
    include_list = []
    var_count = 0

    # Info for error logging
    current_file_issues = []

    def parse(self, file):
        self.current_file_issues = []
        
        try:
            reader = json.load(file)
            for key in reader:
                self.__parse(key, reader[key])

        except json.decoder.JSONDecodeError:
            print(f"Cannot parse '{file.name}', file does not contain valid JSON")
            return
        
        num_issues = len(self.current_file_issues)
        # Don't print any info for succesfully parsed files
        if num_issues > 0:
            print(f"Found {num_issues} issue{'s'[:num_issues>1]}:")

            for issue in self.current_file_issues:
                print(f" - {issue}")

    def __issue(self, message):
        self.current_file_issues.append(message)

    def __unsupported(self, info: str):
        print(f"Unsupported API: {info}")

    def __addr_helper(addr: str, name: str) -> str:
        if addr == "" or addr == "0x0" or len(addr) < 2:
            return ""
        
        if addr[:2] == "0x":
            # ex: SlideAddress(0xDEADBEEF)
            return f"SlideAddress({addr})"
        else: # signature
            sig = ""
            mask = ""

            byteList = addr.split(" ")

            for byte in byteList:
                if(byte == "?"):
                    mask += "?"
                    sig += "\\x00"
                else:
                    mask += "x"
                    sig += "\\x" + byte

            sigtype = "SigscanCall" if(byteList[0] == "E8" or byteList[0] == "E9") else "Sigscan"

            # ex: Sigscan("\x40\x53\x00", "xx?", "mangled_func_name")
            return f"{sigtype}(\"{sig}\", \"{mask}\", \"{name}\")"

    def __parse(self, key: str, value):
        match key:
            case "vtable":
                self.__parse_vtables(value)

            case "functions":
                self.__parse_funcs(value)

            case "variables":
                self.__parse_variables(key, value)

            case "includes":
                for strs in value:
                    self.include_list.append(strs)

            case _:
                self.__unsupported(key)

    # really hacky workaround for dealing with backreferences in msvc symbols
    # todo: roll basic (de?)mangler that replaces these backreferences with the proper full name
    # https://github.com/microsoft/checkedc-llvm/blob/master/lib/Demangle/MicrosoftDemangle.cpp
    def __handle_backreference(self, name : str, vtable : dict, func : str) -> list:
        parents = vtable["parent"]
        func = re.sub(f'(@@@.*){name}@@', r'\g<1>1@', func)

        if parents is not None:
            ret = []
            for parent_str in parents:
                ret.extend(self.__handle_backreference(parent_str, self.parsed_vtables[parent_str], func))
            return ret
            
        return [ func ]

    # todo: add support for multi inheritance vtables, should be easy with new implementation
    def __process_vtable(self, name: str, vtable: dict):
        noclass_funcs = vtable.setdefault("noclass_funcs", [])

        parent = vtable["parent"] 
        has_parent = parent is not None
        if has_parent:
            for vtable_name in parent:
                v = self.parsed_vtables[vtable_name]
                noclass_funcs.extend(copy.deepcopy(v["noclass_funcs"]))
            if len(noclass_funcs) > 1:
                source = vtable.setdefault("source", [])
                source.extend(parent if len(parent) > 1 else self.parsed_vtables[parent[0]]["source"].copy())
        else:
            noclass_funcs.append([])

        base_noclass_funcs = noclass_funcs[0]

        for index, func in enumerate(vtable["functions"]):
            if func == "":
                vtable["functions"][index] = [ "", 0, 0 ]
                base_noclass_funcs.append("")
            elif func[:3] == "??1":
                self.vdtor_list.append({
                    "vtable": name,
                    "name": Windows.mangle_to_var(func),
                    "mangled_name": func,
                    "address": '0x{:X}'.format(int(vtable["address"][0], 16) + (index * 8))
                })

                vtable["functions"][index] = [ "", 0, 0 ]
                if not has_parent:
                    base_noclass_funcs.append("")
            else:
                noclass_func = func.replace(name + "@@", "@@", 1)
                offset = index
                vtable_offset = 0

                if has_parent:
                    potentials = self.__handle_backreference(name, vtable, noclass_func)

                    # get the offsets of the vtable and location for a function
                    offset = len(base_noclass_funcs)

                    i = 0
                    found = False
                    while i < len(noclass_funcs):
                        parent_funcs = noclass_funcs[i]
                        for potential in potentials:
                            if potential in parent_funcs:
                                noclass_func = potential
                                offset = parent_funcs.index(noclass_func)
                                vtable_offset = i
                                found = True
                                break
                        if found:
                            break
                        i += 1

                # append if not in the current list
                if offset >= len(base_noclass_funcs):
                    base_noclass_funcs.append(noclass_func)

                vtable["functions"][index] = [ func, offset, vtable_offset ]

        self.parsed_vtables[name] = vtable

        if name in self.direct_vtables:
            for vtable_name in self.direct_vtables[name]:
                vtable = self.cached_vtables[vtable_name]
                vtable["count"] += 1
                if vtable["count"] == len(vtable["parent"]):
                    self.cached_vtables.pop(vtable_name)
                    self.__process_vtable(vtable_name, vtable)
            self.direct_vtables.pop(name)

    def __parse_vtables(self, value):
        for index, obj in enumerate(value):
            if "name" not in obj:
                self.__issue(f"Vtable at index {index} is missing required key 'name'")
                continue
            name = obj["name"]

            if name in self.parsed_vtables:
                self.__issue(f"Vtable {name} has already been parsed")
                continue
            
            if "address" not in obj:
                self.__issue(f"Vtable {name} is missing required key 'address'")
                continue
            
            if "functions" not in obj:
                self.__issue(f"Vtable {name} is missing required key 'functions'")
                continue

            parent = obj.get("parent")
            if type(parent) is str:
                parent = [ parent ]

            address = obj["address"]
            if type(address) is str:
                address = [ address ]

            vtable = {
                "parent": parent, 
                "address": address, 
                "functions": obj["functions"]
            }

            if parent is not None:
                if not set(parent).issubset(self.parsed_vtables):
                    vtable["count"] = 0
                    self.cached_vtables.setdefault(name, {}).update(vtable)
                    for p in parent:
                        self.direct_vtables.setdefault(p, []).append(name)
                    continue
            
            self.__process_vtable(name, vtable)
                

    def __add_func(self, version: str, name: str, mangled_name: str, addr: str):
        full_addr = Map.__addr_helper(addr, mangled_name)

        if not full_addr:
            self.__issue(f"Function {mangled_name} uses invalid address")
            return

        self.symbol_dict.setdefault(version, []).append({
            "name": name,
            "mangled_name": mangled_name,
            # used directly in the reinterpret_cast call
            "address": full_addr,
        })

    def __parse_funcs(self, value):
        for index, obj in enumerate(value):
            if not("name" in obj):
                self.__issue(f"Function at index {index}, object is missing required key 'name'")
                continue

            mangled_name = obj["name"]
            if any(item[1] == mangled_name for item in self.symbol_list):
                self.__issue(f"Found duplicate symbol: '{mangled_name}'")
                continue

            name = Windows.mangle_to_var(mangled_name)

            if not("address" in obj):
                self.__issue(f"Function {mangled_name} is missing required key 'address'")
                continue
            
            addr = obj["address"]

            if type(addr) is str:
                self.__add_func("default", name, mangled_name, addr)
            elif type(addr) is dict:
                for k, v in addr.items():
                    self.__add_func(k, name, mangled_name, v)
            else:
                self.__issue(f"Address in {mangled_name} is an unsupported type")
                continue

            self.symbol_list.append([name, mangled_name])

    def __parse_variables(self, key, value):
        for var_name in value.keys():
            if var_name in self.var_list:
                self.__issue(f"Found duplicate variable: '{var_name}'")
                continue

            self.var_list.append(var_name)
            addr = value[var_name]
            if type(addr) is str:
                self.var_dict[""].append({
                    "offset": self.var_count,
                    "address": addr
                })
            elif type(addr) is dict:
                for ver in addr:
                    self.var_dict[ver] = self.var_dict.get(key, []) + [{
                        "offset": self.var_count,
                        "address": addr[ver]
                    }]
            self.var_count += 1

    def generate_header(self, out: Output):
        #*.hxx
        out.header("")
        out.header("#pragma once")
        if len(self.symbol_list) > 0 or len(self.parsed_vtables) > 0:
            out.header("")
            out.header("extern \"C\" {")
            for name, _ in self.symbol_list:
                if name:
                    out.header("\textern void* " + name + "_ptr;")
            for name, vtable in self.parsed_vtables.items():
                out.header("\textern void* " + name + "_vtable;")

                if len(vtable["address"]) > 1:
                    for source in vtable["source"][1:]:
                        out.header(f"\textern void* {name}_{source}_vtable;")
            for a in self.vdtor_list:
                if a["name"]:
                    out.header("\textern void* " + a["name"] + "_ptr;")
            out.header("}")
        out.header("")
        out.header("void InitBedrockPointers();")

    def generate_init(self, out: Output):
        self.generate_header(out)

        #*.cxx
        out.cxx("")
        out.cxx("#include <array>")
        out.cxx("#include <Zenova/Hook.h>")
        out.cxx("#include <Zenova/Minecraft.h>")
        out.cxx("")
        out.cxx("#include \"" + file_header_name + "\"")
        for include in self.include_list:
            out.cxx("#include \"" + include + "\"")
        out.cxx("")

        out.cxx("using namespace Zenova::Hook;")
        out.cxx("")

        # uses lambdas to initialize the global variables (allows for "const" initialization)
        if self.var_list:
            var_arr_str = "std::array<uintptr_t, " + str(len(self.var_list)) + ">"
            out.cxx("namespace {")
            out.cxx("static " + var_arr_str + " var_addrs = []() -> " + var_arr_str + " {")
            out.cxx("\tconst Zenova::Version& versionId = Zenova::Minecraft::version();")
            out.cxx("\t" + var_arr_str + " vars{};") # should this be allocated on the heap?
            out.cxx("")

            for var in self.var_dict[""]:
                offset, addr = var.values()
                out.cxx("\tvars[" + str(offset) + "] = SlideAddress(" + str(addr) + ");")

            out.cxx("")
            prefix = ""
            for ver in self.var_dict:
                if ver != "":
                    out.cxx("\t" + prefix + "if (versionId == \"" + ver + "\") {")
                    for var in self.var_dict[ver]:
                        offset, addr = var.values()
                        out.cxx("\t\tvars[" + str(offset) + "] = SlideAddress(" + str(addr) + ");")
                    out.cxx("\t}")
                    prefix = "else "

            if prefix == "else ":
                out.cxx("")

            out.cxx("\treturn vars;")
            out.cxx("}();")
            out.cxx("}")
            out.cxx("")

            i = 0
            for name in self.var_list:
                if name:
                    out.cxx(name + " = *reinterpret_cast<" + name[:name.rfind("&")] + "*>(var_addrs[" + str(i) + "])" + ";")
                i += 1
            out.cxx("")

        for name, _ in self.symbol_list:
            if name:
                out.cxx("void* " + name + "_ptr;")
        for name, vtable in self.parsed_vtables.items():
            out.cxx("void* " + name + "_vtable;")

            if len(vtable["address"]) > 1:
                for source in vtable["source"][1:]:
                    out.cxx(f"void* {name}_{source}_vtable;")
        for a in self.vdtor_list:
            if a["name"]:
                out.cxx("void* " + a["name"] + "_ptr;")
        out.cxx("")

        out.cxx("void InitBedrockPointers() {")
        out.cxx("\tconst Zenova::Version& versionId = Zenova::Minecraft::version();")
        # todo: ensure default is last
        # todo: handle more complex version support
        # - copying common sets for multiple versions into a function to call in both cases
        if len(self.symbol_dict.items()) == 1:
            for ver, symbols in self.symbol_dict.items():
                for func in symbols:
                    out.cxx("\t" + func["name"] + "_ptr = reinterpret_cast<void*>(" + func["address"] + ");")
        else:
            prefix = ""
            for ver, symbols in self.symbol_dict.items():
                if ver == "default":
                    out.cxx("\telse {")
                else:
                    out.cxx(f"\t{prefix}if (versionId == \"{ver}\") {{")
                    prefix = "else "

                for func in symbols:
                    out.cxx("\t\t" + func["name"] + "_ptr = reinterpret_cast<void*>(" + func["address"] + ");")
                        
                out.cxx("\t}")

        for name, vtable in self.parsed_vtables.items():
            addrs = vtable["address"]
            out.cxx(f"\t{name}_vtable = reinterpret_cast<void*>(SlideAddress({addrs[0]}));")

            if len(vtable["address"]) > 1:
                index = 1
                for source in vtable["source"][1:]:
                    out.cxx(f"\t{name}_{source}_vtable = reinterpret_cast<void*>(SlideAddress({addr}));")
                    index += 1

        for a in self.vdtor_list:
            out.cxx("\t" + a["name"] + "_ptr = reinterpret_cast<void*>(GetRealDtor(SlideAddress(" + a["address"] + ")));")
        out.cxx("}")
        out.cxx("")

# Supported Platforms
class Windows:
    def mangle_to_var(name):
        name = name.replace("?", '_')
        name = name.replace("@", '_')
        return name

    # relies on NASM, MASM doesn't allow long identifiers
    def __x86(self, out: Output, map: Map, is64bit):
        if is64bit:
            reg = "rax"
            pointer_size = 8
            out.asm("bits 64")
        else:
            reg = "eax"
            pointer_size = 4

        out.asm("SECTION .data")
        for name, _ in map.symbol_list:
            out.asm("extern " + name + "_ptr")
        for a in map.vdtor_list:
            out.asm("extern " + a["name"] + "_ptr")
        for name, vtable in map.parsed_vtables.items():
            out.asm("extern " + name + "_vtable")

            if len(vtable["address"]) > 1:
                for source in vtable["source"][1:]:
                    out.asm(f"extern {name}_{source}_vtable")
        out.asm("")
        out.asm("SECTION .text")
        for var_name, mangled_name in map.symbol_list:
            out.asm("global " + mangled_name)
            out.asm(mangled_name + ":")
            out.asm("\tjmp qword [rel " + var_name + "_ptr" + "]")
        for a in map.vdtor_list:
            out.asm("global " + a["mangled_name"])
            out.asm(a["mangled_name"] + ":")
            out.asm("\tjmp qword [rel " + a["name"] + "_ptr]")
        for name, vtable in map.parsed_vtables.items():
            for symbol, offset, vtable_offset in vtable["functions"]:
                if symbol != "":
                    vtable_name = name
                    if vtable_offset > 0:
                        vtable_name += "_" + vtable["source"][vtable_offset]

                    out.asm("global " + symbol)
                    out.asm(symbol + ":")
                    out.asm(f"\tmov {reg}, [rel {vtable_name}_vtable]")
                    out.asm("\tjmp [" + reg + "+" + str(offset * pointer_size) + "]")

    def generate_init(self, out: Output):
        out.cxx("")
        out.cxx("BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {")
        out.cxx("\tif(fdwReason == DLL_PROCESS_ATTACH) {")
        out.cxx("\t\tInitBedrockPointers();")
        out.cxx("\t}")
        out.cxx("\treturn TRUE;")
        out.cxx("}")

    def generate_asm(self, arch: str, out: Output, map: Map):
        match arch:
            case "x64":
                self.__x86(out, map, True)

            case "x86":
                self.__x86(out, map, False)

            case _:
                print(f"Unsupported arch: {arch}")

def generate(args: Args, out: Output, map: Map):
    file_comment = "This file was automatically generated using tools/" + os.path.basename(__file__)
    gen_time = datetime.datetime.utcnow().strftime("%a %b %d %Y %H:%M:%S UTC")
    out.cxx("// " + file_comment)
    out.cxx("// " + gen_time)

    out.header("// " + file_comment)
    out.header("// " + gen_time)

    out.asm("; " + file_comment)
    out.asm("; " + gen_time)

    map.generate_init(out)

    match args.platform:
        case "windows":
            platform = Windows()

        case _:
            print(f"Unsupported platform: {args.platform}")

    if args.entry:
        platform.generate_init(out)

    platform.generate_asm(args.arch, out, map)

def should_rebuild_symbols(args: Args) -> bool:
    if args.rebuild:
        return True
    
    last_asm_lines = []

    try:
        with open(args.directory + "/initasm.asm") as last_asm_file:
            last_asm_lines = last_asm_file.readlines()
    except FileNotFoundError: # if file doesn't exist
        return True
    
    if len(last_asm_lines) < 2: # if file doesn't have timestamp line
        return True

    try:
        last_gen_time = datetime.datetime.strptime(last_asm_lines[1][2:].strip(), "%a %b %d %Y %H:%M:%S UTC")
    except ValueError as e: # if time format is not current
        return True
    
    for file_path in args.in_files:
        for glob_file_path in glob(file_path):
            if datetime.datetime.utcfromtimestamp(os.path.getmtime(glob_file_path)) > last_gen_time: # if one of the .json symbol maps is newer than the last generated code
                return True
    
    if datetime.datetime.utcfromtimestamp(os.path.getmtime(os.path.abspath(__file__))) > last_gen_time: # if this script is newer than the last generated code
        return True
    
    return False

args = Args()
if should_rebuild_symbols(args):
    out = Output(args)

    map = Map()

    for file_path in args.in_files:
        for glob_file_path in glob(file_path):
            file_full_path = os.path.abspath(glob_file_path)
            print("Parsing Symbol Map: " + file_full_path)
            with open(file_full_path, "r") as f:
                map.parse(f)

    generate(args, out, map)

    out.dump()
    out.cleanup()
else:
    print("Skipping symbol rebuild")
