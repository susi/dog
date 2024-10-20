/* DOG Documentation Command
Copyright (C) 2000 Eugene Wong Wolf Bergenheim

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This program is part of DOG - The DOG Operating Ground

History
2000-07-04 - Created by Eugene Wong [EW]
2024-05-24 - Restructured and expanded to include more verbose documentation. - WB
2024-05-25 - Added documentation for 44 - WB
2024-05-26 - Added documentation for DO - WB
2024-06-01 - Added documentation for RT - WB
2024-06-01 - Added documentation for TI - WB
2024-06-15 - Added more verbosity to dogcommands & page break before it. - WB
2024-06-17 - Spellcheck - WB
2024-06-20 - Added documentation for DT - WB
2024-06-20 - Added this log - WB
2024-06-26 - Added documentation for DS - WB
2024-10-14 - Added documentation for PP - WB
2024-10-16 - Added documentation for PT - WB
*/

#include "util.h"

#define _NCOMS 11
#define _NBCOMS 8
#define _NECOMS 16

BYTE *dog_help="\n\nSyntax:DOG [-P|-E envsz|-A aliassz|-C command line]\n"
    "Parameters:\n"
    "   -P - Makes DOG a PERMANENT shell.\n"
    "   -E envsz - Makes the environment to envsz bytes (divisible by 16).\n"
    "   -A aliassz - Makes the alias space to aliassz bytes (divisible by 16).\n"
    "   -C command line - Executes the command line and exits.\n\n";

BYTE int_commands[_NCOMS][3]={
    "AL",
    "CC",
    "CD",
    "CT",
    "EH",
    "HH",
    "MD",
    "RD",
    "SE",
    "XX"};

BYTE command_des[_NCOMS][21] = {
    "ALias               ",
    "Change Codepage     ",
    "Change Directory    ",
    "Change Terminal     ",
    "EcHo                ",
    "Help                ",
    "Make Directory      ",
    "Remove Directory    ",
    "Set to Environment  ",
    "eXit                "};

