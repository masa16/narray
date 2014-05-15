require "./version"

while /^--/ =~ ARGV[0]
  a = ARGV.shift
  case a
  when /^--title=(.*)/
    $title = $1
  when /^--lang=(.*)/
    $lang = $1
  when /^--kw=(.*)/
    $kw = $1
  when /^--rt/
    $rt = true
  end
end

rdfile = ARGV.shift
$date  = File.mtime(rdfile).strftime("%b %d %Y")

case $lang
when 'ja'
  rd2 = 'rd2 -r ./rd2html-img-lib.rb --html-lang=ja --with-css=./na.css --out-code=jis'
else
  rd2 = 'rd2 -r ./rd2html-img-lib.rb --html-lang=en --with-css=./na.css'
end

rd2 << " --html-title='#{$title}'"  if $title
rd2 << " -r rd/rt-filter --with-part=RT:rt"  if $rt

IO.popen("#{rd2} #{rdfile}", "r") do |f|
  until f.eof
    s = f.readline
    s.sub!(%r|</head>|io) do |x|
      x = %|<meta name="Keywords" content="#{$kw}"/>\n| + x  if $kw
      x = %|<meta name="author" content="Masahiro Tanaka"/>\n| + x
      x
    end
    s.sub!(%r|</body>|io,
	   "<div>Last update: #{$date}</div>\n</body>")
    s.gsub!(/_narray_version_/, NARRAY_VERSION)
    s.gsub!(/_narray_release_date_/, NARRAY_RELEASE_DATE)
    puts s
  end
end
