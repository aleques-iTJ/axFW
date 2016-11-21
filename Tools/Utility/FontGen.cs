using System;
using System.Text;
using System.Drawing;
using System.Drawing.Text;
using System.Collections.Generic;

namespace bnrToolkit
{
    public struct CharacterSet
    {
        // public static string ASCII = ""
    }

    public struct UVRectangle
    {
        public float X;
        public float Y;
        public float Width;
        public float Height;
    }

    public struct BitmapCharacter
    {
        // What must be provided
        public ushort Character;

        // In Pixels!
        public ushort Width;
        public ushort Height;

        // Combined texture is 0.0f to 1.0f
        public UVRectangle TextureUVRect;

        // Underlying, modifiable bitmap
        public Bitmap Bitmap;


        // One of these days I'll do this not retarded
        // today is not that day
        public override string ToString()
        {
            string tempBuf;

            tempBuf = "Character\t\t: "     + Character.ToString()              + "\r\n\r\n"    +

                      "Width\t\t: "         + Width.ToString()                  + "\r\n"        +
                      "Height\t\t: "        + Height.ToString()                 + "\r\n\r\n"    +

                      "UVRect - X\t: "      + TextureUVRect.X.ToString()        + "\r\n"        +
                      "UVRect - Y\t: "      + TextureUVRect.Y.ToString()        + "\r\n"        +
                      "UVRect - Width\t: "  + TextureUVRect.Width.ToString()    + "\r\n"        +
                      "UVRect - Height\t: " + TextureUVRect.Height.ToString()   + "\r\n";

            return tempBuf;
        }
    }

    public class BitmapFont
    {
        // 18 bytes total, FILE HEADER
        // 10, base header - all bnr file types share this
        private byte[] Identifier   = Encoding.ASCII.GetBytes("bnrF");
        private byte Version        = 1;
        private byte HeaderSize     = 18;
        private uint DataSize       = 0;    // If compressed, it will be uncompressed RGBA

        // 4, core stuff
        // Texture dimensions
        public ushort Width         = 0;
        public ushort Height        = 0;

        // 4 bytes
        // Beginning and end of the character set
        public ushort Start         = 0;
        public ushort End           = 0;


        // Internals
        public Font     Font;
        public int      OutlineThickness    = 1;
        public Color    BackgroundColor     = Color.Magenta;
        public Color    OutlineColor        = Color.Black;
        public Color    TextColor           = Color.White;

        public TextRenderingHint AASetting  = TextRenderingHint.SingleBitPerPixel;

        // Used to track character bitmap location
        private int     nextWidth           = 0;
        private int     nextHeight          = 0;

        private List<BitmapCharacter>   characterTable = new List<BitmapCharacter>();
        private Bitmap                  combinedBitmap;

        //----------------------------------------------------------------

        // This is disgusting
        public override string ToString()
        {
            string tempID = System.Text.Encoding.ASCII.GetString(Identifier);
            string tempBuf;

            tempBuf =   "Identifier\t\t: "      + tempID                + "\r\n" +
                        "Version\t\t: "         + Version.ToString()    + "\r\n" +
                        "HeaderSize\t: "        + HeaderSize.ToString() + "\r\n" +
                        "DataSize\t\t: "        + DataSize.ToString()   + "\r\n\r\n" +

                        "Width\t\t: "           + Width.ToString()      + "\r\n" +
                        "Height\t\t: "          + Height.ToString()     + "\r\n\r\n" +

                        "Start\t\t: "           + Start.ToString()      + "\r\n" +
                        "End\t\t: "             + End.ToString()        + "\r\n";

            return tempBuf;
        }