BYTE *cmd_help[_NCOMS] = {
    "\n\nSyntax: AL [ALIAS] [COMMAND]\n"
    "Parameters:\n"
    "   ALIAS   - The command alias to set/unset\n"
    "             The name is UPPERCASED.\n\n"
    "   COMMAND - The real command that ALIAS expands to.\n"
    "             Without COMMAND, AL unsets ALIAS\n"
    "           - Without arguments the command prints all ALIASes to the screen.\n\n"
    "Effect: Creates a command ALIAS\n\n"
    "Example: 'AL CV CP -v' creates an alias command 'CV' which expands into the\n"
    "         command 'CP -v'.\n\n"
    "         You can also use AL to create an ALIAS of an existing command.\n"
    "         It has the same effect as if typed the expansion.\n"
    "         e.g. 'AL CP CP -v' makes it so that whenever you use just 'CP',\n"
    "         the '-v' option is used, handy for setting default options.\n\n",

    "\n\nSyntax: CC [CodePage] -- NOT IMPLEMENTED YET\n"
    "Parameters:\n"
    "   CodePage - Codepage number to set as current\n"
    "            - cc without Args shows available codepages.\n\n"
    "Effect: Changes the current codepage to CodePage.\n\n",

    "\n\nSyntax: CD [Path]\n"
    "Parameters:\n"
    "   Path - The destination path.\n"
    "          The .. directory can be appended directly to cd like in DOS.\n"
    "          e.g. cd... The virtual directory ... means ..\\.. \n"
    "        - cd without Args prints the current directory on the screen.\n\n"
    "Effect: Changes the current directory to Path.\n\n"
    "Note:   In DOG, directories are executable\n"
    "        'dog\\src\\util' implicitly means 'cd dog\\src\\util'\n"
    "        when 'util' is a directory.\n\n",

    "\n\nSyntax: CT DEV\n"
    "Parameters:\n"
    "   DEV - The name of the device that I/O is set to. Eg. CON, AUX, NUL\n\n"
    "Effect: Changes the current IO terminal to Device.\n\n",

    "\n\nSyntax: EH [OPTION] [TEXT]\n"
    "Parameters:\n"
    "   TEXT - Anything you want to type out from a DOG file.\n"
    "        - EH by itself will put a new line.\n"
    " OPTION can be one of these (in any order):\n"
    "   -n - Do not format the text with the special characters\n\n"
    "Use these special characters to format the text:\n"
    "   $$ - the $ sign.\n"
    "   $_ - space\n"
    "   $b - vertical bar ( ¦ )\n"
    "   $e - The ESC character (ASCII 27 = 1b in hex)\n"
    "   $l - Left angle ( < )\n"
    "   $g - Right angle ( > )\n"
    "   $n - New line\n"
    "   $r - Carriage return\n"
    "   $t - Tabulator\n"
    "Effect: Print a (formatted) string to stdout (normally the screen).\n\n",

    "\n\nSyntax: HH [COMMAND]\n"
    "Parameters:\n"
    "   COMMAND - Giving the name of a DOG command will display\n"
    "             detailed help for that command.\n"
    "           - HH alone displays a list of all DOG commands.\n"
    "Effect: Displays usage information for DOG commands.\n\n",

    "\n\nSyntax: MD <DIR>\n"
    "Parameters:\n"
    "   DIR - Name or Path of new directory.\n"
    "Effect: Creates a new directory named 'dir'.\n\n",

    "\n\nSyntax: RD <DIR>\n"
    "Parameters:\n"
    "   DIR - Name or path of an EMPTY directory to remove.\n"
    "Effect: Removes directory named 'dir'\n\n",

    "\n\nSyntax: SE [VARIABLE] [VALUE]\n"
    "Parameters:\n"
    "   VARIABLE - the name of the environment variable.\n"
    "              The name is UPPERCASED.\n\n"
    "   VALUE    - The value that VARIABLE should have.\n"
    "              Without VALUE, SE unsets VARIABLE\n"
    "            - Without arguments the command prints all VARIABLEs to the screen.\n\n"
    " Variables set and used by DOG:\n"
    "   COMSPEC - Path to DOG\n"
    "   PATH    - The list of paths where to search for commands.\n"
    "             The paths are separated by the `;` character.\n"
    "   PROMPT  - The string displayed indicating that DOG is waiting for a command\n"
    "             You can use these special characters:\n"
    "      $$ - the $ sign.\n"
    "      $_ - space\n"
    "      $b - vertical bar '|'\n"
    "      $e - The ESC character (ASCII 27 = 1b in hex)\n"
    "      $l - Left angle '<'\n"
    "      $g - Right angle '>'\n"
    "      $n - New line\n"
    "      $r - Carriage return\n"
    "      $t - Tabulator\n"
    "      $p - Current drive and path\n"
    "      $c - Current time\n"
    "Effect: Sets the specified VARIABLE to VALUE in the environment.\n",

    "\n\nSyntax: TN <PATH>\n"
    "Parameters:\n"
    "   PATH - Name or path of to resolve.\n"
    "Effect: Returns the 'true name' of the given input by\n"
    "        resolving *, . and .. entries and also any\n"
    "        SUBST, ASSIGN and JOIN are unravelled\n\n",

    "\n\nSyntax: XX\n"
    "Effect: Exit the current DOG shell, unless it is the primary\n"
    "        shell, in which case the command has no effect.\n\n"};

/* External commands */
BYTE ext_commands[_NECOMS][3] = {
    "BP",
    "BR",
    "CL",
    "CP",
    "DS",
    "DT",
    "LS",
    "MV",
    "PP",
    "PT",
    "RM",
    "RT",
    "SZ",
    "TP",
    "VF",
    "VR"};

BYTE ext_command_des[_NECOMS][21] = {
    "BeeP                ",
    "BReak               ",
    "CLear screen        ",
    "CoPy                ",
    "Directory Stack     ",
    "DaTe                ",
    "LiSt files          ",
    "MoVe file (rename)  ",
    "PromPt              ",
    "PaTh                ",
    "ReMove files        ",
    "Remove Tree         ",
    "SiZe of files in dir",
    "TyPe                ",
    "VeriFy              ",
    "VeRsion             "};

