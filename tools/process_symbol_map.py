#!/usr/bin/python3
import datetime
import os
import argparse
import json
import re
import copy
from glob import glob 
from pathlib import Path
from demangle import MSVCDemangler, SymbolNode
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

    cxx_output = []
    hxx_output = []
    asm_output = []

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
        self.cxx_output.append(text)
        if self.out_file_cpp is not None:
            self.out_file_cpp.write(text)

    def header(self, text):
        text += "\n"
        self.hxx_output.append(text)
        if self.out_file_header is not None:
            self.out_file_header.write(text)

    def asm(self, text):
        text += "\n"
        self.asm_output.append(text)
        if self.out_file_asm is not None:
            self.out_file_asm.write(text)

    def dump(self):
        if(self.debug):
            for cxx in self.cxx_output: print(cxx)
            for hxx in self.hxx_output: print(hxx)
            for asm in self.asm_output: print(asm)

class Map:
    class Symbol:
        def __init__(self, node: SymbolNode, addr: dict | str):
            self.name = node
            self.address = addr

    class VFunc:
        def __init__(self, node: SymbolNode):
            self.name = node
            self.vtable = 0
            self.index = 0
            self.isDtor = False

    class Variable:
        def __init__(self, name: str, addr: dict | str):
            self.name = name
            self.address = addr

        def __eq__(self, item):
            if type(item) is str:
                return self.name == item

            return NotImplemented

    class Vtable:
        def __init__(self, parent: list, addr: list, funcs: list):
            self.parent = parent
            self.address = addr
            self.functions: list[Map.VFunc] = funcs

            self.source: list[str] = []
            self.noclass_funcs: list[list] = []

            self.parentsParsed = 0

    def __init__(self):
        self.symbols: list[Map.Symbol] = []
        self.variables: list[Map.Variable] = []
        # holds onto vtables that can't be parsed yet
        self.cached_vtables: dict[str, Map.Vtable] = {}
        self.direct_vtables = {}
        self.vtables: dict[str, Map.Vtable] = {}
        self.include_list = []

        # Info for error logging
        self.current_file_issues = []

    def parse(self, file):
        self.current_file_issues = []
        
        try:
            reader = json.load(file)
            for key in reader:
                self.__parse(key, reader[key])

        except json.decoder.JSONDecodeError:
            print(f"Cannot parse '{file.name}', file does not contain valid JSON")
            return
        
        vtable_count = len(self.cached_vtables)
        if vtable_count > 0:
            print(f"{vtable_count} unparsed vtable{'s'[:vtable_count^1]}:")

            for name, vtable in self.cached_vtables.items():
                parent = list(filter(lambda i: i not in self.vtables, vtable.parent))
                print(f" - {name} is missing parent{'s'[:len(parent)^1]}: {parent}")
        
        num_issues = len(self.current_file_issues)
        # Don't print any info for succesfully parsed files
        if num_issues > 0:
            print(f"Found {num_issues} issue{'s'[:num_issues>1]}:")

            for issue in self.current_file_issues:
                print(f" - {issue}")

    def __issue(self, message):
        self.current_file_issues.append(message)

    def __parse(self, key: str, value: list | dict):
        match key:
            case "vtable" if type(value) is list:
                self.__parse_vtables(value)

            case "functions" if type(value) is list:
                self.__parse_funcs(value)

            case "variables" if type(value) is dict:
                self.__parse_variables(value)

            case "includes" if type(value) is list:
                for strs in value:
                    self.include_list.append(strs)

            case _:
                print(f"Unsupported API or Type: {key}({type(value)})")

    def __process_vtable(self, name: str, vtable: Vtable):
        parent = vtable.parent

        if parent:
            for vtable_name in parent:
                vtable.noclass_funcs.extend(copy.deepcopy(self.vtables[vtable_name].noclass_funcs))
            if len(vtable.source) + 1 != len(vtable.noclass_funcs):
                vtable.source.extend(vtable.parent[1:] if len(vtable.parent) > 1 else self.vtables[vtable.parent[0]].source)
        else:
            vtable.noclass_funcs.append([])

        base_noclass_funcs = vtable.noclass_funcs[0]

        for func in vtable.functions:
            if not func:
                base_noclass_funcs.append("")
                continue
            
            symbol = func.name.mangle(False)
            symbol = symbol.replace(name + "@@", "@@", 1)

            if symbol[:3] == "??1":
                func.isDtor = True
            func.index = len(base_noclass_funcs)

            if parent:
                for i, parent_funcs in enumerate(vtable.noclass_funcs):
                    if symbol in parent_funcs:
                        func.vtable = i
                        func.index = parent_funcs.index(symbol)
                        break

            # append if not in the current list
            if func.vtable == 0 and func.index == len(base_noclass_funcs):
                base_noclass_funcs.append(symbol)

        self.vtables[name] = vtable

        if name in self.direct_vtables:
            for vtable_name in self.direct_vtables[name]:
                vtable = self.cached_vtables[vtable_name]
                vtable.parentsParsed += 1
                if vtable.parentsParsed == len(vtable.parent):
                    self.cached_vtables.pop(vtable_name)
                    self.__process_vtable(vtable_name, vtable)
            self.direct_vtables.pop(name)

    def __parse_vtables(self, value: list[dict]):
        for index, obj in enumerate(value):
            if "name" not in obj:
                self.__issue(f"Vtable at index {index} is missing required key 'name'")
                continue
            name = obj["name"]

            if name in self.vtables:
                self.__issue(f"Vtable {name} has already been parsed")
                continue
            
            if "address" not in obj:
                self.__issue(f"Vtable {name} is missing required key 'address'")
                continue
            
            if "functions" not in obj:
                self.__issue(f"Vtable {name} is missing required key 'functions'")
                continue

            funcs = []
            for mangled in obj["functions"]:
                if mangled:
                    func = MSVCDemangler().parse(mangled)
                    func.verify()

                    funcs.append(Map.VFunc(func))
                else:
                    funcs.append(None)

            parent = obj.get("parent")
            if type(parent) is str:
                parent = [ parent ]

            address = obj["address"]
            if type(address) is str:
                address = [ address ]

            vtable = Map.Vtable(parent, address, funcs)

            if parent and not set(parent).issubset(self.vtables):
                self.cached_vtables[name] = vtable
                for p in parent:
                    self.direct_vtables.setdefault(p, []).append(name)
                continue
            
            self.__process_vtable(name, vtable)

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

    def __parse_funcs(self, value):
        for index, obj in enumerate(value):
            if not("name" in obj):
                self.__issue(f"Function at index {index}, object is missing required key 'name'")
                continue

            mangled_name = obj["name"]
            if any(item.name.symbol == mangled_name for item in self.symbols):
                self.__issue(f"Found duplicate symbol: '{mangled_name}'")
                continue

            func = MSVCDemangler().parse(mangled_name)
            func.verify()

            if not("address" in obj):
                self.__issue(f"Function {mangled_name} is missing required key 'address'")
                continue
            
            addr = obj["address"]

            if type(addr) is str:
                addr = Map.__addr_helper(addr, mangled_name)
            elif type(addr) is dict:
                for k, v in addr.items():
                    full_addr = Map.__addr_helper(v, mangled_name)

                    if not full_addr:
                        self.__issue(f"Function {mangled_name} uses invalid address")
                        continue

                    addr[k] = full_addr
            else:
                self.__issue(f"Address in {mangled_name} is an unsupported type")
                continue
            
            self.symbols.append(Map.Symbol(func, addr))

    def __parse_variables(self, value: dict[str, dict | str]):
        for name, addr in value.items():
            # is this needed? python's json loader will overwrite duplicate keys
            if name in self.variables:
                self.__issue(f"Found duplicate variable: '{name}'")
                continue

            self.variables.append(Map.Variable(name, addr))

    def generate_header(self, out: Output):
        #*.hxx
        out.header("")
        out.header("#pragma once")
        if len(self.vtables) > 0:
            out.header("")
            out.header("extern \"C\" {")
            for name, vtable in self.vtables.items():
                out.header(f"\textern void* {name}_vtable;")

                for i in range(len(vtable.address)-1):
                    out.header(f"\textern void* {name}_{vtable.source[i]}_vtable;")
            out.header("}")

    def generate_init(self, out: Output):
        self.generate_header(out)

        #*.cxx
        out.cxx("")
        out.cxx("#include <array>")
        out.cxx("#include <Zenova/Hook.h>")
        out.cxx("#include <Zenova/Version.h>")
        out.cxx("")
        out.cxx("#include \"" + file_header_name + "\"")
        for include in self.include_list:
            out.cxx("#include \"" + include + "\"")
        out.cxx("")

        out.cxx("using namespace Zenova::Hook;")
        out.cxx("")

        # uses lambdas to initialize the global variables (allows for "const" initialization)
        if self.variables:
            out.cxx("namespace {")
            out.cxx("static auto var_addrs = []() {")
            out.cxx("\tconst Zenova::Version& versionId = Zenova::Version::launched();")
            out.cxx(f"\tstd::array<uintptr_t, {len(self.variables)}> vars{{}};")
            out.cxx("")

            for index, var in enumerate(self.variables):
                addr = var.address
                if type(addr) is str:
                    out.cxx(f"\tvars[{index}] = SlideAddress({addr});")
                elif type(addr) is dict:
                    prefix = "if"
                    for ver, a in addr.items():
                        if ver == "default":
                            out.cxx("\telse {")
                        else:
                            # todo: change this to use int constructor
                            out.cxx(f'\t{prefix} (versionId == "{ver}") {{')
                            prefix = "else if"

                        out.cxx(f"\t\tvars[{index}] = SlideAddress({a});")
                        out.cxx("\t}")

            out.cxx("\treturn vars;")
            out.cxx("}();")
            out.cxx("}")
            out.cxx("")

            for i, variable in enumerate(self.variables):
                name = variable.name
                cpp_type = name[:name.rfind("&")]
                out.cxx(f"{name} = *reinterpret_cast<{cpp_type}*>(var_addrs[{i}]);")
            out.cxx("")

        for name, vtable in self.vtables.items():
            out.cxx(f"void* {name}_vtable;")

            for i in range(len(vtable.address)-1):
                out.cxx(f"void* {name}_{vtable.source[i]}_vtable;")

        if len(self.symbols) > 0:
            out.cxx('extern "C" {')
            for symbol in self.symbols:
                out.cxx(f"\tvoid* {symbol.name.to_var()}_ptr;")
            out.cxx('}')
        
        out.cxx("")

        out.cxx("void InitBedrockPointers() {")
        out.cxx("\tconst Zenova::Version& versionId = Zenova::Version::launched();")

        for name, vtable in self.vtables.items():
            addrs = vtable.address
            out.cxx(f"\t{name}_vtable = reinterpret_cast<void*>(SlideAddress({addrs[0]}));")

            for i in range(len(vtable.address)-1):
                out.cxx(f"\t{name}_{vtable.source[i]}_vtable = reinterpret_cast<void*>(SlideAddress({addrs[i+1]}));")

        for symbol in self.symbols:
            var = symbol.name.to_var()
            addr = symbol.address

            if type(addr) is str:
                out.cxx(f"\t{var}_ptr = reinterpret_cast<void*>({addr});")
            elif type(addr) is dict:
                prefix = "if"
                for ver, addr in symbol.address.items():
                    if ver == "default":
                        out.cxx("\telse {")
                    else:
                        # todo: change this to use int constructor
                        out.cxx(f'\t{prefix} (versionId == "{ver}") {{')
                        prefix = "else if"

                    out.cxx(f"\t\t{var}_ptr = reinterpret_cast<void*>({addr});")
                    out.cxx("\t}")

        out.cxx("}")
        out.cxx("")

