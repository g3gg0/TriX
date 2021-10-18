#!/bin/bash

FILES="trixplug segment stage file_io file workspace util elf crypto trixcrypt netrix treenode ui fmt symbols nokia options arm seer project mutex mem"
SEER_REGISTER_AUTO="../include/seer_auto.h"
OUTPATH="../include"
TRIXPLUG_HEADER="../include/trixplug_funcs.h"
TRIXPLUG_WRAPPER="../include/trixplug_wrapper.h"
TRIXPLUG_VERSION="../include/trixplug_version.h"

register=""
imports=""
mainversion=`cat header_definition/version | gawk "{ print \\$1 }"`
subversion=`cat header_definition/version | gawk "{ print \\$2 }"`

echo "no" > header_definition/update_mainversion
echo "no" > header_definition/update_subversion


echo "" > $TRIXPLUG_HEADER
echo "/* trixplug function structs */" >> $TRIXPLUG_HEADER
echo "" >> $TRIXPLUG_HEADER
echo "struct trix_functions" >> $TRIXPLUG_HEADER
echo "{" >> $TRIXPLUG_HEADER
echo "    int version;" >> $TRIXPLUG_HEADER
echo "    int (*debug_msg) ( int level, char *str, ... );" >> $TRIXPLUG_HEADER
echo "    int (*error_msg) ( int level, char *str, ... );" >> $TRIXPLUG_HEADER
echo "    int (*print_msg) ( char *str, ... );" >> $TRIXPLUG_HEADER

echo "#ifdef TRIXPLUG_MODE" > $TRIXPLUG_WRAPPER
echo "" >> $TRIXPLUG_WRAPPER