BYTE *ext_help[_NECOMS] = {
    "\n\nSyntax: BP [FREQ [TIME]]\n"
    "Parameters:\n"
    "   FREQ - Frequency of the sound.\n"
    "   TIME - Duration in milliseconds (1000 ms = 1 second)\n"
    "        - Without parameters, BP puts ASCII character 07h into stdout,\n"
    "          causing a beep provided that the ANSI driver is loaded.\n"
    "Effect: Makes a constant sound with frequency FREQ for TIME milliseconds\n"
    "        through the PC Speaker.\n\n",

    "\n\nSyntax: BR [ON|OFF]\n"
    "Parameters:\n"
    "   ON - Set DOS Break flag to ON:\n"
    "        Check for break on all DOS calls.\n"
    "  OFF - Set DOS Break flag to OFF:\n"
    "        Check for break only when reading/writing stdin/stdout/stderr.\n\n"
    "Effect: Toggles the DOS Break flag.\n"
    "        In case no parameters are given it returns the Break Flag\n\n",

    "\n\nSyntax: CL\n"
    "Effect: Clears the screen. Works on 50 lines/page screens too.\n\n",

    "\n\nSyntax: CP [OPTION]... SOURCE DEST\n"
    "    or: CP [OPTION]... [PATH1\\]PATTERN1 [PATH2\\]PATTERN2\n"
    "Parameters:\n"
    "   SOURCE / PATTERN1 - The source file(s) to copy\n"
    "   DEST   / PATTERN2 - The target file(s) to create\n\n"
    " OPTION can be one of these (in any order):\n"
    "   -v - print filename of each copied file\n"
    "   -f - force all files to be overwritten\n"
    "   -i - interactive, asks before copying every file\n"
    "Effect: Copies one or more files to the destination.\n"
    "        CP asks before overwriting a file (can be overridden with -f).\n"
    "Examples:\n"
    "        cp dog.com wolf.com\n"
    "        cp dog.com c:\dog\n"
    "        cp dog.com c:\dog\wolf.com\n"
    "        cp c:\dog\n"
    "        cp dog.* wolf.*\n"
    "        cp dog.* wolf.*\n"
    "        cp *.o *.obj\n\n",

    "\n\nSyntax: DS push [[D:]PATH]\n"
    "    or: DS pop\n"
    "    or: DS clear\n"
    "    or: DS list\n"
    "Parameters:\n"
    "   push - Pushes the current directory on to the Directory Stack\n"
    "          If the optional absolute or relative path are given\n"
    "          it also changes to that directory\n"
    "    pop - Pops a directory off the stack and changes to that directory\n"
    "  clear - Clears the Directory Stack\n"
    "   list - Lists the entire Directory Stack\n"
    "Effect: Manages a directory stack.\n"
    "        You can push and pop directories to and from the stack.\n"
    "        You can also clear and list the stack.\n\n",

    "\n\nSyntax: DT [OPTION]\n"
    "Parameters:\n"
    " OPTION can be one of these:\n"
    "   -s - set DATE and TIME\n"
    "   -d - set DATE only\n"
    "   -t - set TIME only\n"
    "Effect: DT displays the current date and time. By using the options DT\n"
    "        can be used to set the DATE and/or TIME\n\n",

    "\n\nSyntax: LS [OPTION]... [PATTERN]...\n"
    "Parameters:\n"
    "   PATTERN - Pattern can be a number of paths including wild characters * and ?\n"
    "             ? represents any single character and\n"
    "             * represents 0 or more characters\n"
    "             e.g. *.dog *.* DO?.COM, *.?\n\n"
    " OPTION can be one of these (in any order, some don't work together):\n"
    "   -c - Use ANSI color to color entries. See COLORS below for format.\n"
    "   -d - Directories Only\n"
    "   -f - Files Only\n"
    "   -l - show volume label\n"
    "   -p - pause at every screen full or PATTERN\n"
    "   -w - show file names only (6 per row)\n"
    "   -z - show human readable sizes (B, KB, MB, GB)\n"
    " COLORS\n"
    "  NOTE: Requires NANSI.SYS or equivalent ANSI driver to work\n"
    "  Colors are taken from the LSCOLORS env variable and is a CSV of color RULES.\n"
    "  RULE is structured as EXT=FG;BG;ATTR where:\n"
    "    * EXT is a file extension or a special one for matching on file attributes:\n"
    "      * _D - Directory\n"
    "      * _S - System file\n"
    "      * _L - Label\n"
    "      * _H - Hidden file\n"
    "      * _A - Archive\n"
    "    * FG is the foreground color, one of:\n"
    "      * 30 - Black\n"
    "      * 31 - Red\n"
    "      * 32 - Green\n"
    "      * 33 - Yellow\n"
    "      * 34 - Blue\n"
    "      * 35 - Magenta\n"
    "      * 36 - Cyan\n"
    "      * 37 - White\n"
    "    * BG is the background color, one of:\n"
    "      * 40 - Black\n"
    "      * 41 - Red\n"
    "      * 42 - Green\n"
    "      * 43 - Yellow\n"
    "      * 44 - Blue\n"
    "      * 45 - Magenta\n"
    "      * 46 - Cyan\n"
    "      * 47 - White\n"
    "    * ATTR sets additionnal video attributes:\n"
    "      * 0 - Resets attributes and colors to white on black\n"
    "      * 1 - Bold/light color\n"
    "      * 4 - Underlined text\n"
    "      * 5 - Blinking text\n"
    "      * 7 - Reverse Video\n"
    "  Default rules is LSCOLORS is unset:\n"
    "    _D=34;0;1,_H=30;40;8,_R=33;40,_S=31;40;1,_L=32;40,\n"
    "    EXE=35;40,COM=35;40,DOG=35;40\n"
    "Effect: List files according to switches and search pattern.\n\n",

    "\n\nSyntax: MV [OPTION]... SOURCE DEST\n"
    "    or: MV [OPTION]... [PATH1\\]PATTERN1 [PATH2\\]PATTERN2\n"
    "Parameters:\n"
    "   SOURCE / PATTERN1 - The old file name(s)\n"
    "   DEST   / PATTERN2 - The new file name(s)\n\n"
    " OPTION can be one of these (in any order):\n"
    "   -v - print filename of each moved file\n"
    "Effect: Moves one or more files to the destination.\n"
    "        MV asks before overwriting an existing file.\n\n"
    "Examples:\n"
    "        mv dog.com wolf.com\n"
    "        mv dog.com c:\dog\n"
    "        mv dog.com c:\dog\wolf.com\n"
    "        mv c:\dog\n"
    "        mv dog.* wolf.*\n"
    "        mv dog.* wolf.*\n"
    "        mv *.o *.obj\n\n",

    "\n\nSyntax: PP [PROMPT]\n"
    "Parameters:\n"
    "           - PP alone displays the formatting for the current DOG prompt.\n"
    "   PROMPT  - The string displayed indicating that DOG is waiting for a command\n"
    "             You can use these special characters:\n"
    "      $$ - the $ sign.\n"
    "      $_ - space\n"
    "      $b - vertical bar '|'\n"
    "      $e - The ESC character (ASCII 27 = 1b in hex)\n"
    "      $l - Left angle '<'\n"
    "      $g - Right angle '>'\n"
    "      $n - New line\n"
    "      $r - Carriage return\n"
    "      $t - Tabulator\n"
    "      $p - Current drive and path\n"
    "      $c - Current time\n"
    "Effect: Displays the PROMPT or sets the PROMPT.\n\n",

    "\n\nSyntax: PT [OPTION] [PATH]\n"
    "Parameters:\n"
    "         - PT alone displays the current PATH"
    "   PATH  - The list of directories which DOG uses to find programs\n"
    "           to run. The PATH is a list of directories separated by a ';'\n"
    "           character.\n"
    " OPTION can be one of these (in any order):\n"
    "   -a - Append the given PATH instead of setting it to the PATH.\n"
    "Effect: Displays or sets the PATH which DOG uses to find programs to run.\n\n",

    "\n\nSyntax: RM [OPTION]... <PATTERN|@FILE>...\n"
    "Parameters:\n"
    "   PATTERN - Pattern can be a number of paths\n"
    "             including wild characters * and ?\n"
    "             ? represents any single character and\n"
    "             * represents 0 or more characters\n"
    "             e.g. *.dog *.* DO?.COM, *.?\n\n"
    "     @FILE - Filename preceded by '@' which is read and every line is\n"
    "             handled like a PATTERN\n"
    " OPTION can be one of these (in any order):\n"
    "   -i - prompt (Yes/No/All) for each file or directory\n"
    "   -r - recurse into sub-directories.\n"
    "        Removes all files in subdirectories too\n"
    "   -v - print filename of each removed file\n"
    "Effect: Remove the file(s) and/or directory(ies) matching PATTERN\n\n",

    "\n\nSyntax: RT [OPTION]... <DIRECTORY|@FILE>...\n"
    "Parameters:\n"
    "   DIRECTORY - The directory(s) to delete.\n"
    "     @FILE - Filename preceded by '@' which is read and every line is\n"
    "             handled like a DIRECTORY\n"
    " OPTION can be one of these (in any order):\n"
    "   -i - prompt (Yes/No/All) for each file or directory\n"
    "   -v - print filename of each removed file\n"
    "Effect: Removes the tree(s) indicated by DIRECTORY(s) with all their\n"
    "        subdirectories and files.\n\n",

    "\n\nSyntax: SZ [OPTION]... [PATTERN]...\n"
    "Parameters:\n"
    "   PATTERN - Pattern can be a number of paths including wild characters * and ?\n"
    "             ? represents any single character and\n"
    "             * represents 0 or more characters\n"
    "             e.g. *.dog *.* DO?.COM, *.?\n\n"
    " OPTION can be one of these (in any order):\n"
    "   -k - give sizes in KB\n"
    "   -m - give sizes in MB\n"
    "   -z - show human readable sizes (B, KB, MB, GB)\n"
    "   -r - recurse into sub-directories\n"
    "Effect: Prints the size of the files in the current directory.\n\n",

    "\n\nSyntax: TP FILE...\n"
    "Parameters:\n"
    "   FILE - A space separated list of files\n"
    "Effect: Types the contents of the files in file_list to stdout.\n\n",

    "\n\nSyntax: VF [ON|OFF]\n"
    "Parameters:\n"
    "ON  - Set the DOS Verify flag to ON\n"
    "OFF - Set the DOS Verify flag to OFF\n"
    "Effect: If ON or OFF specified sets Verify flag to this value.\n"
    "        Otherwise VF returns the current setting.\n\n",

    "\n\nSyntax: VR\n"
    "Effect: Prints the DOG and DOS version.\n"
    "        Tries very hard to identify the flavor of DOS.\n\n"};


