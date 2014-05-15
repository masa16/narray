=begin

= Units-math library `Quanty' was renamed to ((<Phys-Units|URL:https://github.com/masa16/phys-units>)).

= class Quanty

== SYNOPSIS

A class whose instances consist of "value (physical quantity)" and "unit".
Featuring:
* Unit conversion like ((%units%)) command.
* Operation between quantities having different units,
  like ((*km*)) and ((*mile*)), with automatic unit conversion.

== EXAMPLE

  require 'quanty'
  Quanty(1.23,'km') + Quanty(4.56,'m')    #=> Quanty(1.23456,'km')
  Quanty(123,'mile') / Quanty(2,'hr')     #=> Quanty(61,'mile / hr')
  Quanty(61,'miles/hr').want('m/s')       #=> Quanty(27.26944,'m/s')
  Quanty(1.0,'are') == Quanty(10,'m')**2  #=> true
  Math.cos(Quanty(60,'degree'))           #=> 0.5

== Quanty class

=== Super Class:
Object (Numeric is better??)

=== Class method:
--- Quanty.new([value],[unit])
--- Quanty([value],[unit])
Create Quanty class instance having ((|value|)) and ((|unit|)) (is String).
If ((|value|)) is omitted,  ((|value|)) = 1 is assumed.
If ((|unit|)) is omitted,  ((|unit|)) = "" is assumed,
which is regarded as a quantity with "dimensionless" unit
(i.e. all dimensions of unit are zero).
Refer to ((<Notation of unit>)) below.

=== Methods:
--- self + other
--- self - other
Addition and subtraction of quantities.
Operation is made after
the unit of ((|other|)) is converted to the unit of ((|self|)).
Exception is raised if unit conversion is failed.
Return the Quanty class instance with the unit of ((|self|)).

--- self * other
Multiplication of quantities.
Resulting unit is made by concatenating ((|self|)) and ((|other|)).

--- self / other
Division of quantities.
Resulting unit is made by placing (({"/"}))
between ((|self|)) and ((|other|)), and concatenating them.

--- self ** number
Power of quantities.
Resulting unit is made by "(unit of self)^((|number|))"

--- self == other
--- self < other
--- self <= other
--- self > other
--- self >= other
Comparison of quantities.

--- coerce(number)
Convert ((|number|)) to Quanty class instance with dimensionless unit,
then return [((|number|)), ((|self|))].

--- to_si
--- to_SI
Convert ((|self|)) to the SI unit system.

--- to_f
If ((|self|)) is a quantity with a dimensionless unit, return its value.
If ((|self|)) is a quantity with an angular unit,
return the value converted into radian.
Otherwise, raise exception.

--- unit
Return the string of unit.

--- val
--- value
Return the value.

--- want(unit)
Convert ((|self|)) to a quantity with ((|unit|)) (is String),
and return Quanty class instance.

== Notation of unit

==== Multiplication
 'N m' == 'N*m'

==== Division
 '/s' , 'm/s'

==== Power
 'm-2' == 'm^-2' == 'm**-2'

==== Numerical factor
 '12 inch' == 'feet'

==== Combination rule
 'm/s*m' == 'm^2/s'.
 'm/(s*m)' ==  '/s'.

+ See ((%parse.y%)) for more.

== Author
((<Masahiro Tanaka|URL:http://www.ir.isas.ac.jp/~masa/index-e.html>))
(2001-04-25)
=end