        public void AddCharacter(char character)
        {
            if (Font == null)
                return;

            if (combinedBitmap == null)
                combinedBitmap = new Bitmap(Width, Height);

            // Need a rough size, we'll get the exact bounding box later
            // For now, create a dummy bitmap to set up a graphics context for measurement
            SizeF size;
            using (Image tempBitmap = new Bitmap(1, 1))
            using (Graphics tempGfx = Graphics.FromImage(tempBitmap))
            {
                size = tempGfx.MeasureString(character.ToString(), Font);
            }

            int width   = (int)Math.Round(size.Width)  + (OutlineThickness * 2) + 1;
            int height  = (int)Math.Round(size.Height) + (OutlineThickness * 2) + 1;

            BitmapCharacter bmChar  = new BitmapCharacter();
            bmChar.Bitmap           = new Bitmap(width, height);
            
            using (Graphics gfxCharacter = Graphics.FromImage(bmChar.Bitmap))
            using (Graphics gfxCombined  = Graphics.FromImage(combinedBitmap))
            {
                gfxCharacter.TextRenderingHint = AASetting;

                gfxCharacter.FillRectangle(new SolidBrush(Color.Aqua), 0, 0, width, height);
                gfxCombined.FillRectangle(new SolidBrush(BackgroundColor), 0, 0, Width, Height);

                if (OutlineThickness > 0)
                {
                    // Using OutlineThickness = 2 as an example
                    // (and a 1px thick font, but it will work for any)

                    // If the outline is drawn with a length of 2, only the upper left parts of the 
                    // font will actually end up outlined (How can 2px cover the character? It can't!) 
                    // So to start, it can be multiplied by 2... but the text still won't fit perfectly inside
                    // This is becuase the actual text is starting at the 3rd pixel, and thus 
                    // only the 4th pixels remains - and would only effectively outline 1px on bottom right parts...
                    // so it needs to be biased by 1 extra pixel. DONE AND DONE.
                    for (int y = 0; y < (OutlineThickness * 2) + 1; y++)
                    {
                        for (int x = 0; x < (OutlineThickness * 2) + 1; x++)
                        {
                            gfxCharacter.DrawString(character.ToString(), Font, new SolidBrush(OutlineColor), x, y);
                        }
                    }
                }

                // Draw the actual text
                gfxCharacter.DrawString(character.ToString(), Font, new SolidBrush(TextColor), OutlineThickness, OutlineThickness);

                // The width and height should be 0 based at this point
                width--;
                height--;

                // FIX ME
		// how did I ever come to implement this with a list???
                List<int> widthRange  = new List<int>();
                List<int> heightRange = new List<int>();

                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        if (bmChar.Bitmap.GetPixel(x, y).ToArgb() != BackgroundColor.ToArgb())
                        {
                            widthRange.Add(x);
                            heightRange.Add(y);
                        }
                    }
                }

                // The character was blank, just use the original measurement
                if (widthRange.Count == 0 && heightRange.Count == 0)
                {
                    widthRange.Add(0);
                    widthRange.Add((int)Math.Round(size.Width));
                    heightRange.Add(0);
                    heightRange.Add((int)Math.Round(size.Height));
                }

                // Once this is sorted, we'll have the exact bounding box of the character
                widthRange.Sort();
                heightRange.Sort();

                // + 1 because width/heightRange were 0 based
                Rectangle srcBounds = Rectangle.FromLTRB(widthRange [0],
                                                         heightRange[0],
                                                         widthRange [widthRange.Count  - 1] + 1,
                                                         heightRange[heightRange.Count - 1] + 1);

                // Update the source to the perfect fit
                Bitmap temp = bmChar.Bitmap.Clone(srcBounds, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                bmChar.Bitmap.Dispose();
                bmChar.Bitmap = temp;

                bmChar.Character    = (ushort)character;
                bmChar.Width        = (ushort)srcBounds.Width;
                bmChar.Height       = (ushort)srcBounds.Height;

                characterTable.Add(bmChar);
            } // using
        } // func


        public void BuildTextureSheet()
        {
            for (int i = 0; i < characterTable.Count; i++)
            {
                // erm
                using (Graphics gfx = Graphics.FromImage(combinedBitmap))
                {
                    BitmapCharacter bmChar = characterTable[i];
                    Rectangle dstBounds = Rectangle.FromLTRB(nextWidth,
                                                             nextHeight,
                                                             bmChar.Width  + nextWidth,
                                                             bmChar.Height + nextHeight);

                    // Generate the actual texture coordinates
                    float rcpWidth              = (1.0f / Width);
                    float rcpHeight             = (1.0f / Height);

                    bmChar.TextureUVRect.X      = nextWidth  * rcpWidth;
                    bmChar.TextureUVRect.Y      = nextHeight * rcpHeight;
                    bmChar.TextureUVRect.Width  = (bmChar.Width  + nextWidth)  * rcpWidth;
                    bmChar.TextureUVRect.Height = (bmChar.Height + nextHeight) * rcpHeight;

                    gfx.DrawImageUnscaled(bmChar.Bitmap, dstBounds);

                    nextWidth += bmChar.Width;
                    if (nextWidth > (Width - bmChar.Width))
                    {
                        nextHeight += bmChar.Height;
                        nextWidth = 0;
                    }
                }
            }

            // Update the internal bitmap size
            DataSize = (uint)(Width * Height) * 4;

            // Update the first and last characters, ascii code wise
            characterTable.Sort(delegate (BitmapCharacter x, BitmapCharacter y)
            {
                return (x.Character.CompareTo(y.Character));
            });

            Start   = characterTable[0].Character;
            End     = characterTable[characterTable.Count - 1].Character;
        }

        public Image GetImage()
        {
            //nextWidth   = 0;
           // nextHeight  = 0;

            return combinedBitmap;            
        }

        public void Clear()
        {
            nextWidth   = 0;
            nextHeight  = 0;

            characterTable.Clear();
        }
    }
}