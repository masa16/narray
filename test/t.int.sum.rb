require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      a.debug_print
    end
    puts
    a
  end

  def test_int_sum
    #GC.disable
    px "$a = Num::Int16.new([3,4,5])"
    #px "$a[]= 1.001; $a"
    px "$a.seq"
    px "$b = $a['+,*,+'].sum"
    px "$c = $a['+,*,+'].accum"
    px "$d = $a.sum(0..-1)"
    px "$d = $a['+,*,+'].sum(0,1)"
    px "$b = $a[10..20].sum"
  end
end
