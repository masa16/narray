#require 'test/unit'
require "./narray.so"
#require 'irb/xmp'


a = Num::DFloat.new([11])
a.seq(1,1.5)
a.debug_info
a.debug_print

a = Num::DFloat.new([5,5,5])
a.seq(1,2)
a.debug_info
a.debug_print
exit

