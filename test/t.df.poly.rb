require 'test/unit'
require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
      p a
    end
    puts
    a
  end

  def test_dfloat_poly
    px "$x = NArray::DFloat[2,3,4]"
    px "$c = $x.poly(3,2,1)"

    #GC.disable
    #px "$a = NArray::DFloat.new([3,3]).seq"
    px "$x = NArray::DFloat.new([3]).seq+1"
    #px "$c = NArray::DFloat.poly($x,$a)"
    px "$c = $x.poly([1,2,3],[4,5,6],[7,8,9])"
    #px "($a[1,:*] * 2**NArray::DFloat.cast([0..2])).sum"
    #px "($a[2,:*] * 3**NArray::DFloat.cast([0..2])).sum"

  end
end