BYTE bat_commands[_NBCOMS][3]={
    "CA",
    "DO",
    "44",
    "GO",
    "IF",
    "IN",
    "SH",
    "TI"
};

BYTE *bat_command_des[_NBCOMS]={
    "CAall DOGfile",
    "DO repeat command WHILE condition",
    "For word in set run command",
    "GO to label",
    "IF CONDITION run command",
    "INput from user",
    "SHift arguments",
    "TImer"
};


BYTE *bat_help[_NBCOMS]={
    "\n\nSyntax: CA <DOGFILE>\n"
    "Parameters:\n"
    "   DOGFILE - A DOGfile to call.\n\n"
    "Effect: Calls the DOGFILE from another.\n"
    "        Once it has finished running, execution continues on the next line\n"
    "        in the DOGfile where CA was executed from.",

    "\n\nSyntax: DO <COMMAND> [WHILE <CONDITION>]\n"
    "Parameters:\n"
    "   COMMAND   - The command to repeat.\n"
    "   CONDITION - The condition for repeating the command.\n"
    " CONDITION can take the form:\n"
    "      ERROR [IS|NOT] <NUMBER>     - NUMBER compared to the ERRORLEVEL\n"
    "      <VARIABLE> [IS|NOT] <VALUE> - VARIABLE is an environment variable\n"
    "                                    and is compared to VALUE\n"
    "      [NOT] EXIST <FILE>          - Returns true as long as FILE exists.\n"
    "     IS - is optional and has no effect other than makes the statement\n"
    "          look a little bit more like English.\n"
    "    NOT - reverses the condition value in all cases.\n"
    "Effect: Repeats COMMAND while CONDITION is true.\n"
    "        Without a CONDITION the COMMAND is repeated as long as it returns 0\n"
    "        as its exit code."
    "\n\n",

    "\n\nSyntax: 44 <VAR> IN <SET> DO <COMMAND>\n"
    "Parameters:\n"
    "   VAR     - A variable name, use as %%VAR%% as a placeholder in COMMAND\n"
    "   SET     - A comma separated list of words.\n"
    "\n"
    "   COMMAND - The command to repeat.\n"
    "             You can use %%VAR%% as a placeholder for VAR.\n"
    "Effect: For each word in SET runs the COMMAND.\n"
    "        Each word in SET will be set to VAR for each repeat of the for loop.\n"
    "        This allows the COMMAND to include %%VAR%% which will be replaced\n"
    "        with a word from SET.\n\n"
    "Example:\n"
    "   44 F IN DOG,CAT,WOLF,BEAR DO MV %%F%%.CC %%F%%.CPP\n"
    "      This command will first run 'MV DOG.CC DOG.CPP'\n"
    "      by replacing %%F%% with 'DOG' in the first iteration,\n"
    "      etc for each of the terms in SET.\n",

    "\n\nSyntax: GO <LABEL>\n"
    "Parameters:\n"
    "   LABEL - A label in the DOGfile.\n"
    "Effect: Jumps the DOGfile processing to the LABEL. A label is identified as\n"
    "        line containing ':LABEL'. DO will jump to the line after the\n"
    "        ':LABEL' line.\n",

    "\n\nSyntax: IF CONDITION <COMMAND1> [ELSE <COMMAND2>]\n"
    "Parameters:\n"
    "   COMMAND1  - The command to run if CONDITION is TRUE.\n"
    "   COMMAND2  - The optional command to run if CONDITION is FALSE.\n"
    "   CONDITION - The condition for repeating the command.\n"
    " CONDITION can take the form:\n"
    "      ERROR [IS|NOT] <NUMBER>     - NUMBER compared to the ERRORLEVEL\n"
    "      <VARIABLE> [IS|NOT] <VALUE> - VARIABLE is an environment variable\n"
    "                                 and is compared to VALUE\n"
    "      [NOT] EXIST <FILE>       - Returns true as long as FILE exists.\n"
    "    IS  - is optional and has no effect other than makes the statement\n"
    "          look a little bit more like English.\n"
    "    NOT - reverses the condition value in all cases.\n"
    "Effect: If CONDITION is TRUE the COMMAND1 is executed, else COMMAND2 is executed.\n\n",

    "\n\nSyntax: IN <VARIABLE> [PROMPT]\n"
    "Parameters:\n"
    "   VARIABLE - The environment variable to store the user input.\n"
    "   PROMPT   - An optional message to display to the user.\n"
    "              With no prompt DOG will display 'Enter value for VARIABLE: '\n"
    "Effect: Prompts the user for input and saves that input in the environment.\n\n",

    "\n\nSyntax: SH [NUM]\n"
    "Parameters:\n"
    "   NUM - A number to indicate how many variables to shift\n"
    "Effect: Shifts the DOGfile input arguments by NUM steps.\n"
    "Normally A DOGfile can only index 10 arguments at one time (%0 to %9).\n"
    "This command allows a DOGfile to shift the variables right so that\n"
    "what was previously %1 is now %2, etc.\n"
    "By giving the command a number NUM, the arguments are shifted\n"
    "NUM steps instead of just one to a maximum of 9.\n\n",

    "\n\nSyntax: TI [TIME]\n"
    "Parameters:\n"
    "   TIME - The number of seconds to wait.\n"
    "        - Without an argument TI waits forever for a key stroke\n"
    "Effect: Waits TIME seconds or for a key press before continuing the\n"
    "        DOGFile execution\n\n",
};

