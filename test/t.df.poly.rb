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

  def test_dfloat_poly
    #GC.disable
    px "$a = Num::DFloat.new([3,3]).seq"
    px "$x = Num::DFloat.new([3]).seq+1"
    px "$c = Num::DFloat.poly($x,$a)"
    px "($a[1,:*] * 2**Num::DFloat.cast([0..2])).sum"
    px "($a[2,:*] * 3**Num::DFloat.cast([0..2])).sum"

    px "$x = Num::DFloat.cast([2,3])"
    px "$a = Num::DFloat.cast([1,1,1])"
    px "$c = Num::DFloat.poly($x,$a)"
    px "$c = Num::DFloat.poly(2,[1,1,1])"
  end
end
