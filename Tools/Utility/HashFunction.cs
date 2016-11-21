using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bnrToolkit.Utility
{
    class HashFunction
    {
        // The hasher used, currently FNV
        public class FNV
        {
            private static readonly int     Prime       = 16777619;
            private static readonly uint    OffsetBasis = 2166136261;

            public static uint Hash(string str)
            {
                uint hash = OffsetBasis;

                foreach (char c in str)
                {
                    hash = hash ^ c;
                    hash = hash * (uint)Prime;
                }

                return hash;
            }
        }

        public class MD5
        {
            public static byte[] Hash(string str)
            {
                var     md5     = System.Security.Cryptography.MD5.Create();
                byte[]  input   = System.Text.Encoding.ASCII.GetBytes(str);
                return md5.ComputeHash(input);
            }

            /* public static byte[] Hash(string str, int truncationLength)
             {
                  var md5 = System.Security.Cryptography.MD5.Create();
                 byte[] input = System.Text.Encoding.ASCII.GetBytes(str);

                 var bitArray        = new System.Collections.BitArray(md5.ComputeHash(input));
                 var bitArrayMask    = new System.Collections.BitArray(truncationLength - 1);

                 bitArray.And(bitArrayMask);
                 return 
             }*/
        }

        public class Murmur
        {

        }
    }
}
