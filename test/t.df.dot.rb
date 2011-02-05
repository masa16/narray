require 'test/unit'
require "complex"
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

  def test_dfloat_op
    #GC.disable
    px "$a = Num::DFloat.new([3,4]).seq"
    px "$b = Num::DFloat.new([2,3]).seq"
    px "$c = Num::Linalg.dot_mm($a,$b)"

    px "$a = Num::DFloat.new([2,3,4]).seq"
    px "$b = Num::DFloat.new([2,2,3]).seq"
    px "$c = Num::Linalg.dot_mm($a,$b)"

    px "$a = Num::DFloat.cast([[1,0,0],[0,2,1],[-3,0,2]])"
    px "$b = 1-Num::DFloat.new([3,3]).seq"
    px "Num::Linalg.solve($a,$b)"
    px "Num::Linalg.set_unit($b)"
    px "$x = Num::Linalg.solve($a,$b)"
    px "Num::Linalg.dot_mm($x,$a)"
    px "Num::Linalg.dot_mm($a,$x)"
    px "$a = Num::DFloat.cast([[[2,0,-1],[0,2,0],[-3,0,2]],[[1,0,0],[0,2,1],[-3,0,2]]])"
    px "Num::Linalg.inv($a)"
  end
end


#[2,0,-1;0,2,0;-3,0,2]
#a=[2,0,-1;0,2,0;-3,0,2]

#inv([1,0,0;0,2,1;-3,0,2])
