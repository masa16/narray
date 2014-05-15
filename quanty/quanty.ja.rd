=begin

= 単位変換ライブラリ Quanty は ((<Phys-Units|URL:https://github.com/masa16/phys-units>)) に変更されました。

= class Quanty

== 概要

「値 (物理量)」と「単位」を併せ持つクラス。
((%units%)) コマンドのような単位変換機能の他、
((*km*)) と ((*mile*)) など、異なる単位を持つ量の演算も可能。

== 例

  require 'quanty'
  Quanty(1.23,'km') + Quanty(4.56,'m')    #=> Quanty(1.23456,'km')
  Quanty(123,'mile') / Quanty(2,'hr')     #=> Quanty(61,'mile / hr')
  Quanty(61,'miles/hr').want('m/s')       #=> Quanty(27.26944,'m/s')
  Quanty(1.0,'are') == Quanty(10,'m')**2  #=> true
  Math.cos(Quanty(60,'degree'))           #=> 0.5

== Quanty クラス

=== スーパークラス:
    とりあえず Object。(Numericのほうがよいかも)

=== クラスメソッド:
--- Quanty.new([value],[unit])
--- Quanty([value],[unit])
量の値を((|value|))、単位を((|unit|)) (文字列)とする
Unit クラスのインスタンスを生成する。
((|value|))が省略された時は、1が指定された場合と同じ。
((|unit|))が省略された時は、""が指定された場合と同じで、単位なし量になる。
単位の書き方は、下の((<単位表記法>))を参照。

=== メソッド:
--- self + other
--- self - other
量の加算・減算。
((|other|))の単位を((|self|))の単位に変換して演算する。
単位変換ができなければ例外が発生する。
結果を、((|self|))を単位にした Quanty クラスのインスタンスで返す。

--- self * other
量の乗算。
結果の単位は、((|self|))と((|other|))の単位を連結して作る。

--- self / other
量の除算。
結果の単位は、
((|self|))と((|other|))の単位を(({"/"}))をはさんで連結して作る。

--- self ** number
量の冪乗。
結果の単位は、"(((|self|))の単位)^((|number|))" として作る。

--- self == other
--- self < other
--- self <= other
--- self > other
--- self >= other
量の比較。

--- coerce(number)
((|number|))を単位なしQuantyクラスのインスタンスにし、
[((|number|)),((|self|))]として返す。

--- to_si
--- to_SI
((|self|))をSI単位系に変換する。

--- to_f
((|self|))が単位なし量の場合は、自身の値を返す。
((|self|))が角度の場合は、radianに変換した値を返す。
それ以外の単位の場合は、例外が発生する。

--- unit
単位の文字列を返す。

--- val
--- value
量の値を返す。

--- want(unit)
((|self|))を ((|unit|)) (文字列) を単位とした量に変換する。


== 単位表記法

==== 乗法
 'N m' == 'N*m'

==== 除法
 '/s' , 'm/s'

==== べき
 'm-2' == 'm^-2' == 'm**-2'

==== 数値倍
 '12 inch' == 'feet'

==== 結合順
 'm/s*m' == 'm^2/s'.
 'm/(s*m)' ==  '/s'.

+ 詳細は ((%parse.y%)) を参照のこと。

== Author
((<Masahiro Tanaka|URL:http://www.ir.isas.ac.jp/~masa/index-e.html>))
(2001-04-25)
=end
