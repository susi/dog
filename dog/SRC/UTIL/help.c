/* DOG Documentation */

#include "util.h"
/*
Command line Syntax
DOG [-P|-E envsz|-C command line] 
Parameters: 

-P - Makes DOG a PERMANENT shell. 
-E envsz - Makes the environment to (envsz / 16) 16 bit pages. 
-C command line - Executes the command line and exits. 
*/

/*#define no_commands 20 */

char br[300]="\n\nSyntax: BR [ON|OFF]\n\
Parameters:\n\
ON - Set DOS Break flag to ON [check for break on all DOS calls].\n\
OFF - Set DOS Break flag to OFF [(check for break only when reading/writing stdin/stdout/stderr].\n\
Effect: Toggles the DOS Break flag. In case no parameters are given it returns the Break Flag\n\n";

char cc[300]="\n\nSyntax: CC [CodePage]\n\
Parameters:\n\
CodePage - Codepage number to set as current\n\
- cc without Args shows available codepages.\n\
Effect: Changes the current codepage to CodePage.\n\n";

char cd[400]="\n\nSyntax: CD [Path]\n\
Parameters:\n\
Path - The destination path. The .. directory can be appended directly to cd like in DOS. e.g.cd... The virtual directory ... means ..\\.. \
- cd without Args prints the current directory on the screen.\n\
Effect: Changes the current directory to Path.\n\n";

char cl[400]="\n\nSyntax: CL\n\
Effect: Clears the screen. Works on 50 lines/page screens too.\n\n";

char cp[400]="\n\nNOT implemented yet\n\n";

char ct[400]="\n\nSyntax: CT Device\n\
Parameters:\n\
- Device the name of the device that io is set to. Eg. CON, AUX, NUL...\n\
Effect: Changes the current IO terminal to Device.\n\n";

char eh[1000]="\n\nSyntax: EH text\n\
Parameters:\n\
text - anything you want to type out from a DOG file.\n\
Use these special characters to format the text.\n\
\n\
$$ - the $ sign.\n\
$_ - space\n\
$b - vertical bar ( ¦ )\n\
$e - The ESC character (ASCII 27 = 1b in hex)\n\
$l - Left angle ( < )\n\
$g - Right angle ( > )\n\
$n - New line\n\
$r - Carriage return\n\
$t - Tabulator\n\
Effect: Print a (formatted) string to stdio (normally the screen). EH by itself wil put a new line.\n\n";

char hh[400]="\n\nSyntax: HH\n\
Effect: Prints ALL the internal DOG commands.\n\n";

char ls[400]="\n\nSyntax: LS [arg_list] [search pattern]\n\
Parameters:\n\
The args can be given in any order.\n\
arg_list can be any of the following args.\n\
-a - Any file Attribute\n\
-d - Directories Only\n\
-f - Files Only\n\
-h - help text\n\
-l - show volume label\n\
-? - syntax\n\
search pattern - a list of filenames to find. The * and ? wildcards are OK.\n\
Effect: List files according to switches and search pattern.\n\n";

char md[400]="\n\nSyntax: MD <dir>\n\
Parameters:\n\
dir - Name or Path of new directory.\n\
Effect: Creates a new directory as in COMMAND.COM.\n\n";

char nn[400]="\n\nSyntax: MV <old_name> <new_name>\n\
Parameters:\n\
old_name - The path to the file you want to change. * and ? are OK\n\
new_name - The path to the new filename. * and ? are OK\n\
Effect: Creates a new directory as in COMMAND.COM.\n\n";

char rd[400]="\n\nSyntax: RD <dir>\n\
Parameters:\n\
dir - Name or path of EMPTY directory to remove.\n\
Effect: Removes a directory as in COMMAND.COM.\n\n";

char rm[400]="\n\nSyntax: RM <list>\n\
Parameters:\n\
list - A space separated list of files you want to delete. No more multiple commands to delete *.BK! *.BKP etc files!\n\
Effect: Delete files as per the COMMAND.COM DEL/DELETE command.\n\n";

char se[1000]="\n\nSyntax: SE [env_var=[string]]\n\
Parameters:\n\
env_var - the name of the environment variable. The name is UPPERCASED Predefines ones are:\n\
PATH - A ; separated list of directories that are searched in to find a program.\n\
PROMPT - The string may consist of normal text and these special characters\n\
$$ - the $ sign.\n\
$_ - space\n\
$b - vertical bar ( ¦ )\n\
$e - The ESC character (ASCII 27 = 1b in hex)\n\
$l - Left angle ( < )\n\
$g - Right angle ( > )\n\
$n - New line\n\
$r - Carriage return\n\
$t - Tabulator\n\
$p - Current drive and path\n\
$c - Current time.\n\
COMSPEC - Path to DOG\n\
string - The value that env_var should have.\n\
Effect: Sets the specified env_var to string in the environment. Without arguments the command prints all env_vars to the screen.\n\n";

