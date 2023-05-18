#!/usr/bin/python3
from copy import deepcopy
from enum import Enum, Flag, auto
import numpy as np

def nw(x, y, match = 1, mismatch = 1, gap = 1):
    nx = len(x)
    ny = len(y)
    # Optimal score at each possible pair of characters.
    F = np.zeros((nx + 1, ny + 1))
    F[:,0] = np.linspace(0, -nx * gap, nx + 1)
    F[0,:] = np.linspace(0, -ny * gap, ny + 1)
    # Pointers to trace through an optimal aligment.
    P = np.zeros((nx + 1, ny + 1))
    P[:,0] = 3
    P[0,:] = 4
    # Temporary scores.
    t = np.zeros(3)
    for i in range(nx):
        for j in range(ny):
            if x[i] == y[j]:
                t[0] = F[i,j] + match
            else:
                t[0] = F[i,j] - mismatch
            t[1] = F[i,j+1] - gap
            t[2] = F[i+1,j] - gap
            tmax = np.max(t)
            F[i+1,j+1] = tmax
            if t[0] == tmax:
                P[i+1,j+1] += 2
            if t[1] == tmax:
                P[i+1,j+1] += 3
            if t[2] == tmax:
                P[i+1,j+1] += 4
    # Trace through an optimal alignment.
    i = nx
    j = ny
    rx = []
    ry = []
    while i > 0 or j > 0:
        if P[i,j] in [2, 5, 6, 9]:
            rx.append(x[i-1])
            ry.append(y[j-1])
            i -= 1
            j -= 1
        elif P[i,j] in [3, 5, 7, 9]:
            rx.append(x[i-1])
            ry.append('-')
            i -= 1
        elif P[i,j] in [4, 6, 7, 9]:
            rx.append('-')
            ry.append(y[j-1])
            j -= 1
    # Reverse the strings.
    rx = ''.join(rx)[::-1]
    ry = ''.join(ry)[::-1]
    return '\n'.join([rx, ry])

class SpecialIntrinsicKind(Enum):
    Vftable = auto()
    Vbtable = auto()
    Typeof = auto()
    VcallThunk = auto()
    LocalStaticGuard = auto()
    StringLiteralSymbol = auto()
    UdtReturning = auto()
    DynamicInitializer = auto()
    DynamicAtexitDestructor = auto()
    RttiTypeDescriptor = auto()
    RttiBaseClassDescriptor = auto()
    RttiBaseClassArray = auto()
    RttiClassHierarchyDescriptor = auto()
    RttiCompleteObjLocator = auto()
    LocalVftable = auto()
    LocalStaticThreadGuard = auto()

class IntrinsicFunctionKind(Enum):
    _None = auto()
    New = auto()
    Delete = auto()
    Assign = auto()
    RightShift = auto()
    LeftShift = auto()
    LogicalNot = auto()
    Equals = auto()
    NotEquals = auto()
    ArraySubscript = auto()
    Pointer = auto()
    Dereference = auto()
    Increment = auto()
    Decrement = auto()
    Minus = auto()
    Plus = auto()
    BitwiseAnd = auto()
    MemberPointer = auto()
    Divide = auto()
    Modulus = auto()
    LessThan = auto()
    LessThanEqual = auto()
    GreaterThan = auto()
    GreaterThanEqual = auto()
    Comma = auto()
    Parens = auto()
    BitwiseNot = auto()
    BitwiseXor = auto()
    BitwiseOr = auto()
    LogicalAnd = auto()
    LogicalOr = auto()
    TimesEqual = auto()
    PlusEqual = auto()
    MinusEqual = auto()
    DivEqual = auto()
    ModEqual = auto()
    RshEqual = auto()
    LshEqual = auto()
    BitwiseAndEqual = auto()
    BitwiseOrEqual = auto()
    BitwiseXorEqual = auto()
    VbaseDtor = auto()
    VecDelDtor = auto()
    DefaultCtorClosure = auto()
    ScalarDelDtor = auto()
    VecCtorIter = auto()
    VecDtorIter = auto()
    VecVbaseCtorIter = auto()
    VdispMap = auto()
    EHVecCtorIter = auto()
    EHVecDtorIter = auto()
    EHVecVbaseCtorIter = auto()
    CopyCtorClosure = auto()
    LocalVftableCtorClosure = auto()
    ArrayNew = auto()
    ArrayDelete = auto()
    ManVectorCtorIter = auto()
    ManVectorDtorIter = auto()
    EHVectorCopyCtorIter = auto()
    EHVectorVbaseCopyCtorIter = auto()
    VectorCopyCtorIter = auto()
    VectorVbaseCopyCtorIter = auto()
    ManVectorVbaseCopyCtorIter = auto()
    CoAwait = auto()
    Spaceship = auto()
    MaxIntrinsic = auto()

class FuncClass(Flag):
    _None = 0
    Public = auto()
    Protected = auto()
    Private = auto()
    Global = auto()
    Static = auto()
    Virtual = auto()
    Far = auto()
    ExternC = auto()
    NoParameterList = auto()
    VirtualThisAdjust = auto()
    VirtualThisAdjustEx = auto()
    StaticThisAdjust = auto()

class Qualifiers(Flag):
  _None = 0
  Const = auto()
  Volatile = auto()
  Far = auto()
  Huge = auto()
  Unaligned = auto()
  Restrict = auto()
  Pointer64 = auto()

class CallingConv(Enum):
    _None = auto()
    Cdecl = auto()
    Pascal = auto()
    Thiscall = auto()
    Stdcall = auto()
    Fastcall = auto()
    Clrcall = auto()
    Eabi = auto()
    Vectorcall = auto()
    Regcall = auto()

class PointerAffinity(Enum): 
    _None = auto()
    Pointer = auto()
    Reference = auto()
    RValueReference = auto()

class FunctionRefQualifier(Enum): 
    _None = auto()
    Reference = auto()
    RValueReference = auto()

class QualifierMangleMode(Enum):
    Drop = auto() 
    Mangle = auto()
    Result = auto()

