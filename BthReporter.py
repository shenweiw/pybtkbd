
#!/usr/bin/env python

import ctypes,os
import Tkinter,sys,time
import logging
import binascii
import sdp_helper
#add sdp_helper to send out the HID descriptors.
#Continue to practise with Git command.
#Verify the usage of Git Tag

# reverse from hidkey_to_linuxkey.
linuxkey_to_hidkey = [
      0,  41,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  45,  46,  
     42,  43,  20,  26,   8,  21,  23,  28,  24,  12,  18,  19,  47,  48,  
     40, 224,   4,  22,   7,   9,  10,  11,  13,  14,  15,  51,  52,  53,  
    225,  50,  29,  27,   6,  25,   5,  17,  16,  54,  55,  56, 229,  85,  
    226,  44,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  83,  
     71,  95,  96,  97,  86,  92,  93,  94,  87,  89,  90,  91,  98,  99,  
      0, 148, 100,  68,  69, 135, 146, 147, 138, 136, 139, 140,  88, 228,  
     84,  70, 230,   0,  74,  82,  75,  80,  79,  77,  81,  78,  73,  76,  
      0, 239, 238, 237, 102, 103,   0,  72,   0, 133, 144, 145, 137, 227,  
    231, 101, 243, 121, 118, 122, 119, 124, 116, 125, 244, 123, 117,   0,  
    251,   0, 248,   0,   0,   0,   0,   0,   0,   0, 240,   0, 249,   0,  
      0,   0,   0,   0, 241, 242,   0, 236,   0, 235, 232, 234, 233,   0,  
      0,   0,   0,   0,   0, 250,   0,   0, 247, 245, 246,   0,   0,   0,  
      0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
    ]


class Point:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y
        return
    def __str__(self):
        return 'Point({0},{1})'.format(self.x,self.y)
    pass

class Key:
    def __init__(self, char='', keycode=0, keysym='', keysym_num=0, meta=0):
        self.char = char
        self.keycode = keycode
        self.keysym = keysym
        self.keysym_num = keysym_num
        self.meta = meta
        self.ch_val = 0
        if len(self.char): self.ch_val = ord(self.char[0])
        return
    def __str__(self):
        return 'Key({0}({1}), VK={2}, SYM={3}, SYMNUM={4}, META={5})'.format(
            self.char,self.ch_val,self.keycode,
            self.keysym,self.keysym_num,self.meta)
    pass

class Reporter:
    INPUTEVT_MOUSEMOVE       = 0
    INPUTEVT_MOUSELEFTDOWN   = 1
    INPUTEVT_MOUSELEFTUP     = 2
    INPUTEVT_MOUSERIGHTDOWN  = 3
    INPUTEVT_MOUSERIGHTUP    = 4
    INPUTEVT_MOUSEMIDDLEDOWN = 5
    INPUTEVT_MOUSEMIDDLEUP   = 6
    INPUTEVT_MOUSEWHEEL      = 7
    INPUTEVT_KEYDOWN         = 8
    INPUTEVT_KEYUP           = 9
    INPUTEVT_MAX             = 10
    def isValid(self):
        return True
    def reportInputEvent(self, itype, e):
        lgr.info('itype:{0}, event:{1}'.format(itype, e))
        return
    pass

class InputReceptor(Tkinter.Tk):
    m_lastPoint = None # instance of class Point.
    m_reporter = None  # it reports the input event to elsewhere.
    def __init__(self, reporter=Reporter()):
        Tkinter.Tk.__init__(self)
        self.title("Bluetooth HID Client Test")
        self.geometry('640x480+100+100')
        self.minsize(640, 480)
        self.bind('<Motion>', self.onMouseMoveREL)
        self.bind('<Button-1>', self.onMouseLeftDown)
        self.bind('<ButtonRelease-1>', self.onMouseLeftUp)
        self.bind('<Button-3>', self.onMouseRightDown)
        self.bind('<ButtonRelease-3>', self.onMouseRightUp)
        self.bind('<KeyPress>', self.onKeyDown)
        self.bind('<KeyRelease>', self.onKeyUp)
        self.bind('<Enter>', self.onEnter) # pointer enters the widget.
        self.bind('<Leave>', self.onLeave) # pointer leaves the widget.
        self.m_reporter = reporter	
        return
    def onMouseMoveREL(self, e):
        if not self.m_lastPoint:
            self.m_lastPoint = Point(e.x, e.y)
            return
        delta = Point(e.x - self.m_lastPoint.x, e.y - self.m_lastPoint.y)
        self.m_lastPoint = Point(e.x, e.y)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_MOUSEMOVE, delta)
        return
    def onMouseLeftDown(self, e):
        self.onMouseMoveREL(e)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_MOUSELEFTDOWN, Point())
        return
    def onMouseLeftUp(self, e):
        self.onMouseMoveREL(e)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_MOUSELEFTUP, Point())
        return
    def onMouseRightDown(self, e):
        self.onMouseMoveREL(e)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_MOUSERIGHTDOWN, Point())
        return
    def onMouseRightUp(self, e):
        self.onMouseMoveREL(e)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_MOUSERIGHTUP, Point())
        return
    def onKeyDown(self, e):
        key = Key(e.char, e.keycode, e.keysym, e.keysym_num, e.state)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_KEYDOWN, key)
        return
    def onKeyUp(self, e):
        key = Key(e.char, e.keycode, e.keysym, e.keysym_num, e.state)
        self.m_reporter.reportInputEvent(Reporter.INPUTEVT_KEYUP, key)
        return
    def onEnter(self, e):
        return
    def onLeave(self, e):
        self.m_lastPoint = None
        return
    pass