for file in $FILES; do
    header=$OUTPATH/auto_${file}.h
    trixplug_include=$OUTPATH/trixplug_${file}.h
    import_string=""
    register_string=""
    plug_string=""
    pluginit_string=""

    # convert filename to upper case for #defines
    ufile=`echo $file | tr "[a-z]" "[A-Z]"`
    
    # get just the function declarations
    if [ ! -f ../src/$file.c ]; then
	if [ -f ../src/seer/$file.c ]; then
	    data=`cat ../src/seer/$file.c | dos2unix  | indent -l999 -npsl -nut -fc1 | sed "s/\t/ /g" | grep -v -e "^ " -e "^$" -e "{" -e "}" -e ";" -e "^#" -e "^\/\*" -e "^\*\/" -e "^/" -e "^struct" -e "^typedef" -i -e "inline"`
	fi
	if [ -f ../src/crypto/$file.c ]; then
	    data=`cat ../src/crypto/$file.c | dos2unix  | indent -l999 -npsl -nut -fc1 | sed "s/\t/ /g" | grep -v -e "^ " -e "^$" -e "{" -e "}" -e ";" -e "^#" -e "^\/\*" -e "^\*\/" -e "^/" -e "^struct" -e "^typedef" -i -e "inline"`
	fi
	if [ "x$data" == "x" ]; then
	    echo "../src/$file does not exist!"
	    exit
	fi
    else
	data=`cat ../src/$file.c | dos2unix  | indent -l999 -npsl -nut -fc1 | sed "s/\t/ /g" | grep -v -e "^ " -e "^$" -e "{" -e "}" -e ";" -e "^#" -e "^\/\*" -e "^\*\/" -e "^/" -e "^struct" -e "^typedef" -i -e "inline"`
    fi
    
    echo "Processing:  $file.c"
    
    plug_string=""
    pluginit_string=""
    trixplug_registered=""
    
    echo -e "$data" | 
    (
	while read line; do
	    # split up function declaration
	    pre=`echo "$line" | gawk --field-separator=\( "{ print \\$1 }"`;
	    post=`echo "$line" | gawk --field-separator=\( "{ print \\$2 }"`;
	    funcname=`echo "$pre" | gawk "{ print \\$NF }" | sed "s/\*//"`;

	    # rename e.g. stage_add to __stage_add
	    new_funcname=`echo "__$funcname"`; #| sed "s/${file}_/__${file}_/"`;
	    new_pre=`echo "$pre" | sed "s/$funcname $/$new_funcname/"`;
	    
	    # rename e.g. stage_add to add for trixplug
	    plug_funcname=`echo "$funcname" | sed "s/^${file}_//"`;
	    plug_pre=`echo "$pre" | sed "s/$funcname $/(\*$plug_funcname)/"`;
	    

	    # create import etc strings from that stuff
	    declaration_string="${declaration_string}void $funcname ();\n"
	    import_string="$import_string\t\"import $new_pre ( $post;\\\n\"\\\\\n"
	    import_string_html="${import_string_html}import $new_pre ( $post;\n"
	    register_string="$register_string\tscAddExtSymInt ( $funcname );\\\\\n"

	    # create trixplug string
	    plug_string="$plug_string\t$plug_pre ( $post;\n"

	    # create trixplug init
	    pluginit_string="$pluginit_string\tft->$file->$plug_funcname = $funcname;\\\\\n"
	    
	    # create trixplug wrappers
	    plug_struct_wrapper="$plug_struct_wrapper#define $funcname ft->$file->$plug_funcname\n"
	    
	done
	
	if [ ! -e header_definition/$file.def ]; then
	    echo "" > header_definition/$file.def
	fi
	
        echo -e "$plug_string" | grep -v "^\$" | sort -b > header_definition/$file.def.new
	cat header_definition/$file.def | grep -v "^\$" | sort -b > header_definition/$file.def.tmp

	added_diff=`diff -EBbwd header_definition/$file.def.tmp header_definition/$file.def.new | grep "^>" | sed "s/^>//"`
	removed_diff=`diff -EBbwd header_definition/$file.def.tmp header_definition/$file.def.new | grep "^<" | sed "s/^<//"`


	if [ "x$removed_diff" != "x" ]; then
	    echo "  changed/removed:"
	    echo "$removed_diff"
	    echo ""
		if [ "x$added_diff" != "x" ]; then
		echo "  added:"
		echo "$added_diff"
	    fi
	    echo "yes" > header_definition/update_mainversion
	    echo "   => update mainversion"
    	    echo -e "$plug_string" | grep -v "^\$" | sort -b > header_definition/$file.def
	else
    	    if [ "x$added_diff" != "x" ]; then
    		echo "  added:"
		echo "$added_diff"
		echo "yes" > header_definition/update_subversion
		echo "   => update subversion"
    		echo -e "$added_diff" >> header_definition/$file.def
	    fi
	fi
	rm header_definition/$file.def.tmp
	rm header_definition/$file.def.new

	plug_string=`cat header_definition/$file.def`

	
	if [ "x$file" != "xtrixplug" ]; then

	echo "" > $header
	echo "" >> $header
        echo "/* seer headers for $file.c */" >> $header
        echo "" >> $header
	echo "#define ${ufile}_AUTO_IMPORTS \\" >> $header
        echo -e "$import_string" >> $header
        echo "" >> $header

        echo "" >> $header
	echo "#define ${ufile}_AUTO_IMPORTS_HTML \\" >> $header
        echo -e "$import_string_html" | ./c2html | sed "s/\"/\\\\\"/g;s/^/\t\"/;s/$/\"\\\/;"  >> $header
        echo "" >> $header

	echo "/* seer function registration for $file.c */" >> $header
        echo "" >> $header
        echo "#define ${ufile}_AUTO_REGISTER \\" >> $header
        echo -e "$register_string" >> $header
	echo "" >> $header

	echo "/* seer function declaration for $file.c */" >> $header
        echo "#ifndef HEADER_SKIP_DECLARATION" >> $header
        echo -e "$declaration_string" >> $header
	echo "#endif" >> $header
	
	fi
	
	echo "" > $trixplug_include
	echo "#ifndef __TRIX_TRIXPLUG_${ufile}_H__" >> $trixplug_include
	echo "#define __TRIX_TRIXPLUG_${ufile}_H__" >> $trixplug_include
	echo "" >> $trixplug_include
	echo "/* trixplug struct */" >> $trixplug_include
	echo "" >> $trixplug_include
	echo "struct ${file}_funcs" >> $trixplug_include
	echo "{" >> $trixplug_include
        echo -e "$plug_string" >> $trixplug_include
	echo "};" >> $trixplug_include

	echo "" >> $trixplug_include
	echo "/* trixplug struct initializer */" >> $trixplug_include
	echo "" >> $trixplug_include
	echo "#define ${ufile}_PLUG_INIT \\" >> $trixplug_include
	echo "extern struct trix_functions *ft;\\" >> $trixplug_include
	echo "struct ${file}_funcs ${file}_functions;\\" >> $trixplug_include
	echo "unsigned int ${file}_plug_init ( ) \\" >> $trixplug_include
	echo "{\\" >> $trixplug_include
	echo "	if ( !ft )\\" >> $trixplug_include
	echo "		return E_FAIL;\\" >> $trixplug_include
	echo "	ft->$file = &${file}_functions;\\" >> $trixplug_include
        echo -e "$pluginit_string\\" >> $trixplug_include
	echo "	return E_OK;\\" >> $trixplug_include
	echo "}" >> $trixplug_include
	echo "" >> $trixplug_include
	echo "#endif" >> $trixplug_include
	

        echo -e "$plug_struct_wrapper" >> $TRIXPLUG_WRAPPER
	
	echo "#ifdef __TRIX_TRIXPLUG_${ufile}_H__" >> $TRIXPLUG_HEADER
	echo "#define ${ufile}_FUNC_TYPE struct ${file}_funcs *" >> $TRIXPLUG_HEADER
	echo "#else" >> $TRIXPLUG_HEADER
	echo "#define ${ufile}_FUNC_TYPE void *" >> $TRIXPLUG_HEADER
	echo "#endif" >> $TRIXPLUG_HEADER
	echo "	${ufile}_FUNC_TYPE	 $file;" >> $TRIXPLUG_HEADER
    )
    
    if [ "x$file" != "xtrixplug" ]; then
    # now add the special header
    if [ -f $OUTPATH/${file}_seer.h ]; then
	echo "/* additional seer includes from ${file}_seer.h */" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_PRE \\" >> $header
	cat $OUTPATH/${file}_seer.h | sed '1,/^\/\/ INSERT_DECLARATIONS/p;d' | sed 's/\/\/ INSERT_DECLARATIONS//' | sed "s/\"/\\\\\"/g;s/^/\t\"/;s/$/\\\\n\"\\\/;" >> $header
	echo "" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_POST \\" >> $header
	cat $OUTPATH/${file}_seer.h | sed '1,/^\/\/ INSERT_DECLARATIONS/d' | sed "s/\"/\\\\\"/g;s/^/\t\"/;s/$/\\\\n\"\\\/;" >> $header
	echo "" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_PRE_HTML \\" >> $header
	cat $OUTPATH/${file}_seer.h | sed '1,/^\/\/ INSERT_DECLARATIONS/p;d' | sed 's/\/\/ INSERT_DECLARATIONS//' | ./c2html |  sed "s/\"/\\\\\"/g;s/^/\t\"/;s/$/\"\\\/;" >> $header
	echo "" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_POST_HTML \\" >> $header
	cat $OUTPATH/${file}_seer.h | sed '1,/^\/\/ INSERT_DECLARATIONS/d' | ./c2html | sed "s/\"/\\\\\"/g;s/^/\t\"/;s/$/\"\\\/;" >> $header
	echo "" >> $header
	echo "" >> $header
    else
	echo "/* no ${file}_seer.h */" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_PRE" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_POST" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_PRE_HTML" >> $header
	echo "" >> $header
	echo "#define ${ufile}_AUTO_MISC_POST_HTML" >> $header
	echo "" >> $header
    fi
    
    register="$register\t${ufile}_AUTO_REGISTER \\\\\n";
    imports="$imports\t${ufile}_AUTO_MISC_PRE \\\\\n";
    imports="$imports\t${ufile}_AUTO_IMPORTS \\\\\n";
    imports="$imports\t${ufile}_AUTO_MISC_POST \\\\\n";
    headers="$headers#include \"auto_${file}.h\"\n"
    
    fi
    trixplug_inits="$trixplug_inits\t${file}_plug_init ();\\\\\n"