siks = [
    [ "?_7", SpecialIntrinsicKind.Vftable ],
    [ "?_8", SpecialIntrinsicKind.Vbtable ],
    [ "?_9", SpecialIntrinsicKind.VcallThunk ],
    [ "?_A", SpecialIntrinsicKind.Typeof ],
    [ "?_B", SpecialIntrinsicKind.LocalStaticGuard ],
    [ "?_C", SpecialIntrinsicKind.StringLiteralSymbol ],
    [ "?_P", SpecialIntrinsicKind.UdtReturning ],
    [ "?_R0", SpecialIntrinsicKind.RttiTypeDescriptor ],
    [ "?_R1", SpecialIntrinsicKind.RttiBaseClassDescriptor ],
    [ "?_R2", SpecialIntrinsicKind.RttiBaseClassArray ],
    [ "?_R3", SpecialIntrinsicKind.RttiClassHierarchyDescriptor ],
    [ "?_R4", SpecialIntrinsicKind.RttiCompleteObjLocator ],
    [ "?_S", SpecialIntrinsicKind.LocalVftable ],
    [ "?__E", SpecialIntrinsicKind.DynamicInitializer ],
    [ "?__F", SpecialIntrinsicKind.DynamicAtexitDestructor ],
    [ "?__J", SpecialIntrinsicKind.LocalStaticThreadGuard ],
]

basicIFKs = [
    IntrinsicFunctionKind._None,
    IntrinsicFunctionKind._None,
    IntrinsicFunctionKind.New,
    IntrinsicFunctionKind.Delete,
    IntrinsicFunctionKind.Assign,
    IntrinsicFunctionKind.RightShift,
    IntrinsicFunctionKind.LeftShift,
    IntrinsicFunctionKind.LogicalNot,
    IntrinsicFunctionKind.Equals,
    IntrinsicFunctionKind.NotEquals,
    IntrinsicFunctionKind.ArraySubscript,
    IntrinsicFunctionKind._None,
    IntrinsicFunctionKind.Pointer,
    IntrinsicFunctionKind.Dereference,
    IntrinsicFunctionKind.Increment,
    IntrinsicFunctionKind.Decrement,
    IntrinsicFunctionKind.Minus,
    IntrinsicFunctionKind.Plus,
    IntrinsicFunctionKind.BitwiseAnd,
    IntrinsicFunctionKind.MemberPointer,
    IntrinsicFunctionKind.Divide,
    IntrinsicFunctionKind.Modulus,
    IntrinsicFunctionKind.LessThan,
    IntrinsicFunctionKind.LessThanEqual,
    IntrinsicFunctionKind.GreaterThan,
    IntrinsicFunctionKind.GreaterThanEqual,
    IntrinsicFunctionKind.Comma,
    IntrinsicFunctionKind.Parens,
    IntrinsicFunctionKind.BitwiseNot,
    IntrinsicFunctionKind.BitwiseXor,
    IntrinsicFunctionKind.BitwiseOr,
    IntrinsicFunctionKind.LogicalAnd,
    IntrinsicFunctionKind.LogicalOr,
    IntrinsicFunctionKind.TimesEqual,
    IntrinsicFunctionKind.PlusEqual,
    IntrinsicFunctionKind.MinusEqual,
]

class StringView:
    def __init__(self, s: str, pos = 0):
        self.pos = pos
        self.old_pos = pos
        self.view = s

    def update(self):
        s = self.view[self.old_pos:self.pos]
        self.old_pos = self.pos
        return s

    def startsWith(self, s: str):
        return self.view.startswith(s, self.pos)
    
    def startsWithDigit(self):
        return self.view[self.pos].isnumeric()

    def dropFront(self, N = 1):
        self.pos += min(N, self.size())

    def popFront(self):
        c = self.view[self.pos]
        self.pos += 1
        return c

    def consumeFront(self, s: str):
        if self.startsWith(s):
            self.dropFront(len(s))
            return True
        return False

    def popString(self, s: str):
        r = self.get()
        i = r.find(s)
        if i != -1:
            self.dropFront(i + 1)
            return r[:i]
        else:
            raise RuntimeError("StringView::popString")

    def front(self):
        return self.view[self.pos]

    def get(self):
        return self.view[self.pos:]

    def copy(self):
        return StringView(self.view, self.pos)

    def size(self):
        return len(self.view) - self.pos

    def __str__(self):
        return self.get()

    def __getitem__(self, key):
        return self.get()[key]

def outputSpaceIfNecessary(s: str):
    if s:
        c = s[-1]
        if c.isalnum() or c == '>':
            return " "

    return ""

def outputQualifiers(q: Qualifiers, spaceBefore = True, spaceAfter = False):
    if q == Qualifiers._None:
        return ""

    o = ""
    for mask in [Qualifiers.Const, Qualifiers.Volatile, Qualifiers.Restrict]:
        if not q & mask:
            continue

        if spaceBefore:
            o += " "
        
        match mask:
            case Qualifiers.Const: o += "const"
            case Qualifiers.Volatile: o += "volatile"
            case Qualifiers.Restrict: o += "__restrict"
        
    if spaceAfter:
        o += outputSpaceIfNecessary(o)

    return o

def outputCallingConvention(cc: CallingConv):
    match cc:
        case CallingConv.Cdecl: return "__cdecl"
        case CallingConv.Fastcall: return "__fastcall"
        case CallingConv.Pascal: return "__pascal"
        case CallingConv.Regcall: return "__regcall"
        case CallingConv.Stdcall: return "__stdcall"
        case CallingConv.Thiscall: return "__thiscall"
        case CallingConv.Eabi: return "__eabi"
        case CallingConv.Vectorcall: return "__vectorcall"
        case CallingConv.Clrcall: return "__clrcall"
    return ""

class Node:
    def __init__(self):
        self.mangled = ""
        self.backref = ""

    def update(self, mangled: StringView):
        if self.backref: 
            self.backref += mangled.update()
        else:
            self.mangled += mangled.update()

    def mangle(self, backref = True):
        if self.mangled: return self.mangled
        
        raise RuntimeError("mangled is empty")

    def getMangled(self, backref = True):
        if backref and self.backref: return self.backref
        return self.mangle(backref)

