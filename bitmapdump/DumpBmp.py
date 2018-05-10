#!/usr/bin/env python

from __future__ import print_function
import sys
import os.path
from PIL import Image


def DumpBitmap(fileName) :
    img = Image.open(fileName).convert("1")
    (w,h) = img.size
    if w > 128 or h > 32 :
        print("Too Large %d,%d\n" % (w,h))
        return
    varname = os.path.splitext(os.path.basename(fileName))[0]

    print("static const unsigned char %s_rawdata[] = {" % (varname))
    for y in xrange(h):
        pixel=0
        print("\t" ,  end='' )
        for x in xrange(w):
            v = 1
            if img.getpixel((x,y)) != 0 :
                v = 0
            pixel = pixel << 1 | v
            if ((x+1) & 0x7) == 0:
                print(" (byte)0x%02X, " % (pixel & 0xFF) ,  end='' )
                pixel = 0
        if (w & 0x7) != 0:
            pixel = pixel << ( 7 - (w & 0x7))
            print(" (byte)0x%02X, " % (pixel & 0xFF) ,  end='' )
        print("")
    print("};")

    print("const struct __ImageStruct__ IMG_%s = { %d,%d,%d, %s_rawdata};" % (varname,w, h , (w + 7) >> 3, varname))

def DumpBitmap_8bpp(fileName) :
    img = Image.open(fileName).convert("1")
    (w,h) = img.size
    print("size: %d,%d\n" % (w,h))
    if w > 128 or h > 32 :

        return
    varname = os.path.splitext(os.path.basename(fileName))[0]

    print("static const unsigned char %s_rawdata[] = {" % (varname))
    for y in xrange(h):
        print("\t" ,  end='' )
        for x in xrange(w):
            if x % 16 == 0 :
                print("\n\t" ,  end='' )
            if img.getpixel((x,y)) != 0 :
                print(" 0xFF, ",  end='' )
            else :
                print(" 0x00, ",  end='' )
        print("")
    print("};")

    print("const struct __ImageStruct__ IMG_%s = { %d,%d,%d, %s_rawdata};" % (varname,w, h , (w + 7) >> 3, varname))

if __name__=="__main__":
    f = "logo.png"
    if len(sys.argv) > 1 :
        f = sys.argv[1]
    DumpBitmap(f)
    DumpBitmap_8bpp(f)
