/* $XConsortium: Types.h /main/3 1996/06/11 17:09:46 cde-hal $ */
#ifndef _Types_h
#define _Types_h

#define true  1
#define false 0

class VariableTable;
class SymbolTable;
class ResolverStackElement;
class Renderer;

extern VariableTable *gVariableTable;
extern SymbolTable   *gSymTab;
extern SymbolTable   *gElemSymTab;
extern Renderer     *gRenderer;

#endif /* _Types_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
