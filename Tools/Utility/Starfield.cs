using System;

namespace bnrToolkit
{
    class Starfield
    {
        private struct Star
        {
            public float    x;
            public float    y;
            public float    speed;
            public uint     color;
        }

        private Star[] stars;
        private Random rng;

        private int width;
        private int height;

        private const int starSpeed = 4; // + 1 to avoid 0 div

        public Starfield(int width, int height, int numberOfStars)
        {
            stars   = new Star[numberOfStars];
            rng     = new Random();

            this.width  = width;
            this.height = height;

            // Generate the stars
            for (int i = 0; i < stars.Length; i++)
            {
                stars[i].x      = (float)rng.Next(width);
                stars[i].y      = (float)rng.Next(height);
                stars[i].speed  = (float)rng.NextDouble() * starSpeed;

                uint rngColor   = (uint)rng.Next();
                rngColor       &= 0x00FFFFFF; // Mask out the color and set the alpha to 255
                rngColor       |= 0xFF000000;
                stars[i].color  = rngColor;
            }
        }

        public void UpdateAndDraw(ref DIBSection dibSection)
        {
            DIBSection.DIBColor tailColor = new DIBSection.DIBColor();

            for(int i = 0; i < stars.Length; i++)
		    {
                // Make a tail for the stars
                for(int j = 0; j < stars[i].speed * 4; j++)
			    {
                    // Grab the star color to use in the tail
                    tailColor.Alpha     = 0; // oh my god compiler shut up
                    tailColor.Composite = stars[i].color;

                    // Make slower (for the illusion of being further away) stars dimmer
                    float tintFactor    = starSpeed / stars[i].speed;
                    byte whatIsThisCrap = (byte)tintFactor;
                    tailColor.Alpha     /= whatIsThisCrap;  
                    tailColor.Alpha     /= (byte)(j + 1);   // Skew by 1 to avoid the zero divide

                    dibSection.SetPixel((int)stars[i].x + j, (int)stars[i].y, tailColor);
			    }

                if(stars[i].x <= 0)
				    stars[i].x = width; // Reset a star that's moved out bounds
			    else
				    stars[i].x -= stars[i].speed;
            }

            dibSection.Draw();
        }
    }
}