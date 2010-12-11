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
    end
    puts
    a
  end

  def test_dup
    px "$a = Num::DFloat.new([5,5]); $a.seq; $a"
    px "$b = $a['1:3,::2']"
    px "$b.dup"
    px "$b.clone"
  end
end
