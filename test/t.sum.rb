require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray # a.class < Num::NArray
      a.debug_info
      p a
    end
    puts
    a
  end

  def test_dfloat_sum
    #GC.disable
    px "$a = NArray::DFloat.new([3,4,5])"
    #px "$a[]= 1.001; $a"
    px "$a.seq"
    px "$a.sum"
    px "$b = $a['+,*,+'].sum"
    px "$c = $a['+,*,+'].accum"
    px "$d = $a.sum(0..-1)"
    px "$d = $a['+,*,+'].sum(0,1)"
    px "$b = $a[10..20].sum"
  end
end
