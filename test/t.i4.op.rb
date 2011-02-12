require 'test/unit'
require "narray"

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
      #a.debug_print
      p a
    end
    puts
    a
  end

  def test_dfloat_sum
    #GC.disable
    px "$a = NArray::Int32.new([5])"
    px "$b = NArray::Int32.new([5])"
    px "$b.store( 1 ); $b"
    px "$a = NArray::Int32.new([5]).seq"
    px "$a + 1"
    px "$a * 2"
    px "$a - 4"
    px "$a / 3"
    px "$a % 3"
    #px "$a ** 2"
    px "$a"
    px "$a.inplace*2 + 5"
    px "$a"
    px "$a.fill(1234)"
    px "$a.fill(3)"
  end
end