class IdentifierNode(Node):
    def __init__(self):
        super().__init__()
        self.templateParams: NodeArrayNode = None

    def getTemplateParams(self):
        if self.templateParams:
            o = f"<{str(self.templateParams)}"
            o += (" " if o[-1] == ">" else "") + ">"
            return o
        return ""

class NamedIdentifierNode(IdentifierNode):
    def __init__(self, s: str):
        super().__init__()
        self.name = s

    def __str__(self):
        return self.name + self.getTemplateParams()

    def mangle(self, backref = True):
        template = self.templateParams.mangle(backref) if self.templateParams else ""
        return super().mangle(backref) + template

class ConversionOperatorIdentifierNode(IdentifierNode):
    def __init__(self):
        super().__init__()
        self.targetType = None

class IntrinsicFunctionIdentifierNode(IdentifierNode):
    def __init__(self, id: IntrinsicFunctionKind):
        super().__init__()
        self.id = id
    
    def __str__(self):
        match self.id:
            case IntrinsicFunctionKind.New: o = "operator new"
            case IntrinsicFunctionKind.Delete: o = "operator delete"
            case IntrinsicFunctionKind.Assign: o = "operator="
            case IntrinsicFunctionKind.RightShift: o = "operator>>"
            case IntrinsicFunctionKind.LeftShift: o = "operator<<"
            case IntrinsicFunctionKind.LogicalNot: o = "operator!"
            case IntrinsicFunctionKind.Equals: o = "operator=="
            case IntrinsicFunctionKind.NotEquals: o = "operator!="
            case IntrinsicFunctionKind.ArraySubscript: o = "operator[]"
            case IntrinsicFunctionKind.Pointer: o = "operator->"
            case IntrinsicFunctionKind.Increment: o = "operator++"
            case IntrinsicFunctionKind.Decrement: o = "operator--"
            case IntrinsicFunctionKind.Minus: o = "operator-"
            case IntrinsicFunctionKind.Plus: o = "operator+"
            case IntrinsicFunctionKind.Dereference: o = "operator*"
            case IntrinsicFunctionKind.BitwiseAnd: o = "operator&"
            case IntrinsicFunctionKind.MemberPointer: o = "operator->*"
            case IntrinsicFunctionKind.Divide: o = "operator/"
            case IntrinsicFunctionKind.Modulus: o = "operator%"
            case IntrinsicFunctionKind.LessThan: o = "operator<"
            case IntrinsicFunctionKind.LessThanEqual: o = "operator<="
            case IntrinsicFunctionKind.GreaterThan: o = "operator>"
            case IntrinsicFunctionKind.GreaterThanEqual: o = "operator>="
            case IntrinsicFunctionKind.Comma: o = "operator,"
            case IntrinsicFunctionKind.Parens: o = "operator()"
            case IntrinsicFunctionKind.BitwiseNot: o = "operator~"
            case IntrinsicFunctionKind.BitwiseXor: o = "operator^"
            case IntrinsicFunctionKind.BitwiseOr: o = "operator|"
            case IntrinsicFunctionKind.LogicalAnd: o = "operator&&"
            case IntrinsicFunctionKind.LogicalOr: o = "operator||"
            case IntrinsicFunctionKind.TimesEqual: o = "operator*="
            case IntrinsicFunctionKind.PlusEqual: o = "operator+="
            case IntrinsicFunctionKind.MinusEqual: o = "operator-="
            case IntrinsicFunctionKind.DivEqual: o = "operator/="
            case IntrinsicFunctionKind.ModEqual: o = "operator%="
            case IntrinsicFunctionKind.RshEqual: o = "operator>>="
            case IntrinsicFunctionKind.LshEqual: o = "operator<<="
            case IntrinsicFunctionKind.BitwiseAndEqual: o = "operator&="
            case IntrinsicFunctionKind.BitwiseOrEqual: o = "operator|="
            case IntrinsicFunctionKind.BitwiseXorEqual: o = "operator^="
            case IntrinsicFunctionKind.VbaseDtor: o = "`vbase dtor'"
            case IntrinsicFunctionKind.VecDelDtor: o = "`vector deleting dtor'"
            case IntrinsicFunctionKind.DefaultCtorClosure: o = "`default ctor closure'"
            case IntrinsicFunctionKind.ScalarDelDtor: o = "`scalar deleting dtor'"
            case IntrinsicFunctionKind.VecCtorIter: o = "`vector ctor iterator'"
            case IntrinsicFunctionKind.VecDtorIter: o = "`vector dtor iterator'"
            case IntrinsicFunctionKind.VecVbaseCtorIter: o = "`vector vbase ctor iterator'"
            case IntrinsicFunctionKind.VdispMap: o = "`virtual displacement map'"
            case IntrinsicFunctionKind.EHVecCtorIter: o = "`eh vector ctor iterator'"
            case IntrinsicFunctionKind.EHVecDtorIter: o = "`eh vector dtor iterator'"
            case IntrinsicFunctionKind.EHVecVbaseCtorIter: o = "`eh vector vbase ctor iterator'"
            case IntrinsicFunctionKind.CopyCtorClosure: o = "`copy ctor closure'"
            case IntrinsicFunctionKind.LocalVftableCtorClosure: o = "`local vftable ctor closure'"
            case IntrinsicFunctionKind.ArrayNew: o = "operator new[]"
            case IntrinsicFunctionKind.ArrayDelete: o = "operator delete[]"
            case IntrinsicFunctionKind.ManVectorCtorIter: o = "`managed vector ctor iterator'"
            case IntrinsicFunctionKind.ManVectorDtorIter: o = "`managed vector dtor iterator'"
            case IntrinsicFunctionKind.EHVectorCopyCtorIter: o = "`EH vector copy ctor iterator'"
            case IntrinsicFunctionKind.EHVectorVbaseCopyCtorIter: o = "`EH vector vbase copy ctor iterator'"
            case IntrinsicFunctionKind.VectorCopyCtorIter: o = "`vector copy ctor iterator'"
            case IntrinsicFunctionKind.VectorVbaseCopyCtorIter: o = "`vector vbase copy constructor iterator'"
            case IntrinsicFunctionKind.ManVectorVbaseCopyCtorIter: o = "`managed vector vbase copy constructor iterator'"
            case IntrinsicFunctionKind.CoAwait: o = "operator co_await"
            case _: o = ""

        return o + self.getTemplateParams()

