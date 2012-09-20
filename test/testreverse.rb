$LOAD_PATH.push "lib"
require './narray.so'

def px(a)
  puts a
  puts "#=> " + eval(a).inspect
end

px "$a = NArray.int(3,3).indgen!   "
px "$a.reverse                     "
px "$a.reverse(0)                  "
px "$a.reverse(1)                  "
px "$a.rot90                       "
px "$a.rot90(2)                    "
px "$a.rot90(-1)                   "
px "$a = NArray.int(3,3,3).indgen! "
px "$a.reverse                     "
px "$a.reverse(0)                  "
px "$a.reverse(1)                  "
px "$a.rot90                       "
px "$a.rot90(2)                    "
px "$a.rot90(-1)                   "
px "$a = NArray.int(3).indgen!     "
px "$a.reverse                     "
px "$a.reverse(0)                  "
px "$a.rot90                       "
px "$a.rot90(-1)                   "
