require 'test/unit'
#require "complex"
require "./narray.so"
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

  def test_dfloat_min
    #GC.disable
    px "$a = Num::Int32.new([3,4,5]).seq"
    px "$b = $a.min"
    px "$b = $a['+,*,+'].min"
    px "$d = $a.min(0..-1)"
    px "$d = $a['+,*,+'].min(0,1)"
    px "$b = $a[10..20].min"
  end

  def test_dfloat_max
    #GC.disable
    px "$a = Num::Int32.new([3,4,5]).seq"
    px "$b = $a.max"
    px "$b = $a['+,*,+'].max"
    px "$d = $a.max(0..-1)"
    px "$d = $a['+,*,+'].max(0,1)"
    px "$b = $a[10..20].max"
  end
end