class StructorIdentifierNode(IdentifierNode):
    def __init__(self, isDtor: bool):
        super().__init__()
        self.struct = None
        self.isDtor = isDtor

    def __str__(self):
        o = ""
        if self.isDtor:
            o += "~"
        o += str(self.struct) + self.getTemplateParams()
        return o

class NodeArrayNode:
    def __init__(self):
        self.nodes: list[Node] = []

    def get(self, separator):
        if len(self.nodes) > 0:
            out = ""
            for node in self.nodes[:-1]:
                out += str(node) + separator
            out += str(self.nodes[-1])
            return out

    def __str__(self):
        return self.get(",")

    def mangle(self, backref = True, reverse = False):
        start = 0
        end = len(self.nodes)

        o = ""
        while start != end:
            i = end - start - 1 if reverse else start
            o += self.nodes[i].getMangled(backref)
            start += 1
        return o

class QualifiedNameNode:
    def __init__(self):
        self.components = NodeArrayNode()

    def __str__(self):
        return self.components.get("::") 

    def getUnqualifiedIdentifier(self):
        return self.components.nodes[-1]

    def mangle(self, backref = True):
        return self.components.mangle(backref, True)

class TemplateParameterReferenceNode(Node):
    def __init__(self):
        super().__init__()
        self.symbol = None
        self.thunkOffsets = [None] * 3
        self.affinity = PointerAffinity._None
        self.isMemberPointer = False

class IntegerLiteralNode(Node):
    def __init__(self, value = 0):
        super().__init__()
        self.value = value

class TypeNode(Node):
    def __init__(self):
        super().__init__()
        self.quals = Qualifiers._None

    def getPre(self): raise RuntimeError("TypeNode::getPre")
    def getPost(self): return ""

    def __str__(self):
        return self.getPre() + self.getPost()

class FunctionSignatureNode(TypeNode):
    class ThisAdjustor:
        StaticOffset = 0
        VBPtrOffset = 0
        VBOffsetOffset = 0
        VtordispOffset = 0

    def __init__(self):
        super().__init__()
        self.affinity = PointerAffinity._None
        self.callConv = CallingConv._None
        self.funcClass = FuncClass.Global
        self.refQual = FunctionRefQualifier._None
        self.retType: TypeNode = None
        self.isVariadic = False
        self.params: NodeArrayNode = None

        self.thisAdjust = None

    def getPre(self, noCallConv = False):
        o = ""
        if self.thisAdjust:
            o = "[thunk]: "

        if self.funcClass & FuncClass.Public:
            o += "public: "
        if self.funcClass & FuncClass.Protected:
            o += "protected: "
        if self.funcClass & FuncClass.Private:
            o += "private: "

        if not self.funcClass & FuncClass.Global and self.funcClass & FuncClass.Static:
            o += "static "
        if self.funcClass & FuncClass.ExternC:
            o += 'extern "C" '
        if self.funcClass & FuncClass.Virtual:
            o += "virtual "
        
        if self.retType:
            o += self.retType.getPre() + " "

        if not noCallConv:
            o += outputSpaceIfNecessary(o)
            o += outputCallingConvention(self.callConv)

        return o

    def getPost(self):
        o = ""
        if not self.funcClass & FuncClass.NoParameterList:
            o += f"({str(self.params)})"
        
        if self.quals & Qualifiers.Const: o += "const "
        if self.quals & Qualifiers.Volatile: o += " volatile"
        if self.quals & Qualifiers.Restrict: o += " __restrict"
        if self.quals & Qualifiers.Unaligned: o += " __unaligned"

        match self.refQual:
            case FunctionRefQualifier.Reference: o += " &"
            case FunctionRefQualifier.RValueReference: o += " &&"
        
        if self.retType:
            o += self.retType.getPost()

        return o

    def mangle(self, backref = True):
        ret = self.retType.getMangled(backref) if self.retType else ""
        return super().mangle(backref) + ret + self.params.mangle(backref)

class TagTypeNode(TypeNode):
    class Kind(Enum):
        Class = auto()
        Struct = auto()
        Union = auto()
        Enum = auto()

    def __init__(self, tag: Kind):
        super().__init__()
        self.qualName: QualifiedNameNode = None
        self.tag = tag

    def getPre(self):
        o = ""
        match self.tag:
            case self.Kind.Class: o = "class"
            case self.Kind.Struct: o = "struct"
            case self.Kind.Union: o = "union"
            case self.Kind.Enum: o = "enum"
        return f"{o} {self.qualName}{outputQualifiers(self.quals)}"

    def mangle(self, backref = True):
        return super().mangle(backref) + self.qualName.mangle(backref)

class PrimitiveTypeNode(TypeNode):
    class Kind(Enum):
        Void = auto()
        Bool = auto()
        Char = auto()
        Schar = auto()
        Uchar = auto()
        Char16 = auto()
        Char32 = auto()
        Short = auto()
        Ushort = auto()
        Int = auto()
        Uint = auto()
        Long = auto()
        Ulong = auto()
        Int64 = auto()
        Uint64 = auto()
        Wchar = auto()
        Float = auto()
        Double = auto()
        Ldouble = auto()
        Nullptr = auto()

    def __init__(self, kind: Kind):
        super().__init__()
        self.kind = kind

    def getPre(self):
        o = ""
        match self.kind:
            case self.Kind.Void: o = "void"
            case self.Kind.Bool: o = "bool"
            case self.Kind.Char: o = "char"
            case self.Kind.Schar: o = "signed char"
            case self.Kind.Uchar: o = "unsigned char"
            case self.Kind.Char16: o = "char16_t"
            case self.Kind.Char32: o = "char32_t"
            case self.Kind.Short: o = "short"
            case self.Kind.Ushort: o = "unsigned short"
            case self.Kind.Int: o = "int"
            case self.Kind.Uint: o = "unsigned int"
            case self.Kind.Long: o = "long"
            case self.Kind.Ulong: o = "unsigned long"
            case self.Kind.Int64: o = "__int64"
            case self.Kind.Uint64: o = "unsigned __int64"
            case self.Kind.Wchar: o = "wchar_t"
            case self.Kind.Float: o = "float"
            case self.Kind.Double: o = "double"
            case self.Kind.Ldouble: o = "long double"
            case self.Kind.Nullptr: o = "std::nullptr_t"
        return f"{o}{outputQualifiers(self.quals)}"

