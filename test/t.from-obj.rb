require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    a.debug_info
    a.debug_print
    puts
  end

  def test_dfloat_cast_from_object
    px "Num::DFloat.cast([[1,2,3],[4,5,6]])"
    px "Num::DFloat.cast([[1,2,3],[4,5]])"
    px "Num::DFloat.cast([[1,2,3],4,5])"
    px "Num::DFloat.cast([1,[1,[2,4]],3,[[3,4],5],2])"
    px "Num::DFloat.cast(1.23)"
    px "Num::DFloat.cast([])"
    px "Num::DFloat.cast([[]])"
    px "Num::DFloat.cast([[[[]]]])"
    px "Num::DFloat.cast([[],[],[[]]])"
    px "Num::DFloat.cast([1,2,[]])"
    px "Num::DFloat.cast([[1,[]],[]])"
    px "Num::DFloat.cast([1,1..2])"
    px "Num::DFloat.cast([1,(1..2)%0.2])"
  end
end

