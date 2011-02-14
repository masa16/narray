require "erb"
require "./preproc"

#load ARGV[0]
define_type  "Bit", ""

$has_math   = false
$is_bit     = true
$is_int     = false
$is_float   = false
$is_complex = false
$is_object  = false
$is_real    = false
$is_comparable = false


ERB.new(File.read("erb/bit_head.c")).run

def_method "extract"

cast_numeric
cast_array

def_singleton "cast", 1

def_method "coerce_cast",1

def_method "to_a"
def_method "fill",1
def_method "format",-1
def_method "format_to_a",-1
def_method "inspect"

cond_binary "eq"
cond_binary "ne"

# int
bit_binary "bit_and"
bit_binary "bit_or"
bit_binary "bit_xor"
bit_unary  "bit_not"

bit_count  "count_true"
bit_count  "count_false"

def_alias  "count_1", "count_true"
def_alias  "count_0", "count_false"

put_init
