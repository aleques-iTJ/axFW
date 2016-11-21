using System;
using System.Runtime.InteropServices;

namespace bnrToolkit
{
    // Wrapper around a Win32 DIB Section, used for "fast" bitmap drawing
    unsafe class DIBSection
    {
        [DllImport("bnrDIB.dll")]
        private static extern void CreateDIB(ref DIBInfo dibInfo);

        [DllImport("bnrDIB.dll")]
        private static extern void DestroyDIB(ref DIBInfo dibInfo);

        [DllImport("bnrDIB.dll")]
        private static extern void DrawDIB(ref DIBInfo dibInfo);

        [StructLayout(LayoutKind.Sequential)]
        public struct DIBInfo
        {
            // What must be provided
            public IntPtr   Hwnd;
            public int      Width;
            public int      Height;

            // Grapes
            public IntPtr   Data;

            // What's handled internally
            private IntPtr hdc;
            private IntPtr hdcMem;
            private IntPtr dib;
            private IntPtr hbmOld;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct DIBColor
        {
            public DIBColor(uint composite)
            {
                this.Blue       = 0;
                this.Green      = 0;
                this.Red        = 0;
                this.Alpha      = 0;
                this.Composite  = composite;
            }

            [FieldOffset(0)]
            public byte Blue;

            [FieldOffset(1)]
            public byte Green;

            [FieldOffset(2)]
            public byte Red;

            [FieldOffset(3)]
            public byte Alpha;

            [FieldOffset(0)]
            public uint Composite;
        }

        DIBInfo dibInfo;

        public DIBSection(IntPtr hwnd, int width, int height)
        {
            dibInfo         = default(DIBInfo);
            dibInfo.Hwnd    = hwnd;
            dibInfo.Width   = width;
            dibInfo.Height  = height;

            CreateDIB(ref dibInfo);
        }

        ~DIBSection()
        {
            DestroyDIB(ref dibInfo);
        }

        public IntPtr GetData()
        {
            return dibInfo.Data;
        }

        public void* GetDataUnsafe()
        {
            return dibInfo.Data.ToPointer();
        }

        public void Draw()
        {
            DrawDIB(ref dibInfo);
        }

        public void SetPixel(int x, int y, DIBColor color)
        {
            if ((x | y) >= 0 && x < dibInfo.Width && y < dibInfo.Height) // Check bounds
            {
                // ((alpha * (src - dest)) >> 8) + dst
                byte alpha = color.Alpha;
                uint* dst  = (uint*)dibInfo.Data.ToPointer() + ((y * dibInfo.Width) + x);

                if (alpha == 0xFF) { *dst = color.Composite;        return; }
                if (alpha == 0x00) { /* Nothing, it's invisible */  return; }

                else
                {
                    uint dstAG = *dst	            & 0xFF00FF00;
                    uint dstRB = *dst	            & 0x00FF00FF;
                    uint srcAG = color.Composite	& 0xFF00FF00;
                    uint srcRB = color.Composite	& 0x00FF00FF;			

                    dstAG = srcAG - dstAG;
                    dstRB = srcRB - dstRB;			

                    dstAG *= alpha;
                    dstRB *= alpha;
                    dstAG >>= 8; // Divide by 256
                    dstRB >>= 8; // This isn't correct, since it _should_ be 0-255, but it's fast

                    dstAG = dstAG & 0xFF00FF00;
                    dstRB = dstRB & 0x00FF00FF;
                    *dst  = ((dstAG | dstRB)) + *dst;
                }
            }
        }
    }
}