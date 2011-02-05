require 'test/unit'
#require "complex"
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

  def test_int
    #GC.disable
    #px "NArray::Int16.step_unit"
    px "$b = NArray::Int16.new([5]).fill(123)"
    px "$a = NArray::Int16.cast([1,3,-7,13,-19])"
    px "$a.abs"
    px "-$a"
    #px "~$a"
    #px "$a & $b"
    #px "$a ^ $b"

    px "$a.to_a"
    px "$a[2]"
    px "$a[2] = 123; $a"

    px "$c = NArray::Int16.new(5).seq"
    px "$a * $c"


    px "NArray::DFloat.cast($a)"
  end


  def test_int32_cast_bit
    px "$b = NArray::Bit.cast([[1,1,1,0,1],[0,1,0,0,1]])"
    px "NArray::Int8.cast($b)"
    px "$c = NArray::Int16.cast($b)"
    #px "$c = NArray::Int24.cast($b)"
    #px "$c.data.to_str"
    px "NArray::Int32.cast($b)"
  end

end