class MouseEvent:
    BTN_LEFT   = 0x0
    BTN_RIGHT  = 0x1
    BTN_MIDDLE = 0x2
    def __init__(self, buttons, x, y, wheel):
        self.buttons = buttons
        self.x       = x
        self.y       = y
        self.wheel   = wheel
        return
    pass

class KeybdEvent:
    KEY_RIGHTMETA = 126
    KEY_RIGHTALT = 100
    KEY_RIGHTSHIFT = 54
    KEY_RIGHTCTRL = 97
    KEY_LEFTMETA = 125
    KEY_LEFTALT = 56
    KEY_LEFTSHIFT = 42
    KEY_LEFTCTRL = 29
    def __init__(self, modi, keys):
        self.modifier = modi
        self.keys = keys
        return
    pass

class BthReporter(Reporter):
    m_handle = 0
    m_btnState = 0
    m_keyModif = 0
    m_keyPress = []
    def __init__(self):
        self.m_bth = ctypes.CDLL(os.path.abspath('libbthidd.so'))
        self.m_bth.bthidd_intr_send.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
        self.m_handle = self.m_bth.bthidd_init()
        if self.m_handle:
            self.m_bth.bthidd_accept(self.m_handle)
        lgr.info('BthReporter is initiated.')
        return
    def __del__(self):
        if self.m_handle:
            self.m_bth.bthidd_exit(self.m_handle)
        return
    def isValid(self):
        return (self.m_handle != 0)
    def m_reportMouseEvent(self, me):
        pkt = bytearray()
        pkt.append(0xA1)        # always 0xA1
        pkt.append(2)           # mouse report id
        pkt.append(me.buttons)  # buttons
        pkt.append(me.x & 0xff) # x
        pkt.append(me.y & 0xff) # y
        pkt.append(me.wheel)    # wheel
        self.m_bth.bthidd_intr_send(self.m_handle, str(pkt), len(pkt))
        return
    def m_reportKeybdEvent(self, ke):
        pkt = bytearray()
        pkt.append(0xA1)        # always 0xA1
        pkt.append(1)           # keyboard report id
        pkt.append(ke.modifier) # modifier keys (shift, ctrl, ...)
        pkt.append(0)
        for i in range(len(ke.keys)):   pkt.append(ke.keys[i])
        for i in range(6-len(ke.keys)): pkt.append(0)
        lgr.info ('m_reportKeybdEvent(): modi={0}, keys={1}'.format(ke.modifier, ke.keys))
        lgr.info ('stream: {0} <{1}>'.format(binascii.hexlify(str(pkt)), len(pkt)))
        # 0   1  2  3  4  5  6  7  8  9 10
        # a1 01 00 1e 00 00 00 00 00 00 00
        # a1 02 00 1f 00 00 00 00 00 00 00
        # a1 01 00 00 1e 00 00 00 00 00   correct one
        self.m_bth.bthidd_intr_send(self.m_handle, str(pkt), len(pkt))
        return
    def m_isMetaKey(self, keycode):
        if keycode == KeybdEvent.KEY_RIGHTMETA:  return True
        if keycode == KeybdEvent.KEY_RIGHTALT:   return True
        if keycode == KeybdEvent.KEY_RIGHTSHIFT: return True
        if keycode == KeybdEvent.KEY_RIGHTCTRL:  return True
        if keycode == KeybdEvent.KEY_LEFTMETA:   return True
        if keycode == KeybdEvent.KEY_LEFTALT:    return True
        if keycode == KeybdEvent.KEY_LEFTSHIFT:  return True
        if keycode == KeybdEvent.KEY_LEFTCTRL:   return True
        return False
    def m_getModifier(self, keycode):
        'return: 0 means keycode is not a modifier key.'
        modifier = 0
        if keycode == KeybdEvent.KEY_RIGHTMETA:  modifier = 1<<7
        if keycode == KeybdEvent.KEY_RIGHTALT:   modifier = 1<<6
        if keycode == KeybdEvent.KEY_RIGHTSHIFT: modifier = 1<<5
        if keycode == KeybdEvent.KEY_RIGHTCTRL:  modifier = 1<<4
        if keycode == KeybdEvent.KEY_LEFTMETA:   modifier = 1<<3
        if keycode == KeybdEvent.KEY_LEFTALT:    modifier = 1<<2
        if keycode == KeybdEvent.KEY_LEFTSHIFT:  modifier = 1<<1
        if keycode == KeybdEvent.KEY_LEFTCTRL:   modifier = 1<<0
        return modifier
    def m_changeKeyModif(self, isDown, modifier):
        if isDown: self.m_keyModif |= modifier
        else     : self.m_keyModif &= (~modifier & 0xff)
        return
    def m_changeKeyPress(self, isDown, keycode):
        for i in range(len(self.m_keyPress)):
            if self.m_keyPress[i] == linuxkey_to_hidkey[keycode]:
                if not isDown:
                    del self.m_keyPress[i]
                break
        else: # append the newly pressed key.
            if isDown:
                self.m_keyPress.append(linuxkey_to_hidkey[keycode])
            pass
        return
    # overwite
    def reportInputEvent(self, itype, e):
        if not self.m_handle: return
        if itype == Reporter.INPUTEVT_MOUSEMOVE:
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseMove: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSELEFTDOWN:
            self.m_btnState |= (1<<MouseEvent.BTN_LEFT)
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseLD: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSELEFTUP:
            self.m_btnState &= (0x7 & ~(1<<MouseEvent.BTN_LEFT))
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseLU: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSERIGHTDOWN:
            self.m_btnState |= (1<<MouseEvent.BTN_RIGHT)
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseRD: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSERIGHTUP:
            self.m_btnState &= (0x7 & ~(1<<MouseEvent.BTN_RIGHT))
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseRU: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSEMIDDLEDOWN:
            self.m_btnState |= (1<<MouseEvent.BTN_MIDDLE)
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseMD: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSEMIDDLEUP:
            self.m_btnState &= (0x7 & ~(1<<MouseEvent.BTN_MIDDLE))
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,e.x,e.y,0))
            lgr.info('MouseMU: {0} btns={1}'.format(e, self.m_btnState))
            pass
        elif itype == Reporter.INPUTEVT_MOUSEWHEEL:
            self.m_reportMouseEvent(MouseEvent(self.m_btnState,0,0,e))
            lgr.info('MouseMW: wheel={0}'.format(e))
            pass
        elif itype == Reporter.INPUTEVT_KEYDOWN:
            lgr.info('KeyDN: {0}'.format(e))
            keycode = e.keycode - 8  # it just works ... but why?
            if keycode >= len(linuxkey_to_hidkey):
                lgr.error('keycode = {0}, out of hidkey range!'.format(e.keycode))
                return
            modifier = self.m_getModifier(keycode)
            if modifier:
                self.m_changeKeyModif(True, modifier)
            else:
                self.m_changeKeyPress(True, keycode)
            self.m_reportKeybdEvent(KeybdEvent(self.m_keyModif, self.m_keyPress))
            pass
        elif itype == Reporter.INPUTEVT_KEYUP:
            lgr.info('KeyUP: {0}'.format(e))
            keycode = e.keycode - 8  # it just works ... but why?
            if keycode >= len(linuxkey_to_hidkey):
                lgr.error('keycode = {0}, out of hidkey range!'.format(e.keycode))
                return
            modifier = self.m_getModifier(keycode)
            if modifier:
                self.m_changeKeyModif(False, modifier)
            else:
                self.m_changeKeyPress(False, keycode)
            self.m_reportKeybdEvent(KeybdEvent(self.m_keyModif, self.m_keyPress))
            pass
        else:
            Reporter.reportInputEvent(self, itype, e)
            pass
        return
    pass
    

def hidd_main_entry():
    reporter = BthReporter()
    
    if not reporter.isValid():
        lgr.error('BthReporter is invalid! exit.')
        return
    gui = InputReceptor(reporter)
    gui.mainloop()
    return

if __name__ == "__main__":
   
	# create logger
	lgr = logging.getLogger('myapp')
	lgr.setLevel(logging.INFO)
	# add a file handler
	#fh = logging.FileHandler('myapp.log')
	#fh.setLevel(logging.WARNING)
	# create a formatter and set the formatter for the handler.
	#frmt = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
	#fh.setFormatter(frmt)
	# add the Handler to the logger
	#lgr.addHandler(fh)
	#lgr.info('Shenwei Python Logging system')
	
	# create console handler and set level to debug
	ch = logging.StreamHandler()
	ch.setLevel(logging.DEBUG)
	# create formatter
	formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
	# add formatter to ch
	ch.setFormatter(formatter)
	# add ch to logger
	lgr.addHandler(ch)
	
	# "application" code
	lgr.debug("debug message")
	lgr.info("info message")
	lgr.warn("warn message")
	lgr.error("error message")
	lgr.critical("critical message")
	
	hidd_main_entry()   
