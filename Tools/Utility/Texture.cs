using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using bnrToolkit.Utility;

using System.Runtime.InteropServices;

namespace bnrToolkit
{
    public class Texture
    {
        [DllImport("msvcrt.dll", EntryPoint = "memcpy", CallingConvention = CallingConvention.Cdecl, SetLastError = false)]
        public static extern IntPtr memcpy(IntPtr dest, IntPtr src, UIntPtr count);

        public enum Descriptor : ushort // Some room for expansion...
        {
            // Formats
            RGBA            = 0,        // 32 bpp
            DXT1            = 1 << 1,   // 4 bpp
            DXT5            = 1 << 2,   // 8 bpp
        
            // Filtering types
            Nearest         = 1 << 3,
            Trilinear       = 1 << 4,
            Anisotropic     = 1 << 5,  
 
            // Extra flags
            IsCompressed    = 1 << 6,   // Image data is compressed
            IsFlipped       = 1 << 7,   // Render upside down
            IgnoreMips      = 1 << 8    // Pretend mips don't exist
        }

        [StructLayout(LayoutKind.Sequential)]
        public class Header
        {
            // Base header data
            public byte[]   Identifier  { get; private set; } = Encoding.ASCII.GetBytes("bnrT");
            public byte     Version     { get; private set; } = 1;
            public byte     HeaderSize  { get; private set; } = (byte)Marshal.SizeOf(typeof(Header));
            public uint     DataSize    { get; private set; } = 0;    // If compressed, it's easy to calculate the uncompressed size since the format is known

            // Core atributes
            public ushort   Flags       { get; private set; } = 0;
            public ushort   Width       { get; private set; } = 0;
            public ushort   Height      { get; private set; } = 0;
            public ushort   Depth       { get; private set; } = 0;

            // Extra
            public ushort   NumFrames   { get; private set; } = 0;
            public byte     FrameRate   { get; private set; } = 0;
            public byte     MipMapCount { get; private set; } = 0;
        };


        private List<Bitmap>    bitmaps = new List<Bitmap>();
        private Bitmap          originalBitmap;
        private Bitmap          combinedBitmap;


        public static Texture FromFile(string path)
        {
            // Set up the texture fire
            Texture tex = new Texture();

            // Just trust the file extension for now
            string ext = Path.GetExtension(path);
                
            // DDS files are a special case, as there's no built in support for loading them
            // Because of this, we'll need to parse the file manually
            if (ext.Equals(".dds"))
            {
                DDSHelper dds = DDSHelper.FromFile(path);

                tex.Width    = (ushort)dds.ddsFile.Header.Width;
                tex.Height   = (ushort)dds.ddsFile.Header.Height;
                tex.Depth    = (ushort)dds.ddsFile.Header.Width;

                // FIX THIS WHEN NOT LAZY
                tex.Flags        = (ushort)Descriptor.DXT1;
                tex.MipMapCount  = (byte)dds.ddsFile.Header.MipMapCount;

                // Not finished...
            }

            // Otherwise just let GDI+ handle it and the conversion
            // We'll ultimately just pull RGBA format out of it
            else
            {
                // Read the file and convert it to RGBX
                using (Bitmap temp = new Bitmap(path))
                {
                    tex.originalBitmap = temp.Clone(new Rectangle(0, 0, temp.Width, temp.Height), PixelFormat.Format32bppRgb);
                    tex.combinedBitmap = tex.originalBitmap;
                    //tex.bitmaps.Add(conv);
                }

                // Just use the size of the main surface
                tex.Width   = (ushort)tex.originalBitmap.Width;
                tex.Height  = (ushort)tex.originalBitmap.Height;

                tex.DataSize = (uint)(tex.Width * tex.Height) * 4;
            }               

            return tex;
        }

        public void ToFile(string path, bool compress)
        {
            using(FileStream fileOutput = new FileStream(path, FileMode.Create))
            {
                // Fix the file size first
                // FileSize += HeaderSize;

                // Write the header...
                fileOutput.Write(Identifier, 0, Identifier.Length);
                fileOutput.Write(BitConverter.GetBytes(Version),    0, Marshal.SizeOf(Version));
                fileOutput.Write(BitConverter.GetBytes(HeaderSize), 0, Marshal.SizeOf(HeaderSize));
                fileOutput.Write(BitConverter.GetBytes(DataSize),   0, Marshal.SizeOf(DataSize));

                fileOutput.Write(BitConverter.GetBytes(Width),      0, Marshal.SizeOf(Width));
                fileOutput.Write(BitConverter.GetBytes(Height),     0, Marshal.SizeOf(Height));
                fileOutput.Write(BitConverter.GetBytes(Depth),      0, Marshal.SizeOf(Depth));
                fileOutput.Write(BitConverter.GetBytes((ushort)Flags), 0, Marshal.SizeOf(Flags));

                fileOutput.Write(BitConverter.GetBytes(NumFrames),  0, Marshal.SizeOf(NumFrames));
                fileOutput.Write(BitConverter.GetBytes(FrameRate),  0, Marshal.SizeOf(FrameRate));
                fileOutput.Write(BitConverter.GetBytes(MipMapCount),0, Marshal.SizeOf(MipMapCount));

                // Prepare the image data...
                Rectangle   rect    = new Rectangle(0, 0, combinedBitmap.Width, combinedBitmap.Height);
                BitmapData  data    = combinedBitmap.LockBits(rect, ImageLockMode.ReadWrite, combinedBitmap.PixelFormat);
                byte[]      raw     = new byte[DataSize];
                GCHandle    pinned  = GCHandle.Alloc(raw, GCHandleType.Pinned);
                IntPtr      ptr     = pinned.AddrOfPinnedObject();
                UIntPtr     count   = (UIntPtr)DataSize;
                memcpy(ptr, data.Scan0, count);
                pinned.Free();
                combinedBitmap.UnlockBits(data);

                // .. and write it
                if (compress)
                {
                    //byte[] dataCompressed = LZ4.LZ4Codec.EncodeHC(raw, 0, raw.Length);
                    //fileOutput.Write(dataCompressed, 0, dataCompressed.Length);
                }

                else
                    fileOutput.Write(raw, 0, (int)DataSize);
            }
        }

