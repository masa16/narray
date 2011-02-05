require 'test/unit'
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

  def test_dfloat_sum
    #GC.disable
    $n = 1000
    px "$bd = Num::ByteData.new($n)"
    px "$a = Num::DFloat.new([2,5],$bd); $a.seq;"
    px "$bd"

    File.open("test.dat","w") {|f| f.write("\0"*$n)}
    px "$mm = Num::MmapByteData.new('test.dat')"
    px "$b = Num::DFloat.new([5,5],$mm,:offset=>128); $b.seq;"
    px "$mm"
    px "$b.inplace / 123"

    px "$b = Num::DFloat.new([2,100],$mm); $b.seq;"
  end
end
