#!/usr/bin/env python

from __future__ import print_function
import sys
import os.path
from PIL import Image

dst_mask_ = [0x01, 0x02 , 0x04 ,0x08,0x10 ,0x20,0x40,0x80]

def Image2FrameBuf(fileName) :
    img = Image.open(fileName).convert("1")
    (w,h) = img.size
    if w > 128 or h > 32 :
        print("Too Large %d,%d\n" % (w,h))
        return
    varname = os.path.splitext(os.path.basename(fileName))[0]
    DRAM_V_FB = [0] * 512
    for y in xrange(h):
        for x in xrange(w):
            if img.getpixel((x,y )) == 0 :
                DRAM_V_FB[(x) + ((y) >> 3 ) * 128] |= dst_mask_[(y) & 0x7]

    print("static const unsigned char %s_Img[] = {" % (varname) )
    for x in xrange(512):
        if x % 16 == 0 :
            print("")
            print("\t" ,  end='' )
        print("0x%02X, " % (DRAM_V_FB[x] & 0xFF) ,  end='' )
    print("")
    print("};")


if __name__=="__main__":
    f = "logo.png"
    if len(sys.argv) > 1 :
        f = sys.argv[1]
    Image2FrameBuf(f)

