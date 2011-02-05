require 'test/unit'
#require "complex"
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

  def test_int
    #GC.disable
    #px "Num::Int16.step_unit"
    px "$b = Num::Int16.new(5).seq"
    px "$a = Num::Int16.cast([1,3,-7,13,-19])"
    px "$a.abs"
    px "-$a"
    px "~$a"
    px "$a & $b"
    px "$a ^ $b"

    px "$a.to_a"
    px "$a[2]"
    px "$a[2] = 123; $a"

    px "$c = Num::Int16.new(5).seq"
    px "$a * $c"


    px "Num::DFloat.cast($a)"
  end


  def test_int32_cast_bit
    px "$b = Num::Bit.cast([[1,1,1,0,1],[0,1,0,0,1]])"
    px "Num::Int8.cast($b)"
    px "Num::Int16.cast($b)"
    px "$c = Num::Int24.cast($b)"
    px "$c.data.to_str"
    px "Num::Int32.cast($b)"
  end

end
