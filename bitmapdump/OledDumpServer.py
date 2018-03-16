#!/usr/bin/env python

import sys
import os.path
import numpy as np
from PIL import Image
from PIL import ImageDraw

from graphics import *

import urllib
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import SocketServer



#2|root@Pixi4-4:/data/data # ./curl -T t.txt http://localhost:9234
#adb reverse tcp:9234 tcp:9234

(WIDTH, HEIGHT , PITCH) = (128, 32, 16)

global_draw = None
global_image = None

global_win = None

def createImage():
    array = np.ndarray((HEIGHT, WIDTH, 3), np.uint8)
    # background black
    array[:, :, 0] = 0
    array[:, :, 1] = 0
    array[:, :, 2] = 0
    return Image.fromarray(array)


def fillImage(draw, data):
    global PITCH
    if draw is None:
        return
    raw_bytes = bytearray(data)
    if len(data) == (PITCH * HEIGHT) :
        for h in xrange(HEIGHT):
            for w in xrange(WIDTH):
                pixel = raw_bytes[h * PITCH + (w >> 3)]
                fill = (0, 0, 0)
                if (pixel >> (7-(w & 7))) & 0x01:
                    fill = (255, 255, 255)
                draw.point((w, h), fill)
        return
    elif len(data) == (PITCH * HEIGHT * 8) :
        local_PITCH = PITCH * 8
        for h in xrange(HEIGHT):
            for w in xrange(WIDTH):
                pixel = raw_bytes[h * local_PITCH + (w)]
                fill = (0, 0, 0)
                if (pixel & 0x80) != 0:
                    fill = (255, 255, 255)
                draw.point((w, h), fill)
    else :
        print "Error length :", len(data)
    return

def updateWin(win, data):
    if win is None:
        return
    raw_bytes = bytearray(data)
    if len(data) == (PITCH * HEIGHT) :
        for h in xrange(HEIGHT):
            for w in xrange(WIDTH):
                pixel = raw_bytes[h * PITCH + (w >> 3)]
                color = "black"
                if (pixel >> (7-(w & 7))) & 0x01:
                    color = "white"
                win.plotPixel(w, h, color)
    elif len(data) == (PITCH * HEIGHT * 8):
        local_PITCH = PITCH * 8
        for h in xrange(HEIGHT):
            for w in xrange(WIDTH):
                pixel = raw_bytes[h * local_PITCH + (w)]
                color = "black"
                if (pixel & 0x80) != 0:
                    color = "white"
                win.plotPixel(w, h, color)
    else:
        print "Error length :", len(data)
        return
    return


def updateImage(data):
    global global_win, global_draw
    #fillImage(global_draw, data)
    #global_image.show()
    # graphics
    updateWin(global_win,data)
    #print data

class ReceiveFrameBufferHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        #mpath,margs=urllib.splitquery(self.path)
        print "Ignore Get"

    def do_PUT(self):
        #mpath,margs=urllib.splitquery(self.path)
        datas = self.rfile.read(int(self.headers['content-length']))
        print "FrameBuffer Update : " , len(datas)
        updateImage(datas)


def setHttpServer(port):

    Handler = ReceiveFrameBufferHandler
    httpd = SocketServer.TCPServer(("", PORT), Handler)
    print "serving at port", PORT
    print " need adb reverse"
    httpd.serve_forever()


if __name__=="__main__":
    PORT = 9234
    if len(sys.argv) > 1 :
        PORT = sys.argv[1]
    #global_image = createImage()
    #global_draw = ImageDraw.Draw(global_image)
    global_win = GraphWin('OLED', WIDTH, HEIGHT);

    setHttpServer(PORT)
