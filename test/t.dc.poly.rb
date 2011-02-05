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

  def test_dcomplex_poly
    #GC.disable
    px "$a = Num::DComplex.new([3,3]).seq"
    px "$x = Num::DComplex.new([3]).seq+1"
    px "$c = Num::DComplex.poly($x,$a)"
    px "($a[1,:*] * 2**Num::DComplex.cast([0..2])).sum"
    px "($a[2,:*] * 3**Num::DComplex.cast([0..2])).sum"

    px "$x = Num::DComplex.cast([2,3])"
    px "$a = Num::DComplex.cast([1,1,1])"
    px "$c = Num::DComplex.poly($x,$a)"
    px "$c = Num::DComplex.poly([2,3],[1,1,1])"
  end
end
