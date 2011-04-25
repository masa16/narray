require 'test/unit'
require 'narray'

$delta = 1e-6 # numerical tolerance

def assert_narray_close exp, obs
  assert exp.shape == obs.shape && ((exp - obs).abs < $delta).all?,
    "#{exp.inspect} expected; got\n#{obs.inspect}"
end

load 'test/testcumsum.rb'
