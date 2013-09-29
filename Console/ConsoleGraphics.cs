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

    using EasyMod;

    class ConsoleGraphics
    {
        private EasyModApi api;
        private DrawableFont font;

        private DrawableRectangle scrollbar;
        private DrawableRectangle loglines_bg;
        private DrawableRectangle input_bg;
        private DrawableText caret;

        private List<string> log_storage;
        private List<DrawableText> log_lines;
        private DrawableText display_input_line;

        private Color loglines_text_colour;
        private Color input_text_colour;

        private int max_num_lines;
        private int chars_per_line;

        // --- Variables
        private int curr_num_vis_loglines;

        private int curr_linenum;

        private int caret_pos;

        private int display_caret_pos;

        private string real_input_line;

        private int input_line_sidescroll;

        public ConsoleGraphics(EasyModApi ApiInterface, DrawableFont ConsoleFont, Color LoglinesBackgroundColour, Color InputBackgroundColour, Color LoglinesTextColour, Color InputTextColour, Color CaretColour, Color ScrollbarColour)
        {
            this.log_lines = new List<DrawableText>();
            this.log_storage = new List<string>();
            this.caret_pos = 0;
            this.real_input_line = string.Empty;
            this.input_line_sidescroll = 0;

            this.api = ApiInterface;
            this.font = ConsoleFont;

            Rectangle rect = this.api.Env.MainWindow.GetDrawableArea();
         
            this.chars_per_line = (int)((float)rect.Width / (0.85 * (float)this.font.CharWidth));
            this.max_num_lines = (int)((float)rect.Height / (float)this.font.LineHeight);

        
            this.curr_num_vis_loglines = 15;
            this.curr_linenum = 0;

            this.loglines_text_colour = LoglinesTextColour;
            this.input_text_colour = InputTextColour;

            this.loglines_bg = new DrawableRectangle(new Vector2(0, 0), (uint)(rect.Width * 1.5), (uint)this.curr_num_vis_loglines * (uint)this.font.LineHeight, LoglinesBackgroundColour);
            this.input_bg = new DrawableRectangle(new Vector2(0, this.curr_num_vis_loglines * this.font.LineHeight), (uint)(rect.Width * 1.5), 1 * (uint)this.font.LineHeight, InputBackgroundColour);
            this.scrollbar = new DrawableRectangle(new Vector2(rect.Width, this.curr_num_vis_loglines * this.font.LineHeight - 30), 10, 30, ScrollbarColour);
            this.caret = new DrawableText("_", this.font, new Vector2(10, this.curr_num_vis_loglines * this.font.LineHeight + 0.1 * this.font.LineHeight), CaretColour);//new DrawableRectangle(new Vector2(0, this.curr_num_vis_loglines * this.font.LineHeight), (uint)this.font.CharWidth, (uint)this.font.LineHeight, CaretColour);

          

            this.loglines_bg.VisInfo.DepthValue = 30;
            this.loglines_bg.VisInfo.IsVisible = false;

            this.input_bg.VisInfo.DepthValue = 30;
            this.input_bg.VisInfo.IsVisible = false;

            Console.WriteLine("Console graphics: " + this.chars_per_line.ToString() + " chars / line (char width = " + this.font.CharWidth.ToString() + ")");
            Console.WriteLine("   Drawable area: " + rect.Width.ToString() + " x " + rect.Height.ToString());
            Console.WriteLine("   Max num lines: " + this.max_num_lines.ToString());

            for (int i = 0; i < this.max_num_lines; i++)
            {
                this.log_lines.Add(new DrawableText(string.Empty, this.font, new Vector2(2, i * this.font.LineHeight), loglines_text_colour));
                this.log_lines[i].VisInfo.DepthValue = 20;
                this.log_lines[i].VisInfo.IsVisible = false;

                this.api.Graphics.RegisterDrawable(this.log_lines[i]);
            }

            this.display_input_line = new DrawableText(string.Empty, this.font, new Vector2(10, this.curr_num_vis_loglines * this.font.LineHeight), input_text_colour);
            this.display_input_line.VisInfo.DepthValue = 20;
            this.display_input_line.VisInfo.IsVisible = false;

            this.caret.VisInfo.DepthValue = 25;
            this.caret.VisInfo.IsVisible = false;

            this.scrollbar.VisInfo.DepthValue = 25;
            this.scrollbar.IsVisible = false;

            this.api.Graphics.RegisterDrawable(this.scrollbar);
            this.api.Graphics.RegisterDrawable(this.display_input_line);
            this.api.Graphics.RegisterDrawable(this.caret);
            this.api.Graphics.RegisterDrawable(this.loglines_bg);
            this.api.Graphics.RegisterDrawable(this.input_bg);
        }

        public Color InputTextColour
        {
            get { return this.input_text_colour; }
            set { this.input_text_colour = value; }
        }

        public Color LogTextColour
        {
            get { return this.loglines_text_colour; }
            set { this.loglines_text_colour = value; }
        }

        public Color LoglinesBackgroundColour
        {
            get { return this.loglines_bg.Colour; }
            set { this.loglines_bg.Colour = value; }
        }

        public Color InputBackgroundColour
        {
            get { return this.input_bg.Colour; }
            set { this.input_bg.Colour = value; }
        }

        public int NumLogLines
        {
            get
            {
                return this.curr_num_vis_loglines;
            }

            set
            {
                if (value < 5)
                {
                    value = 5;
                }
                else if (value >= this.max_num_lines)
                {
                    value = this.max_num_lines - 1;
                }

                this.curr_num_vis_loglines = value;

                this.refreshGraphics();
            }
        }

        public int CaretPos
        {
            get
            {
                return this.caret_pos;
            }

            set
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > this.real_input_line.Length)
                {
                    value = this.real_input_line.Length;
                }

                int diff = value - this.caret_pos;

                this.caret_pos = value;

                this.display_caret_pos += diff;

                if (this.display_caret_pos < 0)
                {
                    this.display_caret_pos = 0;
                }
                else if (this.display_caret_pos > this.chars_per_line)
                {
                    this.display_caret_pos = this.chars_per_line;
                }

                this.setInputLineGraphical();

                this.caret.Text = string.Empty;

                for (int i = 0; i < this.display_caret_pos; i++)
                {
                    this.caret.Text += " ";
                }

                this.caret.Text += "_";
            }
        }

        public int LogHistoryPos
        {
            get
            {
                return this.curr_linenum;
            }

            set
            {
                if (value == this.curr_linenum)
                {
                    return;
                }

                if (value < 0)
                {
                    value = 0;
                }
                else if (value >= this.log_storage.Count)
                {
                    value = this.log_storage.Count - 1;
                }

                this.curr_linenum = value;
                this.refreshGraphics();
            }
        }

        protected List<string> splitToScreenWidth(List<string> Lines)
        {
            List<string> ret = new List<string>();

            for (int i = 0; i < Lines.Count; i++)
            {
                ret.AddRange(this.splitToScreenWidth(Lines[i]));
            }

            return ret;
        }

        protected List<string> splitToScreenWidth(string Line)
        {
            List<string> ret = new List<string>();

            while (Line.Length >= this.chars_per_line)
            {
                string sub = Line.Substring(0, this.chars_per_line - 1);
                ret.Add(sub);
                Line = Line.Remove(0, this.chars_per_line - 1);
            }

            ret.Add(Line);

            return ret;
        }

        protected void refreshGraphics()
        {
            for (int i = 0; i < this.log_lines.Count; i++)
            {
                if (this.log_storage.Count == 0)
                {
                    break;
                }

                if (i >= this.curr_num_vis_loglines)
                {
                    this.log_lines[i].Text = string.Empty;

                    continue;
                }

                int j = i + this.log_storage.Count - this.curr_num_vis_loglines - this.curr_linenum;

                if (j < 0)
                {
                    this.log_lines[i].Text = string.Empty;
                }
                else if (j >= this.log_storage.Count)
                {
                    this.log_lines[i].Text = string.Empty;
                }
                else
                {
                    this.log_lines[i].Text = this.log_storage[j];
                }
            }
            
            if (this.curr_linenum == 0)
            {
                this.scrollbar.Y = this.curr_num_vis_loglines * this.font.LineHeight - this.scrollbar.Height; // max pos
            }
            else
            {
                float percent_up = (float)this.curr_linenum / (float)(this.log_storage.Count - 1);
                float minus = 1.0f - percent_up;
                this.scrollbar.Y = (this.curr_num_vis_loglines * this.font.LineHeight - this.scrollbar.Height) * minus;
            }

            this.loglines_bg.Height = (uint)this.curr_num_vis_loglines * (uint)this.font.LineHeight;
            
            this.display_input_line.Position = new Vector2(this.display_input_line.Position.X, this.curr_num_vis_loglines * this.font.LineHeight);
            this.caret.Position = this.display_input_line.Position;
            this.input_bg.UpperLeftCornerPos = new Vector2(this.input_bg.UpperLeftCornerPos.X, this.curr_num_vis_loglines * this.font.LineHeight);
        }

        protected void respondToAddedLog()
        {
            this.curr_linenum = 0;
            this.refreshGraphics();
        }

        protected void setInputLineGraphical()
        {
            // no scrolling needed
            if (this.real_input_line.Length <= this.chars_per_line)
            {
                this.display_input_line.Text = this.real_input_line;
                this.input_line_sidescroll = 0;

                return;
            }

            // Input line too long:
            int start_pos = 0;

            if (this.display_caret_pos == this.chars_per_line) // limit reached
            {
                start_pos = this.caret_pos - this.chars_per_line;
            }
            else if (this.display_caret_pos == 0) // left reached
            {
                start_pos = this.caret_pos;
            }
            else // no change
            {
                start_pos = this.input_line_sidescroll;
            }

            //start_pos = this.caret_pos - this.chars_per_line; //this.real_input_line.Length - this.chars_per_line;           

            this.input_line_sidescroll = start_pos;

            this.display_input_line.Text = this.real_input_line.Substring(this.input_line_sidescroll, this.chars_per_line);

        }

        internal void setInputLine(String Line)
        {
            this.real_input_line = Line;

            this.setInputLineGraphical();
        }

        internal void addLinesToLogLines(List<string> Lines)
        {
            Console.WriteLine("Adding " + Lines.Count.ToString() + " to log.");
            List<string> split_strings = this.splitToScreenWidth(Lines);
            this.log_storage.AddRange(split_strings);

            foreach (string s in split_strings)
            {
                Console.WriteLine("TO LOG: " + s);
            }

            this.respondToAddedLog();
        }

        internal void addLineToLogLines(string Line)
        {
            Console.WriteLine("GRPH-: adding line to backlog: " + Line);
            List<string> split_strings = this.splitToScreenWidth(Line);
            Console.WriteLine("GRPH-: num strings from splitting: " + split_strings.Count.ToString());

            this.log_storage.AddRange(split_strings);
                      
            
            foreach (string s in split_strings)
            {
                Console.WriteLine("TO LOG: " + s);
            }

            this.respondToAddedLog();
        }

        internal void ResetLogLineStorageTo(List<String> LogLines)
        {
            List<string> split_loglines = this.splitToScreenWidth(LogLines);
            this.log_storage.Clear();
            this.log_storage.AddRange(split_loglines);
            this.respondToAddedLog();
        }

        internal void HideAll()
        {
            this.input_bg.IsVisible = false;
            this.loglines_bg.IsVisible = false;

            this.display_input_line.IsVisible = false;
            this.caret.IsVisible = false;
            this.scrollbar.IsVisible = false;

            for (int i = 0; i < this.log_lines.Count; i++)
            {
                this.log_lines[i].IsVisible = false;
            }
        }

        internal void ShowAll()
        {
            Console.WriteLine("ConsoleGraphics.ShowAll()");
            this.input_bg.IsVisible = true;
            this.loglines_bg.IsVisible = true;
            this.caret.IsVisible = true;
            this.display_input_line.IsVisible = true;
            this.scrollbar.IsVisible = true;

            for (int i = 0; i < this.log_lines.Count; i++)
            {
                this.log_lines[i].IsVisible = true;
            }
        }
    }
}
