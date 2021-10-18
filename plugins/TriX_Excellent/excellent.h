
unsigned int excellent_init ( );
char *excellent_generate ( t_workspace *ws, unsigned int pos, int width, int x_count, int y_count );


#define DOC_START "\
<?xml version=\"1.0\"?>\
<?mso-application progid=\"Excel.Sheet\"?>\
<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\
 xmlns:o=\"urn:schemas-microsoft-com:office:office\"\
 xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\
 xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\
 xmlns:html=\"http://www.w3.org/TR/REC-html40\">\
 <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\
  <Author>g3gg0</Author>\
  <LastAuthor>g3gg0</LastAuthor>\
  <Created>2006-12-26T01:09:20Z</Created>\
  <LastSaved>2006-12-26T01:15:13Z</LastSaved>\
  <Version>11.6568</Version>\
 </DocumentProperties>\
 <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\
  <WindowHeight>12705</WindowHeight>\
  <WindowWidth>15180</WindowWidth>\
  <WindowTopX>480</WindowTopX>\
  <WindowTopY>120</WindowTopY>\
  <ProtectStructure>False</ProtectStructure>\
  <ProtectWindows>False</ProtectWindows>\
 </ExcelWorkbook>\n\
 <Styles>\n\
  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n\
   <Alignment ss:Vertical=\"Bottom\"/>\n\
   <Borders/>\n\
   <Font/>\n\
   <Interior/>\n\
   <NumberFormat/>\n\
   <Protection/>\n\
  </Style>\n\
"
#define DOC_STYLE_START "\n\
  <Style ss:ID=\"s%i\">\n\
	<Interior ss:Color=\"#"

#define DOC_STYLE_END "\
\" ss:Pattern=\"Solid\"/>\n\
  </Style>\n"

#define DOC_START2 "\n\
 </Styles>\n\
 <Worksheet ss:Name=\"Tabelle1\">\n\
  <Table ss:ExpandedColumnCount=\"1025\" ss:ExpandedRowCount=\"1025\" x:FullColumns=\"1\"\n\
   x:FullRows=\"1\" ss:DefaultColumnWidth=\"30\">\n"

#define DOC_END "\
  </Table>\
  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\
   <PageSetup>\
    <Header x:Margin=\"0.4921259845\"/>\
    <Footer x:Margin=\"0.4921259845\"/>\
    <PageMargins x:Bottom=\"0.984251969\" x:Left=\"0.78740157499999996\"\
     x:Right=\"0.78740157499999996\" x:Top=\"0.984251969\"/>\
   </PageSetup>\
   <Selected/>\
   <Panes>\
    <Pane>\
     <Number>3</Number>\
     <ActiveRow>4</ActiveRow>\
    </Pane>\
   </Panes>\
   <ProtectObjects>False</ProtectObjects>\
   <ProtectScenarios>False</ProtectScenarios>\
  </WorksheetOptions>\
 </Worksheet>\
</Workbook>"

#define CELL_START_COLOR "<Cell ss:StyleID=\"s%i\">\n<Data ss:Type=\"%s\">"
#define CELL_START "<Cell>\n<Data ss:Type=\"%s\">"
#define CELL_END   "</Data></Cell>\n"
#define ROW_START  "<Row>\n"
#define ROW_END    "</Row>\n"

#define EXCELLENT_HEADERS \
	"import char *__excellent_generate ( t_workspace *ws, unsigned int pos, int width, int x_count, int y_count );\n"\
	"import unsigned int __excellent_start_doc ( char *buffer );\n"\
	"import unsigned int __excellent_end_doc ( char *buffer );\n"\
	"import unsigned int __excellent_start_cell ( char *buffer, char *type, int color );\n"\
	"import unsigned int __excellent_start_row ( char *buffer );\n"\
	"import unsigned int __excellent_end_cell ( char *buffer );\n"\
	"import unsigned int __excellent_end_row ( char *buffer );\n"\
	"import char *__excellent_create_doc (  );\n"\


unsigned int excellent_start_doc ( char *buffer );
unsigned int excellent_end_doc ( char *buffer );
unsigned int excellent_start_cell ( char *buffer, char *type, int color );
unsigned int excellent_start_row ( char *buffer );
unsigned int excellent_end_cell ( char *buffer );
unsigned int excellent_end_row ( char *buffer );
char *excellent_create_doc (  );

#define EXCELLENT_SYMBOLS \
	REGISTER_SYMBOL ( excellent_generate );\
	REGISTER_SYMBOL ( excellent_start_doc );\
	REGISTER_SYMBOL ( excellent_end_doc );\
	REGISTER_SYMBOL ( excellent_start_cell );\
	REGISTER_SYMBOL ( excellent_start_row );\
	REGISTER_SYMBOL ( excellent_end_cell );\
	REGISTER_SYMBOL ( excellent_end_row );\
	REGISTER_SYMBOL ( excellent_create_doc );\

