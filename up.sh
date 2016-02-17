webdir=masa@rubyforge:/var/www/gforge-projects/narray/

sftp rubyforge <<EOF
cd /var/www/gforge-projects/narray
put index.html
put index.html.en
put index.html.ja
put SPEC.en
put SPEC.ja
put ruby4science.ja.html
cd quanty
lcd quanty
put na.css
put quanty-ja.html
put quanty-en.html
quit
EOF
