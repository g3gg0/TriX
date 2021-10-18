#!/bin/bash

DIALOG=${DIALOG=tools/dialog.exe}
BANNER="TriX  -  trick it out!" 

done_box () {
    echo
    echo "###################################################"
    echo "#                                                 #"
    echo "#                      DONE                       #"
    echo "#                                                 #"
    echo "###################################################"
    exit;
}



. compile.conf
if [ "x$OS" == "xLinux" ]; then 
    DIALOG="tools/dialog";
    LDEFINES="$LDEFINES -lm";
fi

echo > log

# overwriting TYPE if argument is set
if [ $1 ]; then
    export TYPE=$1;
fi;


if [ "x$TYPE" != "xBEAUTY" ] && [ "x$TYPE" != "xPC" ]; then
    clear
    echo "###################################################"
    echo "#                                                 #"
    echo "#          no valid TYPE selected                 #"
    echo "#                                                 #"
    echo "###################################################"
    done_box;
fi;

# BEAUTIFYING
if [ "x$TYPE" == "xBEAUTY" ]; then
(
#    clear
#    echo "###################################################"
#    echo "#                                                 #"
#    echo "#          beautyfying the code                   #"
#    echo "#                                                 #"
#    echo "###################################################"
    num=0;
    cnt=0;
    
    echo "XXX";
    echo "0";
    echo "\n  Counting files";
    echo "XXX";
    S_FILES=`find ./src/ -name \*.s`;
    C_FILES=`find ./src/ -name \*.c`;
    CPP_FILES=`find ./src/ -name \*.cpp`;
    H_FILES=`find ./src/ -name \*.h`;

    for file in $C_FILES; do
	let num=$num+1;
    done
    for file in $CPP_FILES; do
	let num=$num+1;
    done
    for file in $S_FILES; do
	let num=$num+1;
    done
    for file in $H_FILES; do
	let num=$num+1;
    done
    
    for file in $S_FILES; do
	let cnt=$cnt+1;
	let pct=100*$cnt/$num;
	echo "XXX";
	echo "$pct";
	echo "\n  Beautyfying  <$file>";
	echo "XXX";
        chmod u+rw $file > /dev/null 2>&1
        unix2dos $file > /dev/null 2>&1;
        dos2unix $file > /dev/null 2>&1;
    done;
    for file in $C_FILES; do
	let cnt=$cnt+1;
	let pct=100*$cnt/$num;
	echo "XXX";
	echo "$pct";
	echo "\n  Beautyfying  <$file>";
	echo "XXX";
        chmod u+rw $file > /dev/null 2>&1
        unix2dos $file > /dev/null 2>&1;
        dos2unix $file > /dev/null 2>&1;
        indent -i4 -sc -bli0 -cs -prs -cdb -bl0 -cbi0 -cli4 -ss -nut -ts4 -l200 -bap $file > /dev/null  2>&1;
    done;
    for file in $H_FILES; do
	let cnt=$cnt+1;
	let pct=100*$cnt/$num;
	echo "XXX";
	echo "$pct";
	echo "\n  Beautyfying  <$file>";
	echo "XXX";
        chmod u+rw $file > /dev/null 2>&1
        unix2dos $file > /dev/null 2>&1;
        dos2unix $file > /dev/null 2>&1;
        indent -i4 -sc -bli0 -cs -prs -cdb -bl0 -cbi0 -cli4 -ss -nut -ts4 -l200 -bap $file;
    done;
) | $DIALOG --backtitle "$BANNER" --title "Beautyfying" --guage "Initializing" 8 70 0
exit;
fi;

