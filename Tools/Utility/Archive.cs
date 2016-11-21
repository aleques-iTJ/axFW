using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace bnrToolkit
{
    public class Archive
    {
        public enum PathFlags : ushort
        {
            IsFolder        = 0,
            IsCompressed    = 1
        }

        // 22 bytes total
        // 10
        public byte[] Identifier    = Encoding.ASCII.GetBytes("bnrA"); // Should be 4 bytes
        public byte Version         = 1;
        public byte HeaderSize      = 22;
        public uint FileSize        = 0;

        // 12
        public uint TableSize       = 0;
        public uint DescriptorCount = 0;
        public ushort FolderCount   = 0;
        public ushort FileCount     = 0;

        private List<EntryDescriptor> ContentTable = new List<EntryDescriptor>();

        public class EntryDescriptor
        {
            // 10 bytes total as of version 1
            public PathFlags Flags      = 0;
            public int UncompressedSize = 0;        // The original size of whatever it is
            public int CompressedSize   = 0;        // The size after compression
        }
    }
}