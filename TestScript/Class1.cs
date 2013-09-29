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

namespace TestScript
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Windows.Forms;

    public class TestScript1 : EasyMod.Script
    {
        System.Drawing.Point cursor_pos;
        EasyMod.DrawableRectangle rect;
        EasyMod.DrawableRectangle rect2;
        EasyMod.DrawableRectangle rect3;
        EasyMod.DrawableText text1;
        EasyMod.DrawableFont font;

        public TestScript1(EasyMod.EasyModApi ApiInterface) : base(ApiInterface)
        {
            this.description = "A testing script :-)";

            this.tickInterval = 1;

            this.KeyUp += new EasyMod.KeyEventHandler(TestScript1_KeyUp);
            this.tick += new EasyMod.TickEventHandler(TestScript1_tick);
            this.KeyWord += new EasyMod.KeywordEventHandler(TestScript1_KeyWord);
            ApiInterface.IO.AddTriggerText("scriptkeyword");

            // IntPtr curr_window_handle = System.Diagnostics.Process.GetCurrentProcess().MainWindowHandle;
            this.rect = new EasyMod.DrawableRectangle(new EasyMod.Vector2(0, 0), 400, 400, System.Drawing.Color.FromArgb(100, 0, 255, 255));
            this.rect2 = new EasyMod.DrawableRectangle(new EasyMod.Vector2(0, 0), 200, 50, System.Drawing.Color.FromArgb(255, 255, 0, 0));
            this.rect3 = new EasyMod.DrawableRectangle(new EasyMod.Vector2(0, 0), 50, 200, System.Drawing.Color.FromArgb(255, 0, 0, 255));

            this.font = new EasyMod.DrawableFont("Lucida Console", 15, false);

            this.text1 = new EasyMod.DrawableText("Hello, World!\nNew line", this.font, new EasyMod.Vector2(0, 0), System.Drawing.Color.White);

            EasyMod.VisibilityInfo vis_info = this.rect.VisInfo;

            vis_info.DepthValue -= 10;
            this.rect2.VisInfo = vis_info;

            vis_info.DepthValue = 50;
            this.rect3.VisInfo = vis_info;

            vis_info.DepthValue = 20;
            this.text1.VisInfo = vis_info;

            bool font_reg = this.api.Graphics.RegisterFont(this.font);

            System.Console.WriteLine("Font registered: " + font_reg);
            
            this.api.Graphics.RegisterDrawable(this.rect);
            this.api.Graphics.RegisterDrawable(this.rect2);
            this.api.Graphics.RegisterDrawable(this.rect3);
            this.api.Graphics.RegisterDrawable(this.text1);
        }

        void TestScript1_KeyWord(object sender, EasyMod.KeywordEventArgs e)
        {
            if (e.Keyword == "scriptkeyword")
            {
                Console.WriteLine("Yay!");
                // What happens if we try to draw now..?
                //this.api_interface.Graphics.DrawRectangle(new EasyMod.Vector2(10, 10), 400, 400, System.Drawing.Color.FromArgb(100, 0, 255, 255));
            }
        }

        void TestScript1_tick(object sender, System.Threading.Thread CurrentThread)
        {
            this.cursor_pos = System.Windows.Forms.Cursor.Position;
            System.Drawing.Rectangle windowrect = this.api.Env.MainWindow.GetSizeAndPos();
            System.Drawing.Rectangle drawrect = this.api.Env.MainWindow.GetDrawableArea();

            this.cursor_pos.X -= windowrect.X;
            this.cursor_pos.Y -= windowrect.Y;

            this.rect.X = this.cursor_pos.X;
            this.rect.Y = this.cursor_pos.Y;

            this.rect2.X = this.cursor_pos.X - 30;
            this.rect2.Y = this.cursor_pos.Y;

            this.rect3.X = this.cursor_pos.X - 10;
            this.rect3.Y = this.cursor_pos.Y;

            this.text1.Position = new EasyMod.Vector2(10, 10);

            /*
            this.text1.Text = "# of windows: " + this.api_interface.Env.Windows.Count.ToString() + "\n";

            for (int i = 0; i < this.api_interface.Env.Windows.Count; i++)
            {
                this.text1.Text += i.ToString() + ": \"" + this.api_interface.Env.Windows[i].GetCaption() + "\"\n";
            }
             */

            this.text1.Text = string.Empty;

            for (int i = 0; i < this.api.Env.Windows.Count; i++)
            {
                EasyMod.WindowApi w = this.api.Env.Windows[i];

                this.text1.Text += "\"" + w.GetCaption() + "\":\n";
                this.text1.Text += "Window pos: (" + w.GetSizeAndPos().X.ToString() + "; " + w.GetSizeAndPos().Y.ToString() + ")" + "\n"
                              + "Window size: (" + w.GetSizeAndPos().Width.ToString() + "; " + w.GetSizeAndPos().Height.ToString() + ")" + "\n"
                              + "Draw area: (" + w.GetDrawableArea().Width.ToString() + "; " + w.GetDrawableArea().Height.ToString() + ")" + "\n\n";
            }

            

            

            //this.cursor_pos  = System.Windows.Forms.Form.ActiveForm.PointToClient(System.Windows.Forms.Cursor.Position);
            //this.cursor_pos = System.Windows.Forms.Cursor.Position;
        }

        void TestScript1_KeyUp(object sender, EasyMod.SerializableKeyEventArgs e)
        {
            // Console.WriteLine("Key " + e.Key.ToString() + " up!");
        }
    }
}