char sz[400]="\n\nSyntax: SZ [-k|-m]\n\
Parameters:\n\
-k - give sizes in kb\n\
-m - give sizes in Mb\n\
Effect: Prints the size of the files in the current directory.\n\n";

char tp[400]="\n\nSyntax: TP file_list\n\
Parameters:\n\
file_list - A space separated list of filenames.\n\
Effect: Types the contents of the files in file_list to stdout.\n\\n\n";

char vf[400]="\n\nSyntax: VF [ON|OFF]\n\
Parameters:\n\
ON - Set the DOS Verify flag to ON\n\
OFF - Set the DOS Verify flag to OFF\n\
Effect: If ON or OFF specified sets Verify flag to this value, else returns the current setting.\n\n";

char vr[100]="\n\nSyntax: VR\n\
Effect: Prints the DOG and DOS version. Interprets OS/2 and IBM version correctly.\n\n";

char xx[90]="\n\nEffect: Exit the current DOG shell.\n\
	Works only if the shell is not a primary shell.\n\n";

char * intern_commands[19]={"br", "cc", "cd", "cl", "cp", "ct", "eh", "hh", "ls", "md", "nn", "rd","rm","se","sz","tp","vf","vr","xx"};

main(int argc, char ** argv){


short int x;
if(1==argc){
	/* The following has been split into several lines
	to make it easier to read as source code.
	To make it compile smaller, just the entire statement out
	and make another copy of it below, without the last
	back slash in each line. */
printf("\nThe help program for DOG is meant \
to make it *extremely* easy for\n\
you to learn how to use DOG. In fact, you dont even need to\n\
know how to spell 'compewter'.\n\n\
The internal commands available in DOG are:\n\
BR\n\
CC\n\
CD\n\
CL\n\
CP\n\
CT\n\
EH\n\
HH\n\
LS\n\
MD\n\
MV\n\
RD\n\
RM\n\
SE\n\
SZ\n\
TP\n\
VF\n\
VR\n\
XX\n\n\
Type 'help <command>' to see how to use a command.\n\n\
For instance, type 'help cc' to see how the 'cc' command works,\n\
or type 'help rm' to see how the 'rm' command works.\n\n");
}
else {
	if(0==strcmp(intern_commands[0], argv[1]))
		printf("%s\n", br);
	if(0==strcmp(intern_commands[1], argv[1]))
		printf("%s\n", cc);
	if(0==strcmp(intern_commands[2], argv[1]))
		printf("%s\n", cd);
	if(0==strcmp(intern_commands[3], argv[1]))
		printf("%s\n", cl);
	if(0==strcmp(intern_commands[4], argv[1]))
		printf("%s\n", cp);
	if(0==strcmp(intern_commands[5], argv[1]))
		printf("%s\n", ct);
	if(0==strcmp(intern_commands[6], argv[1]))
		printf("%s\n", eh);
	if(0==strcmp(intern_commands[7], argv[1]))
		printf("%s\n", hh);
	if(0==strcmp(intern_commands[8], argv[1]))
		printf("%s\n", ls);
	if(0==strcmp(intern_commands[9], argv[1]))
		printf("%s\n", md);
	if(0==strcmp(intern_commands[10], argv[1]))
		printf("%s\n", nn);
	if(0==strcmp(intern_commands[11], argv[1]))
		printf("%s\n", rd);
	if(0==strcmp(intern_commands[12], argv[1]))
		printf("%s\n", rm);
	if(0==strcmp(intern_commands[13], argv[1]))
		printf("%s\n", se);
	if(0==strcmp(intern_commands[14], argv[1]))
		printf("%s\n", sz);
	if(0==strcmp(intern_commands[15], argv[1]))
		printf("%s\n", tp);
	if(0==strcmp(intern_commands[16], argv[1]))
		printf("%s\n", vf);
	if(0==strcmp(intern_commands[17], argv[1]))
		printf("%s\n", vr);
	if(0==strcmp(intern_commands[18], argv[1]))
		printf("%s\n", xx);
}
    return 0;
}