(
echo "XXX";
echo "0";
echo "\n  Preparing file lists";
echo "XXX";

export COMPILER_PATH="/bin";
export mode_str="PC";


# EXPORT LISTS
export type="$TYPE";

export C_EXCL_GREP="none_existing/no_one_create_this"
export CPP_EXCL_GREP="ui_ppmodd_main"
export O_EXCL_GREP="none_existing/no_one_create_this"


# if upper part is improved, the following can be removed
for file in $C_EXCL_PC; do
    export C_EXCL_GREP="$C_EXCL_GREP\|$file";
done;
for file in $CPP_EXCL_PC; do
    export CPP_EXCL_GREP="$C_EXCL_GREP\|$file";
done;
for file in $O_EXCL_PC; do
    export O_EXCL_GREP="$O_EXCL_GREP\|$file";
done;
export O="$O_PC";

export C_FILES="";
for file in $C_FILES_ALL; do
	if [ -e "$file.c" ]; then
		export C_FILES="${C_FILES}`echo $file | grep -v "$C_EXCL_GREP"` "
	fi;
done;

export CPP_FILES="";
for file in $CPP_FILES_ALL; do
	if [ -e "$file.cpp" ]; then
		export CPP_FILES="${CPP_FILES}`echo $file | grep -v "$CPP_EXCL_GREP"` "
	fi;
done;

export O_FILES="";
export O_FILES_ALL_GREP="none_existing/no_one_create_this.o";
for file in $O_FILES_ALL; do
    export O_FILES="${O_FILES}`echo $file | grep -v "$O_EXCL_GREP"` "
    export O_FILES_ALL_GREP="$O_FILES_ALL_GREP\|${file}.o"
done;

export O_LIST="";
for file in $O_FILES; do
    export O_LIST="${file}.o $O_LIST";
done;




export mode_str="PC";
export suffix="_pc";
export TARGET="PC";



num=0;
pct=0;
cnt=0;
warn="";


echo "XXX";
echo "0";
echo "\n  Counting files";
echo "XXX";

for file in $C_FILES; do
    let num=$num+1;
done

for file in $CPP_FILES; do
    let num=$num+1;
done



for file in $CPP_FILES; do

    if [ -f ${file}.cpp ]; then
    let cnt=$cnt+1;
    let pct=95*$cnt/$num;
    mode="PC";
    if [ "x`echo $file | gawk -F\+ \"{ print \\$2; }\"`" != "x" ]; then
        mode="`echo $file | gawk -F\+ \"{ print \\$2; }\"`";
        file="`echo $file | gawk -F\+ \"{ print \\$1; }\"`";
    fi;
    echo "XXX";
    echo "$pct";
    echo "\n  Compiling  <$file.cpp> ($mode)";
    echo "XXX";
    msg="`g++ -g  $O  -c $file.cpp -o $file.o  $DEFINES  2>&1`";
    RC=$?;

    for object in $O_FILES_ALL; do
	if [ "x$object" == "x${file}_${TARGET}" ]; then
	    mv $file.o ${file}_${TARGET}.o;
	fi;
    done;
    
    
    if [ "x$RC" != "x0" ]; then
	echo -ne "$msg" >> err.$$;
	exit;
    fi;
    if [ "x$msg" != "x" ]; then
	warn=`echo "$warn\n$file:\n$msg"`
    fi;
    fi;
done


for file in $C_FILES; do

    if [ -f ${file}.c ]; then
    let cnt=$cnt+1;
    let pct=95*$cnt/$num;
    mode="PC";
    if [ "x`echo $file | gawk -F\+ \"{ print \\$2; }\"`" != "x" ]; then
        mode="`echo $file | gawk -F\+ \"{ print \\$2; }\"`";
        file="`echo $file | gawk -F\+ \"{ print \\$1; }\"`";
    fi;
    echo "XXX";
    echo "$pct";
    echo "\n  Compiling  <$file.c> ($mode)";
    echo "XXX";
    msg="`gcc-3.3 -g  $O  -c $file.c -o $file.o  $DEFINES  2>&1`";
    RC=$?;

    for object in $O_FILES_ALL; do
	if [ "x$object" == "x${file}_${TARGET}" ]; then
	    mv $file.o ${file}_${TARGET}.o;
	fi;
    done;
    
    
    if [ "x$RC" != "x0" ]; then
	echo -ne "$msg" >> err.$$;
	exit;
    fi;
    if [ "x$msg" != "x" ]; then
	warn=`echo "$warn\n$file:\n$msg"`
    fi;
    fi;
done



if [ "x$warn" != "x" ]; then
    echo "$warn" >> warn.$$;
fi;


    echo "XXX";
    echo "97";
    echo "\n  Collecting Objects";
    echo "XXX";

    for file in $C_FILES; do
	skip="no";
	
	# split filename from options
	if [ "x`echo $file | gawk -F\+ \"{ print \\$2; }\"`" != "x" ]; then
	    file=`echo $file | gawk -F\+ "{ print \\$1; }"`;
	fi;
	
	# is the file some private developer file?
	for object in $O_FILES_ALL; do
	    if [ "x${object}" == "x${file}_${TARGET}" ]; then
		skip="yes";
	    fi
	done;
	
	# then dont link the plain .o file since it doesnt exist
	if [ "x$skip" != "xyes" ]; then
	    export O_LIST="$O_LIST ${file}.o"
	fi;
    done

    for file in $CPP_FILES; do
	skip="no";
	
	# split filename from options
	if [ "x`echo $file | gawk -F\+ \"{ print \\$2; }\"`" != "x" ]; then
	    file=`echo $file | gawk -F\+ "{ print \\$1; }"`;
	fi;
	
	# is the file some private developer file?
	for object in $O_FILES_ALL; do
	    if [ "x${object}" == "x${file}_${TARGET}" ]; then
		skip="yes";
	    fi
	done;
	
	# then dont link the plain .o file since it doesnt exist
	if [ "x$skip" != "xyes" ]; then
	    export O_LIST="$O_LIST ${file}.o"
	fi;
    done

    echo "XXX";
    echo "96";
    echo "\n  Linking Objects";
    echo "XXX";

    msg=`gcc-3.3 -g $O $O_LIST $LDEFINES -o TriX.exe 2>&1`
    RC=$?;

    if [ "x$RC" != "x0" ]; then
	echo -ne "$msg" | grep -v interwork > err.$$;
	exit 1;
    fi;

    echo "XXX";
    echo "97";
    echo "\n  Cleanup";
    echo "XXX";

    find -name \*~    | xargs rm >/dev/null 2>&1
    
    echo "XXX";
    echo "98";
    echo "\n  Cleanup";
    echo "XXX";

    find -name \*.o   | grep -v "$O_FILES_ALL_GREP" | xargs rm >/dev/null 2>&1
    
    echo "XXX";
    echo "99";
    echo "\n  Cleanup";
    echo "XXX";

    find -name \*.bin | xargs rm >/dev/null 2>&1
    
    chmod +x TriX.exe

    echo "XXX";
    echo "100";
    echo "\n  Done";
    echo "XXX";

) | $DIALOG --backtitle "$BANNER" --title "Compiling - ${TYPE}${PHONE_STR}" --guage "Initializing" 8 70 0

#
# removed $mode_str since its a private variable of the above subshell i extended
# maybe move assignment of this variable outside the subshell?
#
#   - g3gg0 -
#

if [ -f warn.$$ ]; then
    msg=`cat warn.$$`;
    if [ "x$LEAVELOG" != "xYES" ]; then
        rm warn.$$;
    fi;
    $DIALOG --backtitle "$BANNER" --title "Compiling - ${TYPE}${PHONE_STR}" --msgbox "Compile Warnings:\n\n$msg" 20 70 
fi;

if [ -f err.$$ ]; then
    msg=`cat err.$$`;
    if [ "x$LEAVELOG" != "xYES" ]; then
	rm err.$$;
    fi;
    $DIALOG --backtitle "$BANNER" --title "Compiling - ${TYPE}${PHONE_STR}" --msgbox "Compile Error:\n\n$msg" 20 70 
    rm log > /dev/null 2>&1
    exit;
fi;

msg=`cat log`;
$DIALOG --backtitle "$BANNER" --title "Finished" --msgbox "TriX (${TYPE}${PHONE_STR}) is Ready...\n\n$msg" 6 25 

rm log > /dev/null 2>&1