        public void GenerateMipMaps(InterpolationMode downsampleType)
        {
            // Start fresh
            DestroyMipMaps();      

            // Something should be loaded... is this thing a .dds file?
            if (((Flags & (ushort)Descriptor.RGBA) != (ushort)Descriptor.RGBA))
            {
                // Check the mipmap count...
                if (MipMapCount > 0)
                {
                    // Not implemented

                    // If it's > 0, then we probably have a valid, mipped .dds file already
                    // Just need to transcribe the raw image data to our container...
                    // This should already be done
                }
            }

            // Otherwise it's something else actually directly loadable, and was converted to a bitmap
            else
            {
                // The new size for the bitmap with all the mip data, should be (oldSize * 2) - 1 byte
                int totalWidth  = Width;
                int totalHeight = (int)(Height * 1.5f); // Close enough, just keep it rectangular

                // Create the the working set of the final image
                combinedBitmap = new Bitmap(totalWidth, totalHeight, PixelFormat.Format32bppRgb);

                // ... andddd lock it, we'll be dumping all the mipmaps into it
                Rectangle   rect = new Rectangle(0, 0, combinedBitmap.Width, combinedBitmap.Height);
                BitmapData  data = combinedBitmap.LockBits(rect, ImageLockMode.ReadWrite, combinedBitmap.PixelFormat);

                // Each mip will be 1/4th the size of the previous...
                // Remember that we already have the main surface loaded!
                int mipChainWidth   = Width;
                int mipChainHeight  = Height;
                int currentSize     = (mipChainWidth * mipChainHeight) * 4;   // Total current mip size, 32 bit
                int mipOffset       = 0;                                      // Since we're writing at the end of a given surface

                while (mipChainWidth >= 1 || mipChainHeight >= 1)
                {
                    // Create the image that'll be resized and written to the working image
                    Bitmap mip = new Bitmap(mipChainWidth, mipChainHeight, PixelFormat.Format32bppRgb);

                    // Within the mip data...
                    using (Graphics gfx = Graphics.FromImage(mip))
                    {
                        // Draw the original surface to a smaller one
                        gfx.InterpolationMode = downsampleType;

                        gfx.FillRectangle(Brushes.White, 0, 0, mipChainWidth, mipChainHeight);
                        gfx.DrawImage(originalBitmap, 0, 0, mipChainWidth, mipChainHeight);
                    }

                    bitmaps.Add(mip);
                    MipMapCount++;

                    // Lock it
                    Rectangle mipRect   = new Rectangle(0, 0, mip.Width, mip.Height);
                    BitmapData mipData  = mip.LockBits(mipRect, ImageLockMode.ReadWrite, mip.PixelFormat);

                    // Write da good stuff
                    UIntPtr count = (UIntPtr)currentSize;
                    memcpy(data.Scan0 + mipOffset, mipData.Scan0, count);

                    // Done, make sure to advance the offset, need to always point to the end of the last mip
                    mip.UnlockBits(mipData);
                    mipOffset += currentSize;

                    // Each mip will be 1/4th the size of the previous
                    mipChainWidth   /= 2;
                    mipChainHeight  /= 2;
                    currentSize     /= 4;
                }

                // Done, unlock and carry on
                combinedBitmap.UnlockBits(data);
                DataSize = (uint)(combinedBitmap.Width * combinedBitmap.Height) * 4;
            }
        }

        public void DestroyMipMaps()
        {
            // If something's there, obliterate it before starting over
            if (MipMapCount > 1)
            {
                bitmaps.Clear();
                MipMapCount = 1;

                combinedBitmap  = originalBitmap;
                DataSize = (uint)(combinedBitmap.Width * combinedBitmap.Height) * 4;
            }
        }

        public Image GetImage()
        {
            DataSize = (uint)(combinedBitmap.Width * combinedBitmap.Height) * 4;
            return combinedBitmap;
        }

        public Image FromMip(int level)
        {
            return bitmaps[level];
        }
    }
}