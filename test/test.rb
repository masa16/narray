require 'test/unit'
require "narray"
require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def test_nelement
    #  a = Num::NArray.new(Num::NElement,[3,4])
  end

  def test_index_order
    a = Num::DFloat.new(5)
    puts s="a.column_major?"
    p eval( s )
    puts s="a.row_major?"
    p eval( s )
  end

  def test_narray01
    a = Num::DFloat.new([3,2,5])
    a.debug_info
  end

  def test_narray011
    a = Num::DFloat.new([3,2,5])
    a.seq(1,1.5)
    a.debug_info
    a.debug_print

    b = a[true,0,(1..-1)%2]
    b.debug_info
    b.debug_print
  end

  def test_narray02
    a = Num::DFloat.new([3,0])
    a.debug_info
  end

  def test_narray03
    a = Num::DFloat.new([3,-2])
    a.debug_info
  end

  def test_narray04
    a = Num::DFloat.new(5)
    a.debug_info
  end

  def test_narray05
    a = Num::DFloat.new(-5)
    a.debug_info
  end

  def test_dfloat_store
    a = Num::DFloat.new([3,2,5])
    a.debug_info
    a.store(3.14)
    a.debug_print
    puts

    b = Num::DFloat.new([3,2,5])
    b.debug_info
    b.store(2.5)
    b.debug_print
    puts

    c = Num::DFloat.add(a,b)
    p c
    puts "pass 6"
    c.debug_info
    puts "pass 7"
    c.debug_print
    puts "pass 8"
    puts
  end


  def test_to_array
    GC.disable
    a = Num::DFloat.new([2,3,4])
    a.store(1.5)
    a.debug_info
    b = a.to_a
    puts "b = #{b.inspect} "
  end

  def test_flatten
    b = Num::DFloat.new([3,2,5])
    b.debug_info
    b.store(2.5)
    b.debug_print
    b = b.flatten
    puts "flatten"
    b.debug_info
    b.store(2.5)
    b.debug_print
    puts
  end
end
