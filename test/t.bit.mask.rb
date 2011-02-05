require 'test/unit'
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

  def test_bit
    #GC.disable
    px "$a = Num::DFloat.new([2,5]).seq"
    px "$b = Num::Bit.cast([[1,1,1,0,1],[0,1,0,0,1]])"
    px "$b.mask($a)"
  end


end