class ArrayTypeNode(TypeNode):
    pass

class PointerTypeNode(TypeNode):
    def __init__(self):
        super().__init__()
        self.affinity = PointerAffinity._None
        self.classParent = None
        self.pointee: TypeNode = None

    def getPre(self):
        o = ""
        if type(self.pointee) is FunctionSignatureNode:
            o += self.pointee.getPre(True)
        else:
            o += self.pointee.getPre()

        o += outputSpaceIfNecessary(o)

        if self.quals & Qualifiers.Unaligned:
            o += "__unaligned "

        if type(self.pointee) is ArrayTypeNode:
            o += "("
        elif type(self.pointee) is FunctionSignatureNode:
            o += "("
            o += outputCallingConvention(self.pointee.callConv)
            o += " "

        if self.classParent:
            o += str(self.classParent) + "::"
        
        match self.affinity:
            case PointerAffinity.Pointer: o += "*"
            case PointerAffinity.Reference: o += "&"
            case PointerAffinity.RValueReference: o += "&&"

        return o + outputQualifiers(self.quals, False)

    def getPost(self):
        o = ""
        if isinstance(self.pointee, ArrayTypeNode | FunctionSignatureNode):
            o += ")"
        o += self.pointee.getPost()
        return o

    def mangle(self, backref = True):
        return super().mangle(backref) + self.pointee.getMangled(backref)

class Symbol:
    def __init__(self, sig: FunctionSignatureNode, symbol: str):
        self.name: QualifiedNameNode = None
        self.sig = sig
        self.extra = ""
        self.symbol = symbol

    def __str__(self):
        o = str(self.sig.getPre())
        o += outputSpaceIfNecessary(o)
        o += str(self.name) + str(self.sig.getPost())
        return o

    def verify(self):
        built = self.mangle()
        if built != self.symbol:
            print(nw(self.symbol, built))

    def mangle(self, backref = True):
        return self.name.mangle(backref) + self.sig.getMangled(backref) + self.extra