done

echo -e "$headers" > $SEER_REGISTER_AUTO
echo -e "#define TRIX_AUTO_IMPORT \\\\\n$imports" >> $SEER_REGISTER_AUTO
echo "" >> $SEER_REGISTER_AUTO
echo -e "#define TRIX_AUTO_REGISTER \\\\\n$register" >> $SEER_REGISTER_AUTO
echo "" >> $SEER_REGISTER_AUTO
echo "" >> $SEER_REGISTER_AUTO

echo -n "#define HEADER_DEFINE(x) char *x[] = { " >> $SEER_REGISTER_AUTO
for file in $FILES; do
    ufile=`echo $file | tr "[a-z]" "[A-Z]"`

    if [ "x$file" != "xtrixplug" ]; then

    # Filename
    echo -n "\"${file}.h\", \"" >> $SEER_REGISTER_AUTO

    # plaintext version
    echo -n "Definitions:\n\" ${ufile}_AUTO_MISC_PRE \"" >> $SEER_REGISTER_AUTO
    echo -n "\n\n" >> $SEER_REGISTER_AUTO

    echo -n "Declarations:\n\" ${ufile}_AUTO_IMPORTS \"" >> $SEER_REGISTER_AUTO
    echo -n "\n\n" >> $SEER_REGISTER_AUTO

    echo -n "Implementations:\n\" ${ufile}_AUTO_MISC_POST \"" >> $SEER_REGISTER_AUTO
    echo -n "\n\n" >> $SEER_REGISTER_AUTO

    echo -n "\", \"" >> $SEER_REGISTER_AUTO
    
    # HTML version
    echo -n "<h1>Definitions:</h1><br>\" ${ufile}_AUTO_MISC_PRE_HTML \"" >> $SEER_REGISTER_AUTO
    echo -n "<br><hr>" >> $SEER_REGISTER_AUTO

    echo -n "<h1>Declarations:</h1><br>\" ${ufile}_AUTO_IMPORTS_HTML \"" >> $SEER_REGISTER_AUTO
    echo -n "<br><hr>" >> $SEER_REGISTER_AUTO

    echo -n "<h1>Implementations:</h1><br>\" ${ufile}_AUTO_MISC_POST_HTML \"" >> $SEER_REGISTER_AUTO
    echo -n "<br><hr>" >> $SEER_REGISTER_AUTO

    echo -n "\", " >> $SEER_REGISTER_AUTO
    
    fi
