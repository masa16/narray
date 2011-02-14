require "erb"

$dir="erb"

def define_type(name,ctype)
  $class_name = name
  $ctype = ctype
  $type_var = "c"+name
  $type_name = $tp = name.downcase
  $math_var = "m"+name+"Math"
end

def define_real(name,ctype)
  $real_class_name = name
  $real_ctype = ctype
  $real_type_var = "c"+name
  $real_type_name = name.downcase
end

$class_alias = []
def define_alias(*names)
  $class_alias ||= []
  names.each do |x|
    $class_alias << x
  end
end

def tp; $tp; end

def type_name; $type_name; end


class PutERB
  def erb
    file = "#{$dir}/#{@tmpl}.c"
    if $embed
      ERB.new(File.read(file)).result(binding)
    else
      puts "\n//ERB from #{file}"
      ERB.new(File.read(file)).run(binding)
    end
  end
end


class Cast < PutERB
  INIT = []

  def self.c_instance_method
    "nary_#{$tp}_s_cast"
  end

  def initialize(tmpl,tpname,dtype,tpclass,macro)
    @tmpl=tmpl
    @tpname=tpname
    @dtype=dtype
    @tpclass=tpclass
    @macro=macro
  end
  attr_reader :tmpl, :tpname, :dtype, :tpclass, :macro

  def c_function
    "nary_#{tp}_cast_#{tpname}"
  end

  def c_iterator
    "iter_#{tp}_cast_#{tpname}"
  end

  def result
    INIT << self
    erb
  end

  def condition
    "rb_obj_is_kind_of(obj,#{tpclass})"
  end
end


class CastNum < Cast
  def initialize(tmpl)
    @tmpl=tmpl
    @tpname="numeric"
  end
  def condition
    "FIXNUM_P(obj) || TYPE(obj)==T_FLOAT || TYPE(obj)==T_BIGNUM || rb_obj_is_kind_of(obj,cComplex)"
  end
end

class CastArray < Cast
  def initialize(tmpl)
    @tmpl=tmpl
    @tpname="array"
  end
  def condition
    "TYPE(obj)==T_ARRAY"
  end
end


def cast_numeric
  CastNum.new("cast_numeric").result
end

def cast_array
  CastArray.new("cast_array").result
end

def cast_from(cname,dtype,macro)
  if $type_name != cname.downcase
    Cast.new("cast_from",cname.downcase,dtype,"c"+cname,macro).result
  end
end

#def cast_from_real(cname,dtype)
#  Cast.new("cast_from_real",cname.downcase,dtype,"c"+cname).result
#end

# ----------------------------------------------------------------------

class Preproc < PutERB
  INIT = []
  OPMAP = {
    "add"=>"+", "sub"=>"-", "mul"=>"*", "div"=>"/",
    "mod"=>"%", "pow"=>"**", "minus"=>"-@", "plus"=>"+@",
    "and"=>"&",
    "or"=>"|",
    "xor"=>"^",
    "not"=>"~@"
  }

  def self.alias(dst,src)
    INIT << "rb_define_alias(cT, \"#{dst}\", \"#{src}\");"
  end

  def initialize(op,tmpl)
    @op=op
    @tmpl=tmpl
  end
  attr_reader :op

  def c_instance_method
    "nary_#{tp}_#{op}"
  end

  def c_singleton_method
    "nary_#{tp}_s_#{op}"
  end

  def c_iterator
    "iter_#{tp}_#{op}"
  end

  def rb_define_singleton_method(n)
    "rb_define_singleton_method(cT, \"#{op}\", #{c_singleton_method}, #{n});"
  end

  def rb_define_method(n)
    "rb_define_method(cT, \"#{op_map}\", #{c_instance_method}, #{n});"
  end

  def rb_define_math(n)
    "rb_define_singleton_method(mTM, \"#{op}\", #{c_singleton_method}, #{n});"
  end

  def op_map
    OPMAP[op] || op
  end

  def def_singleton(n=0)
    INIT << rb_define_singleton_method(n)
    erb
  end

  def def_binary
    INIT << rb_define_singleton_method(2)
    INIT << rb_define_method(1)
    erb
  end

  def def_method(n=0)
    INIT << rb_define_method(n)
    erb
  end

  def def_func
    erb
  end

  def def_math(n=1)
    INIT << rb_define_math(n)
    erb
  end
end

class PreprocQsort < Preproc
  def initialize(tp,dtype,dcast,tmpl)
    @tp=tp
    @dtype=dtype
    @dcast=dcast
    @tmpl=tmpl
  end
  attr_reader :dtype, :dcast, :tp
end

class Preproc2 < Preproc
  def initialize(op,dtype,tpclass,tmpl)
    @op=op
    @dtype=dtype
    @tpclass=tpclass
    @tmpl=tmpl
  end
  attr_reader :dtype, :tpclass
end


def def_singleton(ope,n=0)
  Preproc.new(ope,ope).def_singleton(n)
end

def def_method(tmpl,n=0)
  Preproc.new(tmpl,tmpl).def_method(n)
end

def binary(ope)
  Preproc.new(ope,"binary").def_binary
end

def pow
  Preproc.new("pow","pow").def_binary
end

def unary(ope)
  Preproc.new(ope,"unary").def_method
end

def unary2(ope,dtype,tpclass)
  Preproc2.new(ope,dtype,tpclass,"unary2").def_method
end

def set2(ope,dtype,tpclass)
  Preproc2.new(ope,dtype,tpclass,"set2").def_method(1)
end

def cond_binary(ope)
  Preproc.new(ope,"cond_binary").def_binary
end

def cond_unary(ope)
  Preproc.new(ope,"cond_unary").def_method
end

def bit_binary(ope)
  Preproc.new(ope,"bit_binary").def_method(1)
end

def bit_unary(ope)
  Preproc.new(ope,"bit_unary").def_method
end

def bit_count(ope)
  Preproc.new(ope,"bit_count").def_method(-1)
end

def accum(ope)
  Preproc.new(ope,"accum").def_method(-1)
end

def qsort(tp,dtype,dcast)
  PreprocQsort.new(tp,dtype,dcast,"qsort").def_func
end

def def_func(ope,tmpl)
  Preproc.new(ope,tmpl).def_func
end

def def_alias(dst,src)
  Preproc.alias(dst,src)
end

def put_init
  file="#{$dir}/init.c"
  puts "\n// from #{file}"
  ERB.new(File.read(file)).run
end

$upcast = []
def upcast(c,t="T")
  $upcast << "rb_hash_aset(hCast, c#{c}, c#{t});"
end


def math(ope,n=1)
  $math = true
  case n
  when 1
    Preproc.new(ope,"unary_s").def_math(n)
  when 2
    Preproc.new(ope,"binary_s").def_math(n)
  when 2
    Preproc.new(ope,"ternary_s").def_math(n)
  end
end
