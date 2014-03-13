#!/usr/bin/env python

import ctypes,os

hidd_report_desc = bytearray(
    [
	0x05, 0x01,	# UsagePage GenericDesktop
	0x09, 0x02,	# Usage Mouse
	0xA1, 0x01,	# Collection Application
	0x85, 0x01,	# REPORT ID: 1
	0x09, 0x01,	# Usage Pointer
	0xA1, 0x00,	# Collection Physical
	0x05, 0x09,	# UsagePage Buttons
	0x19, 0x01,	# UsageMinimum 1
	0x29, 0x03,	# UsageMaximum 3
	0x15, 0x00,	# LogicalMinimum 0
	0x25, 0x01,	# LogicalMaximum 1
	0x75, 0x01,	# ReportSize 1
	0x95, 0x03,	# ReportCount 3
	0x81, 0x02,	# **Input data variable absolute
	0x75, 0x05,	# ReportSize 5
	0x95, 0x01,	# ReportCount 1
	0x81, 0x01,	# **InputConstant (padding)
	0x05, 0x01,	# UsagePage GenericDesktop
	0x09, 0x30,	# Usage X
	0x09, 0x31,	# Usage Y
	0x09, 0x38,	# Usage ScrollWheel
	0x15, 0x81,	# LogicalMinimum -127
	0x25, 0x7F,	# LogicalMaximum +127
	0x75, 0x08,	# ReportSize 8
	0x95, 0x02,	# ReportCount 3
	0x81, 0x06,	# **Input data variable relative
	0xC0, 0xC0,	# EndCollection EndCollection
	0x05, 0x01,	# UsagePage GenericDesktop
	0x09, 0x06,	# Usage Keyboard
	0xA1, 0x01,	# Collection Application
	0x85, 0x02,	# REPORT ID: 2
	0xA1, 0x00,	# Collection Physical
	0x05, 0x07,	# UsagePage Keyboard
	0x19, 0xE0,	# UsageMinimum 224
	0x29, 0xE7,	# UsageMaximum 231
	0x15, 0x00,	# LogicalMinimum 0
	0x25, 0x01,	# LogicalMaximum 1
	0x75, 0x01,	# ReportSize 1
	0x95, 0x08,	# ReportCount 8
	0x81, 0x02,	# **Input data variable absolute
	0x95, 0x08,	# ReportCount 8
	0x75, 0x08,	# ReportSize 8
	0x15, 0x00,	# LogicalMinimum 0
	0x25, 0x65,	# LogicalMaximum 101
	0x05, 0x07,	# UsagePage Keycodes
	0x19, 0x00,	# UsageMinimum 0
	0x29, 0x65,	# UsageMaximum 101
	0x81, 0x00,	# **Input DataArray
	0xC0, 0xC0,	# EndCollection
    ])

class HiddInfo(ctypes.Structure):
    _fields_ = (('service_name', ctypes.c_char_p),
                ('description', ctypes.c_char_p),
                ('provider', ctypes.c_char_p),
                ('desc_data', ctypes.c_char_p),
                ('desc_len', ctypes.c_uint),
                ('sub_class', ctypes.c_uint))

HiddInfoPtr = ctypes.POINTER(HiddInfo)

class Sdp:
    def __init__(self):
        self.dll = ctypes.CDLL(os.path.abspath('libbthidd.so'))
        return
    def CreateRecord(self, handle=0xffffffff):
        return self.dll.SdpCreateRecord(ctypes.c_uint(handle))
    def DestroyRecord(self, record):
        return self.dll.SdpDestroyRecord(ctypes.c_void_p(record))
    def MakeUpHidRecord(self, record, info):
        return self.dll.SdpMakeUpHidRecord(ctypes.c_void_p(record), HiddInfoPtr(info))
    def RegisterRecord(self, record):
        return self.dll.SdpRegisterRecord(ctypes.c_void_p(record))
    def UnregisterRecord(self, record):
        return self.dll.SdpUnregisterRecord(ctypes.c_void_p(record))
    pass

def main_entry():
    info = HiddInfo()
    info.service_name = 'My Virtual Mouse and Keyboard'
    info.description = 'Bluetooth HID Mouse and Keyboard'
    info.provider = 'Huipeng Zhao'
    info.desc_data = str(hidd_report_desc)
    info.desc_len = len(hidd_report_desc)
    info.sub_class = 0x80
    
    sdp = Sdp()
    record = sdp.CreateRecord()
    sdp.MakeUpHidRecord(record, info)
    sdp.RegisterRecord(record)
    sdp.DestroyRecord(record)
    return

if __name__ == '__main__':
    main_entry()
    pass

