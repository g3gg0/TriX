

// INSERT_DECLARATIONS
unsigned int NokiaCreateFunc ( t_function2 *func, unsigned int offset ) { return __nokia_create_func2 ( GetWorkspace(), func, offset, 0 ); }
unsigned int NokiaCreateFuncRebased ( t_function2 *func, unsigned int offset, unsigned int base ) { return __nokia_create_func2 ( GetWorkspace(), func, offset, base ); }