BYTE *whatisdog = "DOG is an alternative command interpreter.\n"
    "It works similar to COMMAND.COM or 4DOS, but is different.\n"
    "Note that DOG is not drop-in compatible with either command\n"
    "interpreter, but instead does things differently.\n"
    "DOG commands are always just 2 letters, but still easy to remember\n"
    "DOG also offers external commands allowing them to be more complex.";

BYTE *dogcommands =
    "DOG consists of 2 classes of commands: INTERNAL and EXTERNAL.\n"
    "The internal commands are built into to the DOG.COM binary and are\n"
    "always in memory and access the internals of DOG.\n"
    "There are 2 kinds of internal commands. Regular user commands and \n"
    "the batch file programming commands.\n\n"
    "When working with DOG in the prompt, you typically use the internal\n"
    "and external commands, and other programs.\n"
    "The batch commands are used to program batch files, called DOGfiles,\n"
    "and cover things like loops, forks and user input. They aren't available\n"
    "when working on the prompt.\n"
    "The external commands are stand-alone executables, but considered\n"
    "to be part of DOG, and may access DOG internals where needed.";

BYTE read_key(void);

main(int argc, char ** argv)
{
    BYTE i;
    char *cmd;
    if(1==argc){
	puts("\nThe help program for DOG is meant to make it easy for\n"
	     "you to learn how to use DOG. In fact, you don't even need to\n"
	     "know how to spell 'compewter'.\n");
	puts(whatisdog);
	puts("\n<<<Press any key to continue...>>>");
	read_key();
	puts(dogcommands);
	puts("\n<<<Press any key to see the internal commands available in DOG>>>");
	read_key();
	puts("\n");
	for(i=0;i < _NCOMS;i++) {
	    printf("%s - %s\n", int_commands[i], command_des[i]);
	}
	puts("\n<<<Press any key to see the external commands available in DOG>>>");
	read_key();
	puts("\n");
	for(i=0;i < _NECOMS;i++) {
	    printf("%s - %s\n", ext_commands[i], ext_command_des[i]);
	}

	puts("\n<<<Press any key to see the dogfile commands available in DOG>>>");
	read_key();
	puts("\n");
	for(i=0;i < _NBCOMS;i++) {
	    printf("%s - %s\n", bat_commands[i], bat_command_des[i]);
	}

	puts("\nType 'HH COMMAND' where COMMAND is one of the above commands to see\n"
	     "how to use that COMMAND. e.g. HH LS - to see how the LS command works\n\n");
    }
    else {
	cmd = strupr(argv[1]);
	for(i=0;i < _NCOMS;i++) {
	    if(strcmp(cmd, int_commands[i]) == 0) {
		puts(cmd_help[i]);
		return 0;
	    }
	}
	for(i=0;i < _NECOMS;i++) {
	    if(strcmp(cmd, ext_commands[i]) == 0) {
		puts(ext_help[i]);
		return 0;
	    }
	}
	for(i=0;i < _NBCOMS;i++) {
	    if(strcmp(cmd, bat_commands[i]) == 0) {
		puts(bat_help[i]);
		return 0;
	    }
	}
	if (strcmp(cmd, "DOG") == 0) {
	    puts(whatisdog);
	    puts(dog_help);
	    return 0;
	}
	if (strcmp(cmd, "ALL") == 0) {
	    puts(whatisdog);
	    puts(dogcommands);
	    puts(dog_help);
	    for(i=0;i < _NCOMS;i++) {
		puts(cmd_help[i]);
	    }
	    for(i=0;i < _NECOMS;i++) {
		puts(ext_help[i]);
	    }
	    for(i=0;i < _NBCOMS;i++) {
		puts(bat_help[i]);
	    }
	    return 0;
	}
	printf("'%s' is not a command in DOG\n try HH\n", cmd);
	return 1;
    }
    return 0;
}

BYTE read_key(void)
{
    BYTE c;
    asm mov ah, 08h; /* read key without echo */
    asm int 21h;
    asm mov c,al;

    if(c >= 'a') {
	c -= ('a' - 'A'); /* uppercase the character */
    }
    return c;
}

/*  LocalWords:  Codepage Args codepages codepage cd
 */
