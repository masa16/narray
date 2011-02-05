require 'test/unit'
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

  def test_dfloat_sum
    #GC.disable
    px "$a = NArray::DFloat.new([5]); $a.seq; $a"
    px "$b = NArray::DFloat.new([5]); $b.seq; $b=-$b+5; $b"
    px "$a.gt $b"
    return

    px "$a = NArray::DFloat.new([3,5]); $a.seq; $a"
    px "$b = NArray::DFloat.new([3]); $b.seq; $b+=5"
    px "$a.gt $b['*,-']"
    px "$c = NArray::DFloat.new([5]); $c.seq; $c+=4"
    px "$a.gt $c['-,*']"
    px "$b['*,-'].gt $c['-,*']"
    px "$b['*,-'].eq $c['-,*']"
  end
end