done
echo "NULL } " >> $SEER_REGISTER_AUTO
echo "" >> $SEER_REGISTER_AUTO


echo "};" >> $TRIXPLUG_HEADER
echo "" >> $TRIXPLUG_HEADER
echo "#define TRIXPLUG_INIT_ALL \\" >> $TRIXPLUG_HEADER
echo -e "$trixplug_inits" >> $TRIXPLUG_HEADER
echo "" >> $TRIXPLUG_HEADER
echo "" >> $TRIXPLUG_HEADER

echo "" >> $TRIXPLUG_WRAPPER
echo "#endif" >> $TRIXPLUG_WRAPPER

update_mainversion=`cat header_definition/update_mainversion`
update_subversion=`cat header_definition/update_subversion`

if [ "x$update_mainversion" != "xno" ]; then
    echo -n "Switch VERSION from $mainversion:$subversion to "
    let mainversion=$mainversion+1;
    let subversion=0;
    echo "$mainversion:$subversion"
else
    if [ "x$update_subversion" != "xno" ]; then
	echo -n "Switch VERSION from $mainversion:$subversion to "
	let subversion=$subversion+1;
	echo "$mainversion:$subversion"
    fi
fi

rm header_definition/update_mainversion
rm header_definition/update_subversion

echo "$mainversion $subversion" > header_definition/version
echo "#define TRIXPLUG_MAJOR $mainversion" > $TRIXPLUG_VERSION
echo "#define TRIXPLUG_MINOR $subversion" >> $TRIXPLUG_VERSION
