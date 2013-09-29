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
    using System.Drawing;
    using System.Linq;
    using System.Text;
    using System.Windows.Forms;

    using EasyMod;

    public struct ConsoleInput
    {
        public String Name;
        public ConsoleInputType InputType;
        public IList<String> Args;
    }

    public class ConsoleScript : Script, IConsole
    {
        // --- Helpers
        private ConsoleGraphics my_graphics;
        private Parser my_parser;

        // --- Storage      
        private List<String> log_lines;
        private List<String> old_inputs;

        private string current_input;

        // --- Parameters
        private UInt32 max_log_lines;
        private UInt32 max_old_inputs;

        // --- State variables
        private bool active;

        private int old_input_pos;

        public ConsoleScript(EasyModApi ApiInterface)
            : base(ApiInterface)
        {
            
            this.my_parser = new Parser();

            this.log_lines = new List<String>();
            this.old_inputs = new List<String>();

            this.max_log_lines = 250;
            this.max_old_inputs = 20;

            this.old_input_pos = 1;

            this.current_input = string.Empty;

            this.active = false;

            
            DrawableFont console_font = new DrawableFont("Lucida Console", 17, false);
            
            Console.WriteLine("Registering font...");
            bool success = this.api.Graphics.RegisterFont(console_font);
            Console.WriteLine("Done.");
            if (!success)
            {
                Console.WriteLine("Registering font failed :-(");
            }

            this.my_graphics = new ConsoleGraphics(ApiInterface,
                                                   console_font,
                                                   Color.FromArgb(75, 0, 0, 255),
                                                   Color.FromArgb(75, 100, 100, 255),
                                                   Color.FromArgb(170, 255, 255, 255),
                                                   Color.FromArgb(255, 255, 255, 255),
                                                   Color.FromArgb(255, 190,   190, 255),
                                                   Color.FromArgb(170, 190, 190, 255));
            this.KeyDown += new EasyMod.KeyEventHandler(this.ConsoleScript_KeyDown);

            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("consoleinfo", "Lists information on the current console", 0, new ConsoleCmdHandler(this.cmd_consoleinfo)));
            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("echo", "Echos back the input", 1, new ConsoleCmdHandler(this.cmd_echo)));
            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("console_log_bgcolour", "Sets the console bg colour. Usage: !console_log_bgcolour <A> <R> <G> <B>", 4, new ConsoleCmdHandler(this.cmd_loglinesbgcolour)));
            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("console_input_bgcolour", "Sets the console bg colour. Usage: !console_input_bgcolour <A> <R> <G> <B>", 4, new ConsoleCmdHandler(this.cmd_inputbgcolour)));
            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("console_input_textcolour", "Sets the console bg colour. Usage: !console_input_textcolour <A> <R> <G> <B>", 4, new ConsoleCmdHandler(this.cmd_inputtextcolour)));
            success = this.api.Console.RegisterConsoleCommand(new ConsoleCommand("console_log_textcolour", "Sets the console bg colour. Usage: !console_log_textcolour <A> <R> <G> <B>", 4, new ConsoleCmdHandler(this.cmd_logtextcolour)));

            Console.WriteLine("Managed to register console commands: " + success.ToString());
            
        }

        protected void cmd_loglinesbgcolour(object sender, SerializableConsoleCmdEventArgs e)
        {
            try
            {
                this.my_graphics.LoglinesBackgroundColour = System.Drawing.Color.FromArgb(int.Parse(e.Arguments[0]),
                                                                                                           int.Parse(e.Arguments[1]),
                                                                                                           int.Parse(e.Arguments[2]),
                                                                                                           int.Parse(e.Arguments[3]));
            }
            catch
            {
                this.PrintLine("invalid input! :-(");
            }
        }

        protected void cmd_logtextcolour(object sender, SerializableConsoleCmdEventArgs e)
        {
            try
            {
                this.my_graphics.LogTextColour = System.Drawing.Color.FromArgb(int.Parse(e.Arguments[0]),
                                                                                                           int.Parse(e.Arguments[1]),
                                                                                                           int.Parse(e.Arguments[2]),
                                                                                                           int.Parse(e.Arguments[3]));
            }
            catch
            {
                this.PrintLine("invalid input! :-(");
            }
        }

        protected void cmd_inputbgcolour(object sender, SerializableConsoleCmdEventArgs e)
        {
            try
            {
                this.my_graphics.InputBackgroundColour = System.Drawing.Color.FromArgb(int.Parse(e.Arguments[0]),
                                                                                                           int.Parse(e.Arguments[1]),
                                                                                                           int.Parse(e.Arguments[2]),
                                                                                                           int.Parse(e.Arguments[3]));
            }
            catch
            {
                this.PrintLine("invalid input! :-(");
            }
        }

        protected void cmd_inputtextcolour(object sender, SerializableConsoleCmdEventArgs e)
        {
            try
            {
                this.my_graphics.InputTextColour = System.Drawing.Color.FromArgb(int.Parse(e.Arguments[0]),
                                                                                                           int.Parse(e.Arguments[1]),
                                                                                                           int.Parse(e.Arguments[2]),
                                                                                                           int.Parse(e.Arguments[3]));
            }
            catch
            {
                this.PrintLine("invalid input! :-(");
            }
        }

        protected void cmd_echo(object sender, SerializableConsoleCmdEventArgs e)
        {
            Console.WriteLine("OUT: echo with argc: " + e.Arguments.Count.ToString());

            if (e.Arguments.Count != 0)
            {
                this.PrintLine(e.Arguments[0]);
                //Console.WriteLine("echo: " + e.Arguments[0]);
            }
        }

        protected void cmd_consoleinfo(object sender, SerializableConsoleCmdEventArgs e)
        {
            Console.WriteLine("ConsoleScript by tcpie");
            this.PrintLine("ConsoleScript by tcpie");
        }

        protected void ConsoleScript_KeyDown(object sender, SerializableKeyEventArgs e)
        {
            if (!this.active)
            {
                return;
            }

            if (this.handleSpecialInputKeys(e))
            {
                return;
            }

            int eff_index = this.my_graphics.CaretPos; // (this.my_graphics.CaretPos <= 0) ? 0 : this.my_graphics.CaretPos - 1;

            string start = this.current_input.Substring(0, eff_index);

            this.current_input = start + e.KeyChar + this.current_input.Substring(eff_index);
            this.my_graphics.setInputLine(this.current_input);
            this.my_graphics.CaretPos++;
        }

        #region protected helper fns
        protected bool handleSpecialInputKeys(SerializableKeyEventArgs e)
        {
            if (e.Key == Keys.Enter)
            {
                string input = this.current_input;
                this.current_input = string.Empty;
                this.my_graphics.setInputLine(this.current_input);
                this.my_graphics.CaretPos = 0;

                this.HandleInput(input);

                this.addInput(input);

                return true;
            }

            if (e.Key == Keys.Subtract && e.Shift)
            {
                this.my_graphics.NumLogLines -= 3;

                return true;
            }

            if (e.Key == Keys.Add && e.Shift)
            {
                this.my_graphics.NumLogLines += 3;

                return true;
            }

            if (e.Key == Keys.Home)
            {
                this.my_graphics.CaretPos = 0;

                return true;
            }

            if (e.Key == Keys.End)
            {
                this.my_graphics.CaretPos = this.current_input.Length + 2;

                return true;
            }

            if (e.Key == Keys.PageUp)
            {
                if (e.Shift)
                {
                    this.my_graphics.LogHistoryPos += 3;
                }
                else
                {
                    this.my_graphics.LogHistoryPos++;
                }

                return true;
            }

            if (e.Key == Keys.PageDown)
            {
                if (e.Shift)
                {
                    this.my_graphics.LogHistoryPos -= 3;
                }
                else
                {
                    this.my_graphics.LogHistoryPos--;
                }

                return true;
            }

            if (e.Key == Keys.Back)
            {
                int remove_index = this.my_graphics.CaretPos - 1;

                if (remove_index >= 0 && remove_index < this.current_input.Length)
                {
                    this.current_input = this.current_input.Remove(remove_index, 1);
                    this.my_graphics.setInputLine(this.current_input);
                    this.my_graphics.CaretPos--;
                }

                return true;
            }

            if (e.Key == Keys.Delete)
            {
                int remove_index = this.my_graphics.CaretPos;

                if (remove_index >= 0 && remove_index < this.current_input.Length)
                {
                    this.current_input = this.current_input.Remove(remove_index, 1);
                    this.my_graphics.setInputLine(this.current_input);
                }

                return true;
            }

            if (e.Key == Keys.Left)
            {
                this.my_graphics.CaretPos--;

                return true;
            }

            if (e.Key == Keys.Right)
            {
                this.my_graphics.CaretPos++;

                return true;
            }

            if (e.Key == Keys.Up)
            {
                if (e.Shift)
                {
                    this.old_input_pos = -1;
                    this.current_input = string.Empty;

                    return true;
                }

                this.old_input_pos--;

                if (this.old_input_pos < -1)
                {
                    this.old_input_pos = this.old_inputs.Count - 1;
                    this.current_input = this.old_inputs[this.old_input_pos];

                }
                else if (this.old_input_pos < 0)
                {
                    this.old_input_pos = -1;
                    this.current_input = string.Empty;
                }
                else
                {
                    this.current_input = this.old_inputs[this.old_input_pos];
                }

                this.my_graphics.setInputLine(this.current_input);
                this.my_graphics.CaretPos = this.current_input.Length + 2;

                return true;
            }

            if (e.Key == Keys.Down)
            {
                if (e.Shift)
                {
                    this.old_input_pos = this.old_inputs.Count;
                    this.current_input = string.Empty;

                    return true;
                }

                this.old_input_pos++;

                if (old_input_pos > this.old_inputs.Count)
                {
                    this.old_input_pos = 0;
                    this.current_input = this.old_inputs[this.old_input_pos];
                }
                else if (this.old_input_pos > this.old_inputs.Count - 1)
                {
                    this.old_input_pos = this.old_inputs.Count;
                    this.current_input = string.Empty;
                }
                else
                {
                    this.current_input = this.old_inputs[this.old_input_pos];
                }

                this.my_graphics.setInputLine(this.current_input);
                this.my_graphics.CaretPos = this.current_input.Length + 2;

                return true;
            }

            if (e.KeyChar == '`' || e.KeyChar == '~')
            {
                // Console should be closed down by Controller
                return true;
            }

            if (!char.IsDigit(e.KeyChar) && 
                !char.IsLetter(e.KeyChar) && 
                !char.IsNumber(e.KeyChar) && 
                !char.IsWhiteSpace(e.KeyChar) && 
                !char.IsPunctuation(e.KeyChar) &&
                !char.IsSeparator(e.KeyChar) &&
                e.KeyChar != '$')
            {
                // We can ignore this input

                return true;
            }

            return false;
        }

        protected void addInput(string line)
        {
            if (line == string.Empty)
            {
                return;
            }

            if (this.old_inputs.Count >= this.max_old_inputs)
            {
                this.old_inputs.RemoveAt(0);
            }

            this.old_inputs.Add(line);
            //this.old_input_pos = this.old_inputs.Count + 1;
        }

        protected void onConsoleCmd(ConsoleInput Cmd)
        {
            Console.WriteLine("onConsoleCmd!");
            SerializableConsoleCmdEventArgs args = new SerializableConsoleCmdEventArgs(Cmd.Name, Cmd.InputType, Cmd.Args.ToArray());
            this.ConsoleCmd(this, args);
        }

        protected void addLogLines(String[] Lines)
        {
            for (int i = 0; i < Lines.Length; i++)
            {
                this.addLogLine(Lines[i]);
            }
        }

        protected void addLogLine(String Line)
        {
            if (this.log_lines.Count >= this.max_log_lines)
            {
                this.log_lines.RemoveAt(0);
            }

            this.log_lines.Add(Line);

            this.my_graphics.addLineToLogLines(Line);
        }

        protected string makeInputErrorMsg(String Message)
        {
            return "ERROR IN INPUT: " + Message;
        }

        #endregion

        #region public interface
        public event ConsoleCmdHandler ConsoleCmd;

        public bool IsOpen()
        {
            return this.active;
        }

        public void HandleInput(string Input)
        {
            IList<ConsoleInput> inputs = this.my_parser.ParseString(Input);

            if (inputs == null)
            {
                return;
            }

            for (int i = 0; i < inputs.Count; i++)
            {
                this.onConsoleCmd(inputs[i]);
            }
        }

        public void PrintLine(String Line)
        {
            Line = Line.Replace("\r", "\\r");
            Line = Line.Replace("\n", "\\n");

            this.addLogLine(Line);
        }

        public void Print(String Text)
        {
            Text = Text.Replace("\r", String.Empty);
            String[] lines = Text.Split('\n');

            
            this.addLogLines(lines);
        }

        public void Close()
        {
            if (!this.active)
            {
                return;
            }

            this.active = false;

            this.my_graphics.HideAll();
        }

        public void Open()
        {
            if (this.active)
            {
                return;
            }

            this.active = true;
            this.my_graphics.ShowAll();
        }
        #endregion
    }
}
