using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;
using System.IO;

using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using bnrToolkit.Utility;

namespace bnrToolkit.Utility
{
    public class DDSHelper
    {
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public class Format
        {
            public uint Size;
            public uint Flags;
            public uint FourCC;
            public uint RGB_BitCount;
            public uint R_BitMask;
            public uint G_BitMask;
            public uint B_BitMask;
            public uint A_BitMask;
        };

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public class FileHeader
        {
            // Not really part of the header... but alright
            // Should be 128 bytes total, 4 for the identifier and 4 for the header itself
            public uint Magic;

            public uint Size;
            public uint Flags;
            public uint Height;
            public uint Width;
            public uint PitchOrLinearSize;
            public uint Depth;
            public uint MipMapCount;

            public uint Reserved1;
            public uint Reserved2;
            public uint Reserved3;
            public uint Reserved4;
            public uint Reserved5;
            public uint Reserved6;
            public uint Reserved7;
            public uint Reserved8;
            public uint Reserved9;
            public uint Reserved10;
            public uint Reserved11;

            public Format ImageFormat;
            public uint Caps1;
            public uint Caps2;
            public uint Caps3;
            public uint Caps4;

            public uint Reserved12; // Because 11 wasn't enough
        }

        public class File
        {
            public FileHeader   Header      = new FileHeader();
            public MemoryStream ImageData   = new MemoryStream();
        };


        internal File ddsFile;

        public static DDSHelper FromFile(string path)
        {
            // Set up the DDS file
            DDSHelper dds   = new DDSHelper();
            dds.ddsFile     = new File();

            // Read the stupid thing
            byte[] raw  = System.IO.File.ReadAllBytes(path);

            // Header first
            GCHandle    pinnedArray = GCHandle.Alloc(raw, GCHandleType.Pinned);
            IntPtr      ptr         = pinnedArray.AddrOfPinnedObject();
            dds.ddsFile.Header      = (FileHeader)Marshal.PtrToStructure(ptr, typeof(FileHeader));
            pinnedArray.Free();

            // Data next
            dds.ddsFile.ImageData.Capacity = raw.Length - 128;
            dds.ddsFile.ImageData.Write(raw, 128, raw.Length);

            return dds;
        }

        public byte[] GetMipLevel(int level)
        {
            int levelSize = Math.Max(1, (((int)ddsFile.Header.Width + 3) / 4)) * Math.Max(1, (((int)ddsFile.Header.Height + 3) / 4));
            int levelOffset = 0;

            // Assume main surface to begin
            int curLevelSize = levelSize;

            if (level > 0)
            {
                for(int i = 0; i < level; i++)
                {
                    curLevelSize    /= 4;
                    levelOffset     += curLevelSize;
                }
            }

            byte[] mipData = new byte[curLevelSize];
            ddsFile.ImageData.Read(mipData, levelOffset, curLevelSize);

            return mipData;
        }
    }
}