class MSVCDemangler:
    def __init__(self):
        self.backrefs: list[Node] = []
        self.funcBackrefs: list[Node] = []

    def memorizeString(self, n: Node):
        if len(self.backrefs) < 10 and n not in self.backrefs:
            self.backrefs.append(n)

    def demangleBackRefName(self, mangled: StringView):
        i = ord(mangled.popFront()) - ord('0')
        if i >= len(self.backrefs):
            raise RuntimeError("demangleBackRefName")

        b = self.backrefs[i]
        n = NamedIdentifierNode(str(b))
        n.mangled = b.mangled
        n.backref = mangled.update()

        return n

    def demangleSpecialIntrinsic(self, mangled: StringView):
        sik = None
        for str, value in siks:
            if mangled.consumeFront(str):
                sik = value
        
        match sik:
            case None: return None

            case _: raise RuntimeError("demangleSpecialIntrinsic")

    def demangleSimpleName(self, mangled: StringView):
        s = mangled.popString('@')
        n = NamedIdentifierNode(s)
        n.update(mangled)
        self.memorizeString(n)
        return n

    def demangleFunctionIdentifierCode(self, mangled: StringView):
        n = None
        if mangled.consumeFront("__"):
            raise RuntimeError("demangleFunctionIdentifierCode DoubleUnder")
        elif mangled.consumeFront("_"):
            raise RuntimeError("demangleFunctionIdentifierCode Under")
        else:
            match mangled.popFront():
                case '0':
                    n = StructorIdentifierNode(False)
                case '1':
                    n = StructorIdentifierNode(True)
                case 'B':
                    raise RuntimeError("demangleFunctionIdentifierCode B")
                case _ as ch:
                    index = int(ch) if ch.isdigit() else ord(ch) - ord('A') + 10
                    n = IntrinsicFunctionIdentifierNode(basicIFKs[index])
        n.update(mangled)
        return n

    def demangleTemplateParameterList(self, mangled: StringView):
        na = NodeArrayNode()
        
        while not mangled.startsWith('@'):
            if mangled.consumeFront("$5") or mangled.consumeFront("$$V") \
                or mangled.consumeFront("$$$V") or mangled.consumeFront("$$Z"):
                continue

            n = None
            if mangled.consumeFront("$$Y"):
                n = self.demangleFullyQualifiedSymbolName(mangled)
            elif mangled.consumeFront("$$B"):
                n = self.demangleType(mangled, QualifierMangleMode.Drop)
            elif mangled.consumeFront("$$C"):
                n = self.demangleType(mangled, QualifierMangleMode.Mangle)
            elif mangled.consumeFront("$"):
                match mangled.popFront():
                    case 'J' | 'I' | 'H' | '1' as c:
                        n = TemplateParameterReferenceNode()

                        s = None
                        if mangled.startsWith('?'):
                            s = self.parse(mangled)
                            self.memorizeString(s)
                        
                        if c != '1':
                            i = 0
                            end = ord(c) - ord('H')
                            while i != end:
                                n.thunkOffsets[i] = self.demangleNumber(mangled)
                                i += 1
                        
                        n.affinity = PointerAffinity.Pointer
                        n.symbol = s
                        n.update(mangled)
                    case 'E' if mangled.startsWith('?'):
                        n = TemplateParameterReferenceNode()
                        n.affinity = PointerAffinity.Reference
                        n.symbol = self.parse(mangled)
                        n.update(mangled)
                    case 'F' | 'G' as c:
                        n = TemplateParameterReferenceNode()

                        i = 0
                        end = ord(c) - ord('F') + 1
                        while i != end:
                            n.thunkOffsets[i] = self.demangleNumber(mangled)
                            i += 1

                        n.isMemberPointer = True
                        n.update(mangled)
                    case '0':
                        n = IntegerLiteralNode(self.demangleNumber(mangled))
                        n.update(mangled)
                    
                    # workaround for this reimplementation
                    case _: mangled.pos -= 2
            
            if n is None:
                n = self.demangleType(mangled, QualifierMangleMode.Drop)

            na.nodes.append(n)

        if mangled.consumeFront('@'):
            return na

        raise RuntimeError("demangleTemplateParameterList")

    def demangleTemplateInstantiationName(self, mangled: StringView, template: bool):
        backrefs = self.backrefs
        funcBackrefs = self.funcBackrefs
        self.backrefs = []
        self.funcBackrefs = []
        
        identifier = self.demangleUnqualifiedSymbolName(mangled)
        identifier.templateParams = self.demangleTemplateParameterList(mangled)

        self.backrefs = backrefs
        self.funcBackrefs = funcBackrefs

        if template:
            self.memorizeString(identifier)

        return identifier

    def demangleUnqualifiedTypeName(self, mangled: StringView):
        if mangled.startsWithDigit():
            return self.demangleBackRefName(mangled)
        if mangled.consumeFront("?$"):
            return self.demangleTemplateInstantiationName(mangled, True)
        return self.demangleSimpleName(mangled)

    def demangleUnqualifiedSymbolName(self, mangled: StringView):
        if mangled.startsWithDigit():
            return self.demangleBackRefName(mangled)
        if mangled.consumeFront("?$"):
            return self.demangleTemplateInstantiationName(mangled, False)
        if mangled.consumeFront('?'):
            return self.demangleFunctionIdentifierCode(mangled)
        return self.demangleSimpleName(mangled)

    def startsWithLocalScopePattern(self, mangled: StringView):
        if not mangled.consumeFront('?') or mangled.size() < 2:
            return False

        raise RuntimeError("startsWithLocalScopePattern")
        return True

    def demangleNameScopePiece(self, mangled: StringView):
        if mangled.startsWithDigit():
            return self.demangleBackRefName(mangled)
        if mangled.consumeFront("?$"):
            return self.demangleTemplateInstantiationName(mangled, True)
        if mangled.consumeFront("?A"):
            raise RuntimeError("demangleAnonymousNamespaceName")
            return None
        if self.startsWithLocalScopePattern(mangled.copy()):
            raise RuntimeError("demangleLocallyScopedNamePiece")
            return None
        return self.demangleSimpleName(mangled)

    def demangleNameScopeChain(self, mangled: StringView, idNode: IdentifierNode):
        qn = QualifiedNameNode()
        nodes = qn.components.nodes
        nodes.append(idNode)

        while not mangled.consumeFront('@'):
            if mangled.size() == 0:
                raise RuntimeError("demangleNameScopeChain")

            nodes.insert(0, self.demangleNameScopePiece(mangled))

        nodes[0].update(mangled)

        return qn

    def demangleFullyQualifiedTypeName(self, mangled: StringView):
        id = self.demangleUnqualifiedTypeName(mangled)
        qn = self.demangleNameScopeChain(mangled, id)

        return qn

    def demangleFullyQualifiedSymbolName(self, mangled: StringView):
        id = self.demangleUnqualifiedSymbolName(mangled)
        qn = self.demangleNameScopeChain(mangled, id)

        if type(id) is StructorIdentifierNode:
            id.struct = qn.components.nodes[-2]

        return qn

    def demangleFunctionClass(self, mangled: StringView):
        match mangled.popFront():
            case '9': return FuncClass.ExternC | FuncClass.NoParameterList
            case 'A': return FuncClass.Private
            case 'B': return FuncClass.Private | FuncClass.Far
            case 'C': return FuncClass.Private | FuncClass.Static
            case 'D': return FuncClass.Private | FuncClass.Static
            case 'E': return FuncClass.Private | FuncClass.Virtual
            case 'F': return FuncClass.Private | FuncClass.Virtual
            case 'G': return FuncClass.Private | FuncClass.StaticThisAdjust
            case 'H': return FuncClass.Private | FuncClass.StaticThisAdjust | FuncClass.Far
            case 'I': return FuncClass.Protected
            case 'J': return FuncClass.Protected | FuncClass.Far
            case 'K': return FuncClass.Protected | FuncClass.Static
            case 'L': return FuncClass.Protected | FuncClass.Static | FuncClass.Far
            case 'M': return FuncClass.Protected | FuncClass.Virtual
            case 'N': return FuncClass.Protected | FuncClass.Virtual | FuncClass.Far
            case 'O': return FuncClass.Protected | FuncClass.Virtual | FuncClass.StaticThisAdjust
            case 'P': return FuncClass.Protected | FuncClass.Virtual | FuncClass.StaticThisAdjust | FuncClass.Far
            case 'Q': return FuncClass.Public
            case 'R': return FuncClass.Public | FuncClass.Far
            case 'S': return FuncClass.Public | FuncClass.Static
            case 'T': return FuncClass.Public | FuncClass.Static | FuncClass.Far
            case 'U': return FuncClass.Public | FuncClass.Virtual
            case 'V': return FuncClass.Public | FuncClass.Virtual | FuncClass.Far
            case 'W': return FuncClass.Public | FuncClass.Virtual | FuncClass.StaticThisAdjust
            case 'X': return FuncClass.Public | FuncClass.Virtual | FuncClass.StaticThisAdjust | FuncClass.Far
            case 'Y': return FuncClass.Global
            case 'Z': return FuncClass.Global | FuncClass.Far
            case '$':
                VFlag = FuncClass.VirtualThisAdjust
                if mangled.consumeFront('R'):
                    VFlag = VFlag | FuncClass.VirtualThisAdjustEx

                match mangled.popFront():
                    case '0': return FuncClass.Private | FuncClass.Virtual | VFlag
                    case '1': return FuncClass.Private | FuncClass.Virtual | VFlag | FuncClass.Far
                    case '2': return FuncClass.Protected | FuncClass.Virtual | VFlag
                    case '3': return FuncClass.Protected | FuncClass.Virtual | VFlag | FuncClass.Far
                    case '4': return FuncClass.Public | FuncClass.Virtual | VFlag
                    case '5': return FuncClass.Public | FuncClass.Virtual | VFlag | FuncClass.Far
            
            case _ as c: raise RuntimeError("demangleFunctionClass " + c)

    def demangleNumber(self, mangled: StringView):
        neg = -1 * mangled.consumeFront('?')

        if mangled.startsWithDigit():
            return neg * (int(mangled.popFront()) + 1)

        ret = 0
        while mangled.size() != 0:
            match mangled.popFront():
                case '@':
                    return neg * ret
                case c if 'A' <= c <= 'P':
                    ret = (ret << 4) + (ord(c) - ord('A'))
                case _: break

        raise RuntimeError("demangleNumber")

    def demanglePointerExtQualifiers(self, mangled: StringView):
        quals = Qualifiers._None
        if mangled.consumeFront('E'):
            quals = quals | Qualifiers.Pointer64
        if mangled.consumeFront('I'):
            quals = quals | Qualifiers.Restrict
        if mangled.consumeFront('F'):
            quals = quals | Qualifiers.Unaligned
        return quals

    def demangleQualifiers(self, mangled: StringView):
        match mangled.popFront():
            case 'Q': return Qualifiers._None
            case 'R': return Qualifiers.Const
            case 'S': return Qualifiers.Volatile
            case 'T': return Qualifiers.Const | Qualifiers.Volatile
            case 'A': return Qualifiers._None
            case 'B': return Qualifiers.Const
            case 'C': return Qualifiers.Volatile
            case 'D': return Qualifiers.Const | Qualifiers.Volatile
            case _ as c: raise RuntimeError("demangleQualifiers " + c)

    def demangleCallingConvention(self, mangled: StringView):
        match mangled.popFront():
            case 'A' | 'B': return CallingConv.Cdecl
            case 'C' | 'D': return CallingConv.Pascal
            case 'E' | 'F': return CallingConv.Thiscall
            case 'G' | 'H': return CallingConv.Stdcall
            case 'I' | 'J': return CallingConv.Fastcall
            case 'M' | 'N': return CallingConv.Clrcall
            case 'O' | 'P': return CallingConv.Eabi
            case 'Q': return CallingConv.Vectorcall
            case _: return CallingConv._None

    def demanglePrimitiveType(self, mangled: StringView):
        n = None
        if mangled.consumeFront("$$T"):
            n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Nullptr)

        match mangled.popFront():
            case 'X': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Void)
            case 'D': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Char)
            case 'C': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Schar)
            case 'E': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Uchar)
            case 'F': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Short)
            case 'G': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Ushort)
            case 'H': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Int)
            case 'I': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Uint)
            case 'J': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Long)
            case 'K': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Ulong)
            case 'M': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Float)
            case 'N': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Double)
            case 'O': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Ldouble)
            case '_' if mangled.size() != 0:
                match mangled.popFront():
                    case 'N': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Bool)
                    case 'J': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Int64)
                    case 'K': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Uint64)
                    case 'W': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Wchar)
                    case 'S': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Char16)
                    case 'U': n = PrimitiveTypeNode(PrimitiveTypeNode.Kind.Char32)
            case _ as c: raise RuntimeError("demanglePrimitiveType" + c)
        n.update(mangled)
        return n

    def isMemberPointer(self, mangled: StringView):
        match mangled.popFront():
            case '$' | 'A':
                return False
            case 'P' | 'Q' | 'R' | 'S':
                pass
            case _:
                raise RuntimeError("isMemberPointer Ty is not a pointer type")
        
        if mangled.startsWithDigit():
            return mangled.front() == '8'

        mangled.consumeFront('E')
        mangled.consumeFront('I')
        mangled.consumeFront('F')

        if not mangled:
            raise RuntimeError("isMemberPointer mangled")

        match mangled.front():
            case 'A' | 'B' | 'C' | 'D': return False
            case 'Q' | 'R' | 'S' | 'T': return True
            case _ as c: raise RuntimeError("isMemberPointer" + c)

    def demanglePointerType(self, mangled: StringView):
        n = PointerTypeNode()
        n.affinity = PointerAffinity.Pointer
        match mangled.popFront():
            case 'P': n.quals = Qualifiers._None
            case 'Q': n.quals = Qualifiers.Const
            case 'R': n.quals = Qualifiers.Volatile
            case 'S': n.quals = Qualifiers.Const | Qualifiers.Volatile
            case 'A': n.affinity = PointerAffinity.Reference
            case '$' if mangled.consumeFront("$Q"): n.affinity = PointerAffinity.RValueReference
            case _: raise RuntimeError("demanglePointerType")

        n.quals = self.demanglePointerExtQualifiers(mangled)
        
        n.update(mangled)

        if mangled.consumeFront('6'):
            n.pointee = self.demangleFunctionType(mangled, False)
        else:
            n.pointee = self.demangleType(mangled, QualifierMangleMode.Mangle)

        return n

    def demangleMemberPointerType(self, mangled: StringView):
        n = PointerTypeNode()
        n.affinity = PointerAffinity.Pointer
        match mangled.popFront():
            case 'P': n.quals = Qualifiers._None
            case 'Q': n.quals = Qualifiers.Const
            case 'R': n.quals = Qualifiers.Volatile
            case 'S': n.quals = Qualifiers.Const | Qualifiers.Volatile
            case _: raise RuntimeError("demangleMemberPointerType")

        n.quals = n.quals | self.demanglePointerExtQualifiers(mangled)

        if mangled.consumeFront('8'):
            n.classParent = self.demangleFullyQualifiedSymbolName(mangled)
            n.pointee = self.demangleFunctionType(mangled, True)
        else:
            quals = self.demangleQualifiers(mangled)
            n.classParent = self.demangleFullyQualifiedTypeName(mangled)
            n.pointee = self.demangleType(mangled, QualifierMangleMode.Drop)
            n.pointee.quals = quals

        return n

    def demangleType(self, mangled: StringView, qmm: QualifierMangleMode):
        quals = Qualifiers._None
        if qmm == QualifierMangleMode.Mangle:
            quals = self.demangleQualifiers(mangled)
        elif qmm == QualifierMangleMode.Result:
            if mangled.consumeFront('?'):
                quals = self.demangleQualifiers(mangled)

        ty = None 
        c = mangled.front()
        # Tag 
        if c in ('T', 'U', 'V', 'W'):
            match mangled.popFront():
                case 'T': ty = TagTypeNode(TagTypeNode.Kind.Union)
                case 'U': ty = TagTypeNode(TagTypeNode.Kind.Struct)
                case 'V': ty = TagTypeNode(TagTypeNode.Kind.Class)
                case 'W' if mangled.popFront() == '4':
                    ty = TagTypeNode(TagTypeNode.Kind.Enum)
                case _ as c: raise RuntimeError("demangleClassType " + c)
            ty.update(mangled)
            ty.qualName = self.demangleFullyQualifiedTypeName(mangled)

        # Pointer
        elif c in ('A', 'P', 'Q', 'R', 'S') or mangled.startsWith("$$Q"):
            if self.isMemberPointer(mangled.copy()):
                ty = self.demangleMemberPointerType(mangled)
            else:
                ty = self.demanglePointerType(mangled)
        # Array
        elif c == 'Y':
            raise RuntimeError("demangleArrayType")

        # Function
        elif mangled.consumeFront("$$A8@@"):
            ty = self.demangleFunctionType(mangled, True)
        elif mangled.consumeFront("$$A6"):
            ty = self.demangleFunctionType(mangled, False)

        # Custom Type
        elif c == '?':
            raise RuntimeError("demangleCustomType")

        else:
            ty = self.demanglePrimitiveType(mangled)

        ty.quals = ty.quals | quals
        return ty

    def demangleFunctionParameterList(self, mangled: StringView):
        if mangled.consumeFront('X'):
            n = NamedIdentifierNode("void")
            n.update(mangled)
            return n

        na = NodeArrayNode()
        while not mangled.startsWith('@') and not mangled.startsWith('Z'):
            if mangled.startsWithDigit():
                i = ord(mangled.popFront()) - ord('0')
                if i >= len(self.funcBackrefs):
                    raise RuntimeError("demangleFuncBackRefName")

                n = deepcopy(self.funcBackrefs[i])
                n.backref = mangled.update()
                na.nodes.append(n)
            else:
                old = mangled.size()
                tn = self.demangleType(mangled, QualifierMangleMode.Drop)
                
                na.nodes.append(tn)

                if len(self.funcBackrefs) <= 9 and (old - mangled.size()) > 1:
                    self.funcBackrefs.append(tn)                
        
        if mangled.consumeFront('@') or mangled.consumeFront('Z'):
            return na

        return None

    def demangleFunctionType(self, mangled: StringView, hasThisQuals: bool):
        fsn = FunctionSignatureNode()

        if hasThisQuals:
            fsn.quals = self.demanglePointerExtQualifiers(mangled)

            if mangled.consumeFront('G'):
                fsn.refQual = FunctionRefQualifier.Reference
            elif mangled.consumeFront('H'):
                fsn.refQual = FunctionRefQualifier.RValueReference

            fsn.quals = fsn.quals | self.demangleQualifiers(mangled)

        fsn.callConv = self.demangleCallingConvention(mangled)
        fsn.update(mangled)

        if not mangled.consumeFront('@'):
            fsn.retType = self.demangleType(mangled, QualifierMangleMode.Result)

        fsn.params = self.demangleFunctionParameterList(mangled)

        if mangled.consumeFront('Z'):
            return fsn

        # Throw
        return None

    def demangleFunctionEncoding(self, mangled: StringView):
        fc = FuncClass._None
        if mangled.consumeFront("$$J0"):
            fc = FuncClass.ExternC

        fc = fc | self.demangleFunctionClass(mangled)

        thisAdjust = None
        if fc & FuncClass.StaticThisAdjust:
            thisAdjust = FunctionSignatureNode.ThisAdjustor()
            thisAdjust.StaticOffset = self.demangleNumber(mangled)
        elif fc & FuncClass.VirtualThisAdjust:
            thisAdjust = FunctionSignatureNode.ThisAdjustor()
            if fc & FuncClass.VirtualThisAdjustEx:
                thisAdjust.VBPtrOffset = self.demangleNumber(mangled)
                thisAdjust.VBOffsetOffset = self.demangleNumber(mangled)
            thisAdjust.VtordispOffset = self.demangleNumber(mangled)
            thisAdjust.StaticOffset = self.demangleNumber(mangled)

        if fc & FuncClass.NoParameterList:
            fsn = FunctionSignatureNode()
            fsn.update()
        else:
            fsn = self.demangleFunctionType(mangled, not (fc & (FuncClass.Global | FuncClass.Static)))
        fsn.funcClass = fc

        if thisAdjust is not None:
            fsn.thisAdjust = thisAdjust
        
        return Symbol(fsn, mangled.view) 

    def demangleEncodedSymbol(self, mangled: StringView, qn: QualifiedNameNode):
        match mangled.front():
            case '0' | '1' | '2' | '3' | '4' | '8':
                return None
        
        fsn = self.demangleFunctionEncoding(mangled)
        fsn.name = qn
        fsn.extra = mangled.update()

        uqn = qn.getUnqualifiedIdentifier()
        if type(uqn) is ConversionOperatorIdentifierNode:
            uqn.targetType = fsn.sig.retType

        return fsn

    def parse(self, mangled_str: str):
        mangled = StringView(mangled_str)

        if not mangled.consumeFront('?'):
            raise RuntimeError("parse")
        
        si = self.demangleSpecialIntrinsic(mangled)
        if si != None:
            return si

        qn = self.demangleFullyQualifiedSymbolName(mangled)
        
        symbol = self.demangleEncodedSymbol(mangled, qn)
        if symbol is None:
            raise RuntimeError(f"Error parsing mangled symbol {mangled_str}")
        
        return symbol
