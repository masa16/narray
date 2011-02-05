require 'test/unit'
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray
      a.debug_info
      a.debug_print
      #p a.to_a
    end
    puts
    a
  end

  def test_transpose
    px "$b = Num::DFloat.new([3,4,5]).seq"
    px "$b.transpose"
    px "$b.transpose(2,1,0)"
    px "$b.transpose(-2..-1,0)"
    px "$b.transpose((-1..0)%-1)"
    px "$c = $b[[0,2],'*',[0,1,4]]"
    px "$c.transpose((-1..0)%-1)"
  end
end
