
#define SYM_PTR 1
#define SYM_CALL 2

typedef struct s_symbols
{
	STRUCT_HEADER_LIST(t_symbols);
    char *name;
    int type;
    char *data;
} t_symbols;

// INSERT_DECLARATIONS

unsigned int SymbolAdd ( const char *name, const int type, const char *data ) { return __symbols_add ( GetWorkStage(0), name, type, data ); }
unsigned int SymbolGetInt ( const char *name ) { return __symbols_get_address ( GetWorkStage(0), name ); }
