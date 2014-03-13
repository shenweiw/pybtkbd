
# copied from linux/net/bluetooth/hidp/core.c file.
hidkey_to_linuxkey = [
      0,   0,   0,   0,  30,  48,  46,  32,  18,  33,  34,  35,  23,  36,
     37,  38,  50,  49,  24,  25,  16,  19,  31,  20,  22,  47,  17,  45,
     21,  44,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  28,   1,
     14,  15,  57,  12,  13,  26,  27,  43,  43,  39,  40,  41,  51,  52,
     53,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  87,  88,
     99,  70, 119, 110, 102, 104, 111, 107, 109, 106, 105, 108, 103,  69,
     98,  55,  74,  78,  96,  79,  80,  81,  75,  76,  77,  71,  72,  73,
     82,  83,  86, 127, 116, 117, 183, 184, 185, 186, 187, 188, 189, 190,
    191, 192, 193, 194, 134, 138, 130, 132, 128, 129, 131, 137, 133, 135,
    136, 113, 115, 114,   0,   0,   0, 121,   0,  89,  93, 124,  92,  94,
     95,   0,   0,   0, 122, 123,  90,  91,  85,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     29,  42,  56, 125,  97,  54, 100, 126, 164, 166, 165, 163, 161, 115,
    114, 113, 150, 158, 159, 128, 136, 177, 178, 176, 142, 152, 173, 140
    ]

def show_keycode(keycode_array):
    size = len(keycode_array)
    for i in range(size):
        if (i % 14) == 0:
            print ' '
        print '{0:3d},'.format(keycode_array[i]), 
        pass
    return

def reverse_keycode_array(old_array):
    size = max(old_array) + 1
    print 'keycode array size = {0}'.format(size)
    new_array = [ 0 ] * size
    for i in range(len(old_array)):
        new_array[old_array[i]] = i
        pass
    new_array[0] = 0
    show_keycode(new_array)
    return

reverse_keycode_array(hidkey_to_linuxkey)

