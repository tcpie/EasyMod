/*
Copyright (c) 2013 Stijn "tcpie" Hinterding (contact: contact at tcpie dot eu)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published 
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

namespace Console
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    using EasyMod;
    // Possible syntax:
    // Anything "normal" is considered a variable. For example:
    // rfb_godmode 1
    //      In this example, the variable "rfb_godmode" gets assigned the value 1
    //
    // Any console command is prepended with a "!". For example:
    // !teleport 900 30 10
    // !settime 23 30
    //      In this last example, the console command "settime" is called, with parameters "23" and "30"
    //
    // If something is surrounded by quotation marks ("), it is considered to be 'one'. For example:
    // my_name "I am awesome"
    //      In this example, the variable "my_name" gets assigned the value "I am awesome"
    //
    //  Or:
    //
    // !killplayer "the great n00b"
    //
    // Note that the quotation marks are NOT included into the parameter!
    //
    // At the moment, only SINGLE commands are allowed. Which means: the result of a command cannot be assigned to a variable (sorry :-/)
    //
    // The ";" character denotes the end of the command, but is not mandatory. It does allow, however, the user to issue
    // multiple commands on one line. For example:
    //
    //  !suicide; my_name "I am lame"; !say "Sorry guys, I let you down"
    //
    class Parser
    {
        public Parser()
        {

        }

        protected List<string> getInputLines(String input)
        {
            List<string> string_inputs = new List<string>();

            bool is_string = false;
            int last_start_index = 0;

            for (int i = 0; i < input.Length; i++)
            {
                if (input[i] == '"')
                {
                    is_string = !is_string;
                }

                if (!is_string && input[i] == ';' ||            // End of command
                    !is_string && i == input.Length - 1)        // End of input     (note: we do NOT handle strings which are open at the end of input or end of command. These are ignored.)
                {
                    string substring = (input[i] == ';') ? input.Substring(last_start_index, i - last_start_index) : input.Substring(last_start_index, i - last_start_index + 1);
                    substring = substring.Trim();

                    if (substring == string.Empty)
                    {
                        continue;
                    }

                    string_inputs.Add(substring);
                    last_start_index = i + 1;
                }
            }

            return string_inputs;
        }

        protected bool getConsoleInputFromLine(String input, out ConsoleInput output)
        {
            output = new ConsoleInput();

            bool input_start = true;
            bool in_string = false;
            bool in_input_name = false;
            bool in_input_arg = false;
            bool in_delim = false;
            int delim_start_index = -10;

            List<string> curr_args = new List<string>();
            string curr_arg = string.Empty;
            ConsoleInputType curr_type = ConsoleInputType.Command;
            string curr_name = string.Empty;
            int handled_arg_count = 0;

            char delim_char = '\\';

            for (int i = 0; i < input.Length; i++)
            {
                if (i >= delim_start_index + 2)
                {
                    in_delim = false;
                }

                if (input[i] == delim_char)
                {
                    if (!in_delim)   // Two slashes: '\' is treated as regular slash, thus we ignore
                    {
                        in_delim = true;
                        delim_start_index = i;
                        continue;
                    }
                }

                if (input[i] == '"' && !in_delim)
                {
                    if (in_string)
                    {
                        // Name end
                        if (in_input_name)
                        {
                            curr_name = curr_name.Trim();

                            if (curr_name != string.Empty)
                            {
                                in_input_name = false;
                                in_input_arg = true;
                            }
                        }
                        else if (in_input_arg) // Arg end
                        {
                            curr_arg = curr_arg.Trim();
                        
                            if (curr_arg != string.Empty)
                            {
                                curr_args.Add(curr_arg);
                                curr_arg = string.Empty;
                                handled_arg_count++;
                            }
                        }
                    }

                    in_string = !in_string;
                    continue;
                }

                if (input_start)
                {
                    input_start = false;
                    curr_type = (input[i] == '!' && !in_delim) ? ConsoleInputType.Command : ConsoleInputType.Variable;
                    in_input_name = true;

                    if (curr_type == ConsoleInputType.Variable)
                    {
                        curr_name += input[i];
                    }

                    continue;
                }

                if (in_input_name)
                {
                    if (in_string || 
                        input[i] != ' ' ||
                        (input[i] == ' ' && in_delim))
                    {
                        curr_name += input[i];
                        continue;
                    }
                    else if (input[i] == ' ' && !in_delim)
                    {
                        curr_name = curr_name.Trim();

                        if (curr_name != string.Empty)
                        {
                            in_input_name = false;
                            in_input_arg = true;
                        }

                        continue;
                    }
                }

                if (in_input_arg)
                {
                    if (in_string || 
                        input[i] != ' ' ||
                        (input[i] == ' ' && in_delim))
                    {
                        curr_arg += input[i];
                    }
                    
                    if (((input[i] == ' ' && !in_delim) || i == input.Length - 1) 
                        && !in_string)
                    {
                        curr_arg = curr_arg.Trim();

                        if (curr_arg != string.Empty)
                        {
                            curr_args.Add(curr_arg);
                            curr_arg = string.Empty;
                            handled_arg_count++;
                        }
                    }
                }
            }

            if (in_string ||                                                        // String was not closed
                curr_name == string.Empty)                                                // No name specified
            {
                Console.WriteLine("Input failed. in_string: " + in_string + " current name: " + curr_name);
                return false;
            }

            output.InputType = curr_type;
            output.Args = curr_args;
            output.Name = curr_name;

            return true;
        }

        public IList<ConsoleInput> ParseString(String input)
        {
            Console.WriteLine("PRS: Parsing input: \"" + input + "\"");

            input = input.Trim();

            List<string> input_lines = this.getInputLines(input);

            Console.WriteLine("Found " + input_lines.Count.ToString() + " lines:");

            for (int i = 0; i < input_lines.Count; i++)
            {
                Console.WriteLine(input_lines[i]);
            }

            List<ConsoleInput> inputs = new List<ConsoleInput>();

            Console.WriteLine("Inputs: ");

            for (int i = 0; i < input_lines.Count; i++)
            {
                ConsoleInput temp_input;

                bool ret = this.getConsoleInputFromLine(input_lines[i], out temp_input);

                if (ret)
                {
                    Console.WriteLine("\"" + temp_input.Name + "\": type: " + temp_input.InputType.ToString() + " args: (" + temp_input.Args.Count.ToString() + ")");

                    for (int j = 0; j < temp_input.Args.Count; j++)
                    {
                        Console.WriteLine(temp_input.Args[j]);
                    }

                    inputs.Add(temp_input);
                }
                else
                {
                    Console.WriteLine("Line \"" + input_lines[i] + "\" is invalid!");
                }
            }

            return inputs;
            
        }
    }
}
