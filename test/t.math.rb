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

  def test_nmath
    #GC.disable
    px "$a = Num::DFloat.new([5]).seq"
    px "Num::NMath.sin($a)"
    px "$b = Num::Int32.new([5]).seq"
    px "Num::NMath.sin($b)"
    px "Num::NMath.atan2($a,$b)"
    px "$c = Num::DComplex.new([5]).seq"
    px "Num::NMath.sin($c)"
    px "Num::NMath.sin([1,2,3])"
    px "Num::NMath.sin([1.im,2,3+1.im])"
    px "Num::NMath.sin(1)"
    px "Num::NMath.sin(1.im)"
    px "Num::NMath.sin('hage')"
  end
end