# Supported Platforms
class Windows:
    # relies on NASM, MASM doesn't allow long identifiers
    def __x86(self, out: Output, map: Map, is64bit: bool):
        if is64bit:
            reg = "rax"
            pointer_size = 8
            out.asm("bits 64")
        else:
            reg = "eax"
            pointer_size = 4

        out.asm("SECTION .data")
        for symbol in map.symbols:
            out.asm(f"extern {symbol.name.to_var()}_ptr")
        for name, vtable in map.vtables.items():
            out.asm("extern " + name + "_vtable")

            for i in range(len(vtable.address)-1):
                out.asm(f"extern {name}_{vtable.source[i]}_vtable")
        out.asm("")

        out.asm("SECTION .text")
        for symbol in map.symbols:
            out.asm("global " + symbol.name.symbol)
            out.asm(symbol.name.symbol + ":")
            out.asm(f"\tjmp qword [rel {symbol.name.to_var()}_ptr]")
        for name, vtable in map.vtables.items():
            for func in vtable.functions:
                if not func:
                    continue

                vtable_name = name
                if len(vtable.address) > 1 and func.vtable > 0: 
                    vtable_name += f"_{vtable.source[func.vtable-1]}" 

                out.asm("global " + func.name.symbol)
                out.asm(func.name.symbol + ":")
                if func.isDtor:
                    out.asm(f"\tmov edx, 0")
                out.asm(f"\tmov {reg}, [rel {vtable_name}_vtable]")
                out.asm(f"\tjmp [{reg}+{(func.index * pointer_size)}]")

    def generate_init(self, out: Output):
        out.cxx("")
        out.cxx("#include <Windows.h>")
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
