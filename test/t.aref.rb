require 'test/unit'
require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    a.debug_info
    a.debug_print
    puts
    a
  end

  def py(s)
    puts s
    p a = eval(s)
    puts
    a
  end

  def test_dfloat_cast_from_object
    px "$a = Num::DFloat.new([5,5,5])"
    px "$a.seq(0,1)"
    px "$b = $a[1..2,2..3,3..4]"
    py "$b.store [0.0]"
    px "$a"
    py "$a[1..2,2..3,3..4] = 9999"
    px "$a"
    py "$a[1..2,2..3,3..4] = [[9901,9902],[9903,9904]]"
    px "$a"
    py "$a[1..2,2..3,3..4] = [[[9901],[9902]],[[9903],[9904]]]"
    px "$a"
    py "$a[1..2,2..3,3..4] += 10000"
    px "$a"
  end
end


=begin
px a="Num::DFloat.new([5,5,5])"
a.debug_info
a.seq(0,1)
a.debug_print

puts s="a[1..2,2..3,3..4]"
c = eval(s)
c.debug_info
c.fill(2.5)
c.debug_print
a.debug_print
puts


b = a[0,0,0]
b.debug_info
b.fill(2.5)
b.debug_print
puts
exit


b = a[-1,true,-1]
b.debug_info
b.fill(2.5)
b.debug_print
puts

b = a.slice -1,true,-1
b.debug_info
b.fill(2.5)
b.debug_print
puts
#exit


b = a[[1,2,3],[0,1,2],[2,4,3]]
b.debug_info
b.fill(2.5)
b.debug_print
puts

puts "c = a[(1..-1)%1,(-1..0)%-1,(0..-1)%2]"
c = a[(1..-1)%1,(-1..0)%-1,(0..-1)%2]
c.debug_info
c.fill(2.5)
c.debug_print
puts


puts "e = b[[1,2],[0,1],[2,0]]"

e = b[[1,2],[0,1],[2,0]]
e.debug_info
e.fill(2.5)
e.debug_print
puts


puts "c[(1..-1)%1,(1..-1)%1,(0..-1)%1]"

d = c[(1..-1)%1,(1..-1)%1,(0..-1)%1]
#d = b[[1,2],[0,1],[2,4])
d.debug_info
d.fill(2.5)
d.debug_print
puts


puts "b[(1..-1)%1,(1..-1)%1,(0..-1)%1]"

d = b[(1..-1)%1,(1..-1)%1,(0..-1)%1]
#d = b[[1,2],[0,1],[2,4])
d.debug_info
d.fill(2.5)
d.debug_print
puts


puts "f = c[[1,2],[0,1],[2,0]]"
f = c[[1,2],[0,1],[2,0]]
f.debug_info
f.fill(2.5)
f.debug_print
puts
=end
