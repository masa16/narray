#require 'test/unit'
require "narray"
#require 'irb/xmp'


a = Num::DFloat.new([5,5,5])
a.debug_info
a.seq(0,1)
a.debug_print

puts s="a[1..2,2..3,[4,2,0]]"
c = eval(s)
c.debug_info
c.debug_print

d = c.copy

d.debug_info
d.debug_print

d.store(0)
d.debug_print

c.store(d)

c.debug_print
a.debug_print